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

using namespace sys;

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
	TcpChannel(TcpTransport* a_transport, Socket&& a_socket, bool a_incoming);
	virtual ~TcpChannel();

// public member functions
public:
	//! close this channel
	virtual void close() override;

	Socket* get_socket() { return &m_socket; }

// protected member functions
protected:
	//! prepares a packet to be sent over this channel
	virtual void prepare_to_send(packet_ptr& a_packet) override;
	//! send a packet over this channel
	virtual void do_send(packet_ptr& a_packet) override;

	//! prepares a packet to be received from this channel
	virtual void prepare_to_receive(packet_ptr& a_packet) override;	

// protected member functions called by TcpTransport
protected:
	friend class TcpTransport;
	//! connects the socket to the specified endpoint address
	void connect(const SockAddr& a_addr);

// private member functions
private:
	//! called when socket has data ready to receive
	void receive_chunk();
	//! ensure rx packet is allocated
	void prepare_rx_packet();

// private members
private:
	//! socket used for communication
	Socket m_socket;
	//! current packet in progress of being received
	packet_ptr m_rx_packet;
};

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
