//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "tcp-transport.h"

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
#include "tcp-channel.h"
#include "utils/logger.h"
//
// C++ 
#include <future> // async
//
// system
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace trans {
//------------------------------------------------------------------------------	

//! logger instance
static Logger logger("TcpTransport");


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------	
//
TcpTransport::TcpTransport():
	m_thread(this)
{
	m_thread.startup();
}

//------------------------------------------------------------------------------	
//
TcpTransport::~TcpTransport()
{
	m_thread.shutdown();
}

//------------------------------------------------------------------------------	
//
Channel* TcpTransport::connect(const std::string& a_endpoint)
{
	// create new channel
	TcpChannel* channel = new TcpChannel(this, Socket(SockProto::TCP));
	
	// resolve endpoint asynchronously to avoid blocking the caller
	std::async(std::launch::async, [&](){
		// resolve endpoint name to socket address
		REMO_INFO("resolving endpoint '%s'...", 
			a_endpoint.c_str());
		SockAddr addr(a_endpoint);
		REMO_INFO("endpoint '%s' is at '%s'", 
			a_endpoint.c_str(), addr.to_string().c_str());
		// connect channel
		channel->connect(addr);
	});

	return channel;
}

//------------------------------------------------------------------------------
// helper class implementation
//------------------------------------------------------------------------------	
//
TcpThread::TcpThread(TcpTransport* a_transport):
	Worker(),
	m_transport(a_transport),
	m_sockets(),
	m_serversock(SockProto::TCP),
	m_ctrl_in(SockProto::UDP),
	m_ctrl_out(SockProto::UDP)
{
	// setup server socket
	m_serversock.set_blocking(false);
	m_serversock.on_receive_ready(std::bind(&TcpThread::handle_incoming_connection, this));
	m_serversock.bind(SockAddr("0.0.0.0:1986")); // TODO make configurable
	m_serversock.listen();

	// setup inter-thread communication sockets
	m_ctrl_in.set_blocking(false);
	m_ctrl_in.on_receive_ready(std::bind(&TcpThread::handle_cmd, this));
	m_ctrl_in.connect(m_ctrl_out.get_socket_addr());
	m_ctrl_out.connect(m_ctrl_in.get_socket_addr());

	// add special sockets to our set
	m_sockets.add(&m_ctrl_in);
	m_sockets.add(&m_serversock);
}

//------------------------------------------------------------------------------	
//
TcpThread::~TcpThread()
{
}

//------------------------------------------------------------------------------	
//
void TcpThread::action()
{
	m_sockets.poll();
}

//------------------------------------------------------------------------------	
//
void TcpThread::handle_incoming_connection()
{
	// create new channel for accepted socket
	TcpChannel* channel = new TcpChannel(m_transport, m_serversock.accept());
	// add socket to our set
	m_sockets.add(&channel->get_socket());
	// notify upper layers
	m_transport->accept(channel);
}

//------------------------------------------------------------------------------	
//
void TcpThread::handle_cmd()
{

}



//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
