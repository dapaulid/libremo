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
#include "utils/async.h"
//
// C++ 
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
TcpTransport::TcpTransport(const Settings& a_settings):
	Transport(a_settings),
	settings(a_settings),
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
	REMO_VERB("connecting to '%s'", a_endpoint.c_str());

	// create new channel
	TcpChannel* channel = new TcpChannel(this, Socket(SockProto::TCP), false);
	
	// resolve endpoint asynchronously to avoid blocking the caller
	utils::async([a_endpoint, channel](){
		// resolve endpoint name to socket address
		REMO_INFO("resolving endpoint '%s'...", 
			a_endpoint.c_str());
		SockAddr addr(a_endpoint);
		REMO_INFO("endpoint '%s' is at '%s'", 
			a_endpoint.c_str(), addr.to_string().c_str());
		// connect channel
		channel->connect(addr);
	});

	REMO_VERB("connect() returning");

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
	m_serversock(),
	m_ctrl_in(),
	m_ctrl_out()
{
}

//------------------------------------------------------------------------------	
//
TcpThread::~TcpThread()
{
}

//------------------------------------------------------------------------------	
//
void TcpThread::do_startup()
{
	// setup server socket
	REMO_INFO("setting up server socket");
	m_serversock = Socket(SockProto::TCP);
	m_serversock.set_blocking(false);
	m_serversock.on_receive_ready(std::bind(&TcpThread::handle_incoming_connection, this));
	m_serversock.set_reuse_addr(true);
	m_serversock.bind(m_transport->settings.listen_addr);
	m_serversock.listen();

	// setup inter-thread communication sockets
	REMO_INFO("setting up inter-thread communication");
	m_ctrl_in = Socket(SockProto::UDP);
	m_ctrl_in.set_blocking(false);
	m_ctrl_in.on_receive_ready(std::bind(&TcpThread::handle_cmd, this));
	m_ctrl_in.bind(SockAddr::localhost);
	m_ctrl_out = Socket(SockProto::UDP);
	m_ctrl_out.bind(SockAddr::localhost);
	m_ctrl_out.connect(m_ctrl_in.get_socket_addr());
	m_ctrl_in.connect(m_ctrl_out.get_socket_addr());	

	// add special sockets to our set
	m_sockets.add(&m_ctrl_in);
	m_sockets.add(&m_serversock);
}

//------------------------------------------------------------------------------	
//
void TcpThread::do_shutdown()
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
	TcpChannel* channel = new TcpChannel(m_transport, m_serversock.accept(), true);
	// keep track of it
	do_add_channel(channel);
	// notify upper layers
	m_transport->accept(channel);
}

//------------------------------------------------------------------------------	
//
void TcpThread::handle_cmd()
{
	// "dequeue"
	TcpChannel* channel = nullptr;
	m_ctrl_in.recv(&channel, sizeof(channel));

	if (channel) {
		do_add_channel(channel);
	}
}

//------------------------------------------------------------------------------	
//
void TcpThread::add_channel(TcpChannel* a_channel)
{
	REMO_ASSERT(!is_self(), "must not be called from own thread");

	// "enqueue"
	m_ctrl_out.send(&a_channel, sizeof(a_channel));
}

//------------------------------------------------------------------------------	
//
void TcpThread::do_add_channel(TcpChannel* a_channel)
{
	REMO_ASSERT(is_self(), "must be called from own thread");

	// add channel socket to our set
	m_sockets.add(a_channel->get_socket());
}


//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
