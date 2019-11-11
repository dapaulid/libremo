//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "remo.h"

#include "logger.h"

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

//! logger instance
static Logger logger("controller");


//------------------------------------------------------------------------------	
// constants
//------------------------------------------------------------------------------	
//
//! maximum number of packets available
//! NOTE: this was selected as an arbitrary value and probably depends
//!       on the "concurrencyness" of the system (e.g. max. number of threads)
const size_t PACKET_POOL_SIZE = 16;


//------------------------------------------------------------------------------	
// class Controller
//------------------------------------------------------------------------------	
//
Controller::Controller():
    m_packet_pool()
{
    // pre-allocate packets
    for (size_t i = 0; i < PACKET_POOL_SIZE; i++) {
        m_packet_pool.add(new Packet());
    }

}

//------------------------------------------------------------------------------	

Controller::~Controller()
{
  
}

//------------------------------------------------------------------------------	

void Controller::send_packet(Packet* a_packet)
{
    logger.info(">> %s", a_packet->to_string().c_str());
}

//------------------------------------------------------------------------------	

void Controller::receive_packet(Packet* a_packet)
{
    logger.info("<< %s", a_packet->to_string().c_str());
}

//------------------------------------------------------------------------------	

void Controller::handle_packet(Packet* a_packet)
{
    (void)a_packet;
}

//------------------------------------------------------------------------------	

void Controller::handle_call(Packet* a_packet)
{
    (void)a_packet;
}

//------------------------------------------------------------------------------	

void Controller::handle_result(Packet* a_packet)
{
    (void)a_packet;
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
