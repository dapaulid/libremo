//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "transport.h"

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
#include "utils/logger.h"
#include "utils/contracts.h"
//
// C++ 
//
// system
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace trans {
//------------------------------------------------------------------------------	

//! logger instance
static Logger logger("Transport");


//------------------------------------------------------------------------------	
// constants
//------------------------------------------------------------------------------	
//
//! maximum number of packets available
//! NOTE: this was selected as an arbitrary value and probably depends
//!       on the "concurrencyness" of the system (e.g. max. number of threads)
const size_t PACKET_POOL_SIZE = 16;


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------	
//
Transport::Transport(const Settings& a_settings):
	settings(a_settings),
	m_channels(),
	m_packet_pool(),
	m_accept_handler()
{
	alloc_packets();
}

//------------------------------------------------------------------------------	
//
Transport::~Transport()
{
	remove_channels();
}

//------------------------------------------------------------------------------	
//
void Transport::on_accept(const accept_handler& a_handler)
{
	m_accept_handler = a_handler;
}

//------------------------------------------------------------------------------	
//
void Transport::accept(Channel* a_channel)
{	
	// add to bookkeeping
	add_channel(a_channel);
	// notify observer
	if (m_accept_handler) {
		m_accept_handler(a_channel);
	}
}

//------------------------------------------------------------------------------	
//
void Transport::closed(Channel* a_channel)
{
	// implemented by subclasses
	(void)a_channel;
}

//------------------------------------------------------------------------------	
//
void Transport::add_channel(Channel* a_channel)
{
	REMO_PRECOND({
		REMO_ASSERT(m_channels.find(a_channel) == m_channels.end(),
			"channel must not yet exist");
	});

	m_channels.insert(a_channel);
}

//------------------------------------------------------------------------------	
//
void Transport::remove_channel(Channel* a_channel)
{
	REMO_PRECOND({
		REMO_ASSERT(m_channels.find(a_channel) != m_channels.end(),
			"channel must exist");
	});

	m_channels.erase(a_channel);
	delete a_channel;
}

//------------------------------------------------------------------------------	
//
void Transport::remove_channels()
{
	for (auto it = m_channels.begin(); it != m_channels.end(); it++) {
		delete *it;
	}
	m_channels.clear();
}

//------------------------------------------------------------------------------	
//
void Transport::close_channels()
{
	for (auto it = m_channels.begin(); it != m_channels.end(); it++) {
		(*it)->close();
	}
}

//------------------------------------------------------------------------------	
//
void Transport::alloc_packets()
{
    // pre-allocate packets
    for (size_t i = 0; i < PACKET_POOL_SIZE; i++) {
        m_packet_pool.add(new Packet());
    }
}

//------------------------------------------------------------------------------
//
packet_ptr Transport::take_packet()
{
	packet_ptr packet(m_packet_pool.take());
	if (!packet) {
		REMO_THROW(ErrorCode::ERR_OUT_OF_PACKETS, 
			"out of packets");
	}
	REMO_ASSERT(packet->get_size() == 0,
		"a fresh packet must be empty");
	return packet;
}


//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
