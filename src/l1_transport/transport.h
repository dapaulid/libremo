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
#include "utils/settings.h"
//
// C++ 
#include <unordered_set>
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
	//! class specific settings go here
	struct Settings: public utils::Settings {

	} settings;

	//! callback type for accept events
	typedef std::function<void(Channel*)> accept_handler;

// ctor/dtor
public:
	Transport(const Settings& a_settings);
	virtual ~Transport();

// public member functions
public:
	//! create a new channel that connects to the given endpoint
	virtual Channel* connect(const std::string& a_endpoint) = 0;

	//! register a callback function that is invoked when an incoming connection was established
	void on_accept(const accept_handler& a_handler);

	//! get a new packet from the pool. intended to be used by channels when receiving data
	packet_ptr take_packet();

// public member functions called by Channel & subclasses
public:
	//! handle incoming connection
	void accept(Channel* a_channel);
	//! handle close event
	virtual void closed(Channel* a_channel);

// protected member functions called by TcpThread
protected:
	friend class TcpThread;
	void close_channels();

// protected member functions
protected:
	//! adds a new channel to be handled by this transport
	//! takes ownership
	void add_channel(Channel* a_channel);
	// removes (and deletes) a channel from this transport
	void remove_channel(Channel* a_channel);

// private member functions
private:
	void alloc_packets();
	void remove_channels();


// private members
private:
	//! set of channels handled by this transport
	typedef std::unordered_set<Channel*> Channels;
	Channels m_channels;
	//! packet pool to avoid heap allocations
	RecyclingPool<Packet> m_packet_pool;
	//! callback function that is invoked when an incoming connection was established
	accept_handler m_accept_handler;
};

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
