//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "channel.h"

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
static Logger logger("Channel");


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------	
//
Channel::Channel():
	m_state(State::closed),
	m_receive_handler(),
	m_state_handler()
{
}

//------------------------------------------------------------------------------	
//
Channel::~Channel()
{
	REMO_ASSERT(is_closed(), 
		"channel must be closed before destroying");
}

//------------------------------------------------------------------------------	
//
void Channel::on_receive(const Channel::receive_handler& a_handler)
{
	m_receive_handler = a_handler;
}

//------------------------------------------------------------------------------	
//
void Channel::on_state_changed(const Channel::state_handler& a_handler)
{
	m_state_handler = a_handler;
}

//------------------------------------------------------------------------------	
//
void Channel::change_state(State a_new_state)
{
	if (a_new_state != m_state) {
		REMO_INFO("Changed state from '%s' to '%s'",
			get_state_str(m_state), get_state_str(a_new_state));
		// set new state
		m_state = a_new_state;
		// notify observer
		if (m_state_handler) {
			m_state_handler(this, a_new_state);
		}
	}
}

//------------------------------------------------------------------------------	
//
const char* Channel::get_state_str(State a_state)
{
	switch (a_state) {
		case State::closed: return "closed";
		case State::opening: return "opening";
		case State::open: return "open";
		case State::closing: return "closing";
		case State::closed_by_peer: return "closed_by_peer";
		// no default
	}
	// should not be reached
	return "(unknown)";
}

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
