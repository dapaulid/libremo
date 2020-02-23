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
#include "l1_transport/channel.h"
#include "l0_system/socket.h"
//
// C++ 
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace trans {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------
//
class TcpTransport;


//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
class TcpChannel: public Channel
{
// ctor/dtor
public:
	TcpChannel(TcpTransport* a_transport);
	virtual ~TcpChannel();

// public member functions
public:
	//! send a packet over this channel
	//! may block if the send buffer is full
	virtual void send(packet_ptr& a_packet) override;
	
	//! close this channel
	virtual void close() override;

// private member functions
private:
	//! called when socket has data ready to receive
	void receive_chunk();
	//! ensure rx packet is allocated
	void prepare_rx_packet();

// private members
private:
	//! socket used for communication
	sys::Socket m_socket;
	//! current packet in progress of being received
	packet_ptr m_rx_packet;
};

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
