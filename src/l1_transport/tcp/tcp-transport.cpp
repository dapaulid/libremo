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
	m_thread.join();
}

//------------------------------------------------------------------------------	
//
Channel* TcpTransport::connect(const std::string& a_endpoint)
{
	// resolve endpoint name to socket address
	REMO_INFO("resolving endpoint '%s'...", 
		a_endpoint.c_str());
	SockAddr addr(a_endpoint);
	REMO_INFO("endpoint '%s' is at '%s'", 
		a_endpoint.c_str(), addr.to_string().c_str());

	// create new channel
	TcpChannel* channel = new TcpChannel(this, Socket(SockProto::TCP), false);
	// connect channel
	channel->connect(addr);
	// add to bookkeeping
	add_channel(channel);
	// listen on it
	m_thread.add_channel(channel);
	
	return channel;
}

//------------------------------------------------------------------------------	
//
void TcpTransport::closed(Channel* a_channel)
{
	// stop listening on this channel
	m_thread.remove_channel((TcpChannel*)a_channel);
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
	// setup server socket
	// do this here instead of asynchronously in thread startup,
	// to avoid tests connecting to socket before it is listening
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
TcpThread::~TcpThread()
{
}

//------------------------------------------------------------------------------	
//
void TcpThread::do_startup()
{
}

//------------------------------------------------------------------------------	
//
void TcpThread::action()
{
	REMO_VERB("polling %d sockets", m_sockets.count());
	m_sockets.poll();
}

//------------------------------------------------------------------------------	
//
void TcpThread::do_shutdown()
{
	// TODO shutdown necessary/useful? Windows returns WSAENOTCONN for this
	//m_ctrl_in.shutdown();
	//m_serversock.shutdown();
	m_sockets.remove(&m_ctrl_in);
	m_sockets.remove(&m_serversock);
	// close all channels
	m_transport->close_channels();
	
	// wait for sockets to properly disconnect
	while (m_sockets.count() > 0) {
		REMO_VERB("polling %d sockets (shutdown)", m_sockets.count());		
		m_sockets.poll();
	};
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
	} else {
		// shutdown sentinel received
		REMO_INFO("shutdown signal received");
		// set termination flag
		terminate();
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
//
void TcpThread::remove_channel(TcpChannel* a_channel)
{
	REMO_ASSERT(is_self(), "must be called from own thread");

	// remove channel socket to our set
	m_sockets.remove(a_channel->get_socket());
}

//------------------------------------------------------------------------------	
//
void TcpThread::shutdown()
{
	/**
	 * NOTE: do NOT call base to set termination flag, as we may then
	 * terminate before processing the command queue and miss some channels
	 * we need to close.
	 */
	REMO_INFO("requesting thread to terminate");
	// send sentinel to thread to wake it from poll()
	add_channel(nullptr);
}

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
