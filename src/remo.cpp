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


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


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
} // end namespace remo
//------------------------------------------------------------------------------
