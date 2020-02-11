//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "remote_endpoint.h"

#include "local_endpoint.h"
#include "../l1_transport/reader.h"
#include "../l1_transport/writer.h"
#include "../l0_system/logger.h"


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------

//! logger instance
static Logger logger("RemoteEndpoint");


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
RemoteEndpoint::RemoteEndpoint(LocalEndpoint* a_local):
	Endpoint(),
	m_local(a_local),
	m_packet_pool()
{
	alloc_packets();
}

//------------------------------------------------------------------------------
//
RemoteEndpoint::~RemoteEndpoint()
{
}

//------------------------------------------------------------------------------	
//
void RemoteEndpoint::send_packet(packet_ptr& a_packet)
{
    REMO_INFO(">> %s", a_packet->to_string().c_str());

    // "loopback"
    receive_packet(a_packet);
}

//------------------------------------------------------------------------------	
//
void RemoteEndpoint::receive_packet(packet_ptr& a_packet)
{
    REMO_INFO("<< %s", a_packet->to_string().c_str());
    handle_packet(a_packet);
}

//------------------------------------------------------------------------------	
//
void RemoteEndpoint::handle_packet(packet_ptr& a_packet)
{
    if (a_packet->get_size() == 0) {
        REMO_WARN("ignoring packet of size 0");
        return;
    }

    uint8_t type = a_packet->get_byte(0);
    switch (type) {
    case PacketType::packet_call:
        handle_call(a_packet);
        break;
    case PacketType::packet_result:
        handle_result(a_packet);
        break;
    default:
        REMO_WARN("ignoring packet of unknown type 0x%02X", type);
    }
}

//------------------------------------------------------------------------------	
//
void RemoteEndpoint::handle_call(packet_ptr& a_packet)
{
    BinaryReader reader(*a_packet);
    reader.read_call();

    // call it
    TypedValue result = m_local->call(reader.get_function(), reader.get_args());

    packet_ptr reply = take_packet();
    BinaryWriter reply_writer(*reply);

    reply_writer.write_result(result, reader.get_args());

    send_packet(reply);
    
}

//------------------------------------------------------------------------------	
//
void RemoteEndpoint::handle_result(packet_ptr& a_packet)
{
    // TODO use some data structure
    m_received_result = std::move(a_packet);
}

//------------------------------------------------------------------------------	
//
void RemoteEndpoint::alloc_packets()
{
    // pre-allocate packets
    for (size_t i = 0; i < PACKET_POOL_SIZE; i++) {
        m_packet_pool.add(new Packet());
    }
}

//------------------------------------------------------------------------------
//
packet_ptr RemoteEndpoint::take_packet()
{
	packet_ptr packet(m_packet_pool.take());
	if (!packet) {
		throw error(ErrorCode::ERR_OUT_OF_PACKETS, 
			"out of packets. maybe some transport plugin leaking?");
	}
	return packet;
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
