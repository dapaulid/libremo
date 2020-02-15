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

#include "system.h"

#include <string>
#include <functional>

//------------------------------------------------------------------------------
namespace remo {
namespace sys {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// types
//------------------------------------------------------------------------------
//
enum class AddrFamily {
	Unspec,
	IPv4,
	IPv6,
	Invalid
};

enum class SockProto {
	UDP,
	TCP
};

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------
//
static const int WAIT_FOREVER = -1;
static const int NO_WAIT = 0;

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------
//
class Socket;
class SocketSet;
struct SockAddr;

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------
//
class Socket
{
public:
	// callback type for ready events
	typedef std::function<void(Socket*)> ready_handler;
	
	// shutdown flags
	enum ShutdownFlag {
		ShutRd   = 0,
		ShutWr   = 1,
		ShutRdWr = 2
	};

public:
	Socket(SockProto a_proto, AddrFamily a_family = AddrFamily::Unspec);
	virtual ~Socket();

	void connect(const SockAddr& a_addr);

	void bind(const SockAddr& a_addr);
	void listen(int a_backlog = -1);
	Socket accept();

	size_t send(const void* a_buffer, size_t a_bufsize);
	size_t recv(void* a_buffer, size_t a_bufsize);

	void shutdown(ShutdownFlag how = ShutRdWr);

	void on_receive_ready(const ready_handler& a_handler);

	SockAddr get_socket_addr() const;
	SockAddr get_remote_addr() const;

	int get_fd() const { return m_sockfd; }

protected:
	Socket(int a_sockfd);

	void open(SockProto a_proto, AddrFamily a_family);
	void close();

	void set_sockfd(int asockfd);
	std::string get_log_name() const { return m_log_name; }

	//! event handlers
	virtual void receive_ready();

private:
	//! socket descriptor
	int m_sockfd;
private:
	friend SocketSet;
	//! ready event callbacks
	ready_handler m_receive_ready;
	//! socket name used for logging
	std::string m_log_name;
};

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------
//
class SocketSet
{
public:
	SocketSet();
	~SocketSet();

	void add(Socket* a_socket);
	void remove(Socket* a_socket);

	size_t poll(int a_timeout_ms = WAIT_FOREVER);

	size_t count() const;

private:
	struct impl;
	impl* pimpl;
};

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------
//
struct SockAddr
{
public:
	SockAddr();
	SockAddr(const std::string& a_str);

	void from_string(const std::string& a_str);
	std::string to_string() const;

	AddrFamily get_family() const;
	uint16_t get_port() const;

public:
	// some well-known addresses
	static const SockAddr localhost;

private:
	friend Socket;
	// opaque type corresponding to native sockaddr_storage
	// use uint64_t to enforce 8 byte alignment
	typedef uint64_t Storage [16];
	Storage m_addr;
	// actual number of bytes used in storage
#ifdef REMO_SYS_WIN
	typedef int socklen_t;
#else
	typedef unsigned int socklen_t;
#endif
	socklen_t m_addrlen;
};

//------------------------------------------------------------------------------
} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
