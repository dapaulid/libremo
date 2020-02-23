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
Transport::Transport():
	m_packet_pool(),
	m_accept_handler()
{
}

//------------------------------------------------------------------------------	
//
Transport::~Transport()
{
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
	// notify observer
	if (m_accept_handler) {
		m_accept_handler(a_channel);
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
		throw error(ErrorCode::ERR_OUT_OF_PACKETS, 
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
