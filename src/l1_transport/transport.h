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
#include "channel.h"
#include "packet.h"
#include "utils/recycling.h"
//
// C++ 
#include <functional>
#include <string>
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
class Transport
{
// types
public:
	//! callback type for accept events
	typedef std::function<void(Channel*)> accept_handler;

// ctor/dtor
public:
	Transport();
	virtual ~Transport();

// public member functions
public:
	//! create a new channel that connects to the given endpoint
	virtual Channel* connect(const std::string& a_endpoint) = 0;

	//! register a callback function that is invoked when an incoming connection was established
	void on_accept(const accept_handler& a_handler);

	//! get a new packet from the pool. intended to be used by channels when receiving data
	packet_ptr take_packet();

// private member functions
private:
	void alloc_packets();

// private members
private:
	//! packet pool to avoid heap allocations
	RecyclingPool<Packet> m_packet_pool;
	//! callback function that is invoked when an incoming connection was established
	accept_handler m_accept_handler;
};

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
