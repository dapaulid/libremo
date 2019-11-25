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

#include "reader.h"
#include "writer.h"
#include "logger.h"
#include "local_endpoint.h"

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
void RemoteEndpoint::send_packet(Packet* a_packet)
{
    logger.info(">> %s", a_packet->to_string().c_str());

    // "loopback"
    receive_packet(a_packet);
}

//------------------------------------------------------------------------------	
//
void RemoteEndpoint::receive_packet(Packet* a_packet)
{
    logger.info("<< %s", a_packet->to_string().c_str());
    handle_packet(a_packet);
}

//------------------------------------------------------------------------------	
//
void RemoteEndpoint::handle_packet(Packet* a_packet)
{
    if (a_packet->get_size() == 0) {
        logger.warning("ignoring packet of size 0");
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
        logger.warning("ignoring packet of unknown type 0x%02X", type);
    }
}

//------------------------------------------------------------------------------	
//
void RemoteEndpoint::handle_call(Packet* a_packet)
{
    BinaryReader reader(*a_packet);
    reader.read_call();

    // call it
    TypedValue result = m_local->call(reader.get_function(), reader.get_args());

    Packet* reply = take_packet();
    BinaryWriter reply_writer(*reply);

    ArgList args = reader.get_args(); // TODO const? separate variable;
    reply_writer.write_result(result, args);

    send_packet(reply);
    
}

//------------------------------------------------------------------------------	
//
void RemoteEndpoint::handle_result(Packet* a_packet)
{
    // TODO use some data structure
    m_received_result = a_packet;
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
Packet* RemoteEndpoint::take_packet()
{
	Packet* packet = m_packet_pool.take();
	if (!packet) {
		throw error(ErrorCode::ERR_OUT_OF_PACKETS, 
			"out of packets. maybe some transport plugin leaking?");
	}
	return packet;
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------