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
#include "transport.h"
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
Channel::Channel(Transport* a_transport, const std::string& a_log_name):
	m_transport(a_transport),
	m_log_name(a_log_name),
	m_state(State::opening),
	m_receive_handler(),
	m_state_handler()
{
	REMO_INFO("creating channel %s", m_log_name.c_str());
}

//------------------------------------------------------------------------------	
//
Channel::~Channel()
{
	REMO_PRECOND({
		REMO_ASSERT(is_closed(), 
			"channel must be closed before destroying");
	});
	
	REMO_INFO("destroyed channel %s", m_log_name.c_str());
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
void Channel::send(packet_ptr& a_packet)
{
	prepare_to_send(a_packet);
	do_send(a_packet);
}

//------------------------------------------------------------------------------	
//
void Channel::receive(packet_ptr& a_packet)
{	
	prepare_to_receive(a_packet);
	// notify observer
	if (m_receive_handler) {
		m_receive_handler(this, a_packet);
	}
}

//------------------------------------------------------------------------------	
//
void Channel::closed()
{
	// enter closed state
	// an unsolicited close means that the peer closed the channel
	enter_state(m_state == State::closing ? State::closed : State::closed_by_peer);
	// notify controller
	m_transport->closed(this);
}


//------------------------------------------------------------------------------	
//
void Channel::enter_state(State a_new_state)
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
size_t Channel::determine_packet_size(const packet_ptr& a_packet)
{
	uint32_t payload_size = 0;

	// packet header complete?
	if (a_packet->get_size() >= sizeof(payload_size)) {
		// yes -> read payload size
		payload_size = *(uint32_t*)a_packet->get_data();
		// determine actual packet size
		size_t actual_packet_size = static_cast<size_t>(payload_size) + sizeof(payload_size);
		// packet complete?
		if (actual_packet_size <= a_packet->get_size()) {
			// yes -> return actual size
			return actual_packet_size;
		}
	}
	return PACKET_INCOMPLETE;
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
