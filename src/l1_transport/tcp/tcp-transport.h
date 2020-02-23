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
#include "l1_transport/transport.h"
#include "l0_system/socket.h"
#include "l0_system/worker.h"
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
// helper class declaration
//------------------------------------------------------------------------------
//
/**
 * This class handles the actual socket communication for all our channels.
 * 
 * The functionality is separated from TcpTransport in order to avoid
 * concurrent access to member variables.
 */
class TcpThread: public Worker
{

// ctor/dtor
public:
	TcpThread(TcpTransport* a_transport);
	virtual ~TcpThread();

// protected member functions
protected:
	//! handle communication
	virtual void action() override;

	//! called when server socket is ready to accept a new connection
	void handle_incoming_connection();
	//! called when the receiving end of the "pseudo queue" is ready to receive
	void handle_cmd();

// private members
private:
	//! our transport controller (owner)
	TcpTransport* m_transport;
	//! sockets handled by this thread
	SocketSet m_sockets;
	//! socket for accepting incoming connections
	Socket m_serversock;
	//! sockets used as "pseudo-queue" for poor man's inter-thread communication
	Socket m_ctrl_in;
	Socket m_ctrl_out;
};


//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
class TcpTransport: public Transport
{
// types
public:
	//! class specific settings go here
	struct Settings: public Transport::Settings {
		//! "server" socket address
		SockAddr listen_addr = SockAddr("0.0.0.0:1986");
		
	} settings;

// ctor/dtor
public:
	TcpTransport(const Settings& a_settings);
	virtual ~TcpTransport();

// public member functions
public:
	//! create a new channel that connects to the given endpoint
	virtual Channel* connect(const std::string& a_endpoint) override;

// private members
private:
	//! worker thread doing the socket communication
	TcpThread m_thread;
};


//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
