//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#pragma once

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
#include "packet.h"
//
// C++ 
#include <functional>
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace trans {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// constants
//------------------------------------------------------------------------------
//
//! special return value for determine_packet_size to indicate that
//! a packet is not yet complete
static const size_t PACKET_INCOMPLETE = 0;


//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------
//
class Transport;


//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
class Channel
{
// types
public:
	//! channel states
	enum class State {
		opening,
		open,
		closing,
		closed,		
		closed_by_peer
	};

	//! callback type for receive events
	typedef std::function<void(Channel*, packet_ptr&)> receive_handler;
	//! calback type for state change event
	typedef std::function<void(Channel*, State)> state_handler;

// ctor/dtor
public:
	Channel(Transport* a_transport, const std::string& a_log_name);
	virtual ~Channel();

// public member functions
public:
	//! send a packet over this channel
	//! may block if the send buffer is full
	void send(packet_ptr& a_packet);
	
	//! register a callback function that is invoked when a packet was received by this channel
	void on_receive(const receive_handler& a_handler);
	//! register a callback function that is invoked when the channel state changed
	void on_state_changed(const state_handler& a_handler);

	//! close this channel
	virtual void close() = 0;
	//! marks this socket as closed
	void closed();	

	State get_state() const { return m_state; }
	bool is_open() const { return m_state == State::open; }
	bool is_closed() const { return m_state == State::closed || m_state == State::closed_by_peer; }

	//! return the transport controller (owner) of this channel 
	Transport* get_transport() { return m_transport; }

	//! channel name for logging purposes
	virtual std::string get_log_name() const { return m_log_name; }	

	//! returns the name of the given state
	static const char* get_state_str(State a_state);

// protected member functions
protected:
	//! prepares a packet to be sent over this channel
	//! subclasses can override this to write headers, do masking etc.
	virtual void prepare_to_send(packet_ptr& a_packet) { (void)a_packet; };
	//! send a packet over this channel
	//! to be implemented by subclasses
	virtual void do_send(packet_ptr& a_packet) = 0;

	//! prepares a packet to be received from this channel
	//! subclasses can override this to consume headers, do unmasking etc.
	virtual void prepare_to_receive(packet_ptr& a_packet) { (void)a_packet; };
	//! receive a packet from this channel
	void receive(packet_ptr& a_packet);

	//! enters the specified state
	void enter_state(State a_new_state);

	//! determine the actual packet size. used for message framing
	virtual size_t determine_packet_size(const packet_ptr& a_packet);

// private member functions
private:

// private members
private:
	//! transport controller (owner)
	Transport* m_transport;
	//! name for logging purposes
	std::string m_log_name;
	//! channel state
	State m_state;
	//! callback invoked when a packet was received by this channel
	receive_handler m_receive_handler;
	//! callback invoked when the state channel changed
	state_handler m_state_handler;
};

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------

