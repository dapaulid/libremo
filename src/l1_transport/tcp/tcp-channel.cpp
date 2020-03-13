//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "tcp-channel.h"

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
#include "tcp-transport.h"
#include "utils/logger.h"
#include "utils/contracts.h"
//
// C++ 
#include <cstring> // memcpy
//
// system
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace trans {
//------------------------------------------------------------------------------	

//! logger instance
static Logger logger("TcpChannel");


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------	
//
using namespace sys;

//------------------------------------------------------------------------------	
//
TcpChannel::TcpChannel(TcpTransport* a_transport, Socket&& a_socket, bool a_incoming):
	Channel(a_transport, a_socket.get_log_name()),
	m_socket(std::move(a_socket)),
	m_rx_packet()
{
	// set callbacks
	m_socket.on_receive_ready(std::bind(&TcpChannel::receive_chunk, this));
	m_socket.on_disconnected(std::bind(&TcpChannel::closed, this));
	// TODO handle this on TX ready?
	if (a_incoming) {
		// enable non-blocking mode
		m_socket.set_blocking(false);
		// channel is open now
		enter_state(State::open);
	}
}

//------------------------------------------------------------------------------	
//
TcpChannel::~TcpChannel()
{
}

//------------------------------------------------------------------------------	
//
void TcpChannel::send(packet_ptr& a_packet)
{
	const char* ptr = a_packet->get_buffer();
	size_t size = a_packet->get_size();

	/**
	 * we may not be able to send the whole packet, e.g. because the send buffer is full.
	 * we therefore loop until all bytes are sent, waiting on the socket to become send
	 * ready to avoid burning CPU cycles.
	 * 
	 * Note that we cannot just give up and return after sending an incomplete packet,
	 * as this may mess up the receiver side.
	 */ 
	while (size > 0) {
		
		// send data over socket
		size_t bytes_sent = 0;
		Socket::IOResult result = m_socket.send(ptr, size, &bytes_sent);
		
		// consistency checks
		REMO_ASSERT(result != Socket::IOResult::PeerShutdown,
			"should not happen for send");
		REMO_ASSERT(bytes_sent <= size,
			"sent more bytes than in buffer");
		
		// wait if necessary
		if (result == Socket::IOResult::WouldBlock) {
			REMO_WARN("socket not ready for sending (e.g. tx buffer full), waiting for it to become ready...");
			m_socket.wait_send_ready();
			REMO_WARN("socket ready for sending");
		}
		
		// determine remaining buffer to send
		ptr += bytes_sent;
		size -= bytes_sent;
	}
}

//------------------------------------------------------------------------------	
//
void TcpChannel::receive_chunk()
{
	REMO_WARN("receive_chunk");
	// TODO add header in send

	REMO_PRECOND({
		REMO_ASSERT(!is_closed(), 
			"channel must not receive data while closed");
		REMO_ASSERT(!m_rx_packet || m_rx_packet->get_size() <= m_rx_packet->get_buffer_size(),
			"packet size must not exceed buffer size");
	});
	REMO_POSTCOND({
		REMO_ASSERT(!m_rx_packet || m_rx_packet->get_size() <= m_rx_packet->get_buffer_size(),
			"packet size must not exceed buffer size");
	});

	// allocate packet to be received if needed
	prepare_rx_packet();

	// determine remaining buffer to receive
	void* ptr = m_rx_packet->get_buffer() + m_rx_packet->get_size();
	size_t size = m_rx_packet->get_buffer_size() - m_rx_packet->get_size();
	
	// receive data from socket
	size_t bytes_received = 0;
	Socket::IOResult result = m_socket.recv(ptr, size, &bytes_received);

	// consistency checks
	REMO_ASSERT(bytes_received <= size,
		"received more bytes than buffer can hold");

	// handle peer shutdown
	if (result == Socket::IOResult::PeerShutdown) {
		// did we receive an incomplete packet?
		if (m_rx_packet->get_size() > 0) {
			// yes -> discard it
			REMO_WARN("peer disconnected after sending incomplete packet (%d bytes), discarding it",
				m_rx_packet->get_size());
			m_rx_packet.reset();	
		}
		// we're closed now
		closed();
		return;
	}

	if (result == Socket::IOResult::WouldBlock) {
		// should not happen, write warning to see if it does...
		REMO_WARN("socket recv() would still block after poll()");
		// it's okay, just go on
	}

	// update packet size
	m_rx_packet->set_size(m_rx_packet->get_size() + bytes_received);

	// packet complete?
	const size_t actual_size = determine_packet_size(m_rx_packet);
	if (actual_size == PACKET_INCOMPLETE) {
		// not yet
		return;
	}

	// consistency checks
	REMO_ASSERT(actual_size <= m_rx_packet->get_size(),
		"actual packet size must not exceed received size");

	// we have a complete packet!
	packet_ptr complete_packet = std::move(m_rx_packet);
	REMO_ASSERT(!m_rx_packet,
		"rx packet should be moved");

	// are there any excess bytes?
	size_t excess_bytes = complete_packet->get_size() - actual_size;
	if (excess_bytes > 0) {
		// yes -> they belong to the next packet we will receive, copy them over

		// allocate next packet to be received
		prepare_rx_packet();
		REMO_ASSERT(excess_bytes <= m_rx_packet->get_buffer_size(),
			"excess packet bytes must not exceed packet buffer size");
		
		// show warning, because this is not zero-copy anymore :(
		REMO_WARN("received consecutive packets on socket read, need to copy %d bytes",
			excess_bytes);
		
		// copy excess bytes to next packet
		m_rx_packet->set_size(excess_bytes);			
		std::memcpy(m_rx_packet->get_buffer(), 
			complete_packet->get_buffer() + actual_size, excess_bytes);
		
		// truncate excess bytes from complete packet
		complete_packet->set_size(actual_size);
	}

	// done
	receive(complete_packet);
}

//------------------------------------------------------------------------------	
//
void TcpChannel::connect(const SockAddr& a_addr)
{
	// connect the socket
	m_socket.connect(a_addr);
	// enable non-blocking mode
	m_socket.set_blocking(false);
	// channel is open now
	enter_state(State::open);
}

//------------------------------------------------------------------------------	
//
void TcpChannel::close()
{
	// enter 'closing' state
	enter_state(State::closing);
	// shutdown socket. This will cancel any pending poll() calls
	m_socket.shutdown();
}

//------------------------------------------------------------------------------	
//
void TcpChannel::prepare_rx_packet()
{
	// allocate packet if needed
	if (!m_rx_packet) {
		m_rx_packet = get_transport()->take_packet();
	}
}

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
