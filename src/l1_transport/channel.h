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
// system
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace trans {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
class Channel
{
// types
public:
	// callback type for receive events
	typedef std::function<void(Channel*, packet_ptr&)> receive_handler;

// ctor/dtor
public:
	Channel();
	virtual ~Channel();

// public member functions
public:
	//! send a packet over this channel
	virtual void send(packet_ptr& a_packet) = 0;
	//! register a callback function that is invoked when a packet was received by this channel
	void on_receive(const receive_handler& a_handler);

// private member functions
private:

// private members
private:
	//! callback invoked when a packet was received by this channel
	receive_handler m_receive_handler;
};

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
