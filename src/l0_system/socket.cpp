//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "socket.h"

#include "system.h"
#include "error.h"
#include "utils/logger.h"
#include "utils/contracts.h"

#include <sstream>
#include <vector>

#include <string.h>


//------------------------------------------------------------------------------	
//
// include platform specific libraries
#ifdef REMO_SYS_WIN
	// Windows
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#pragma comment(lib, "Ws2_32.lib")

	// type aliases
	typedef ULONG nfds_t;
	// function aliases
	static const auto& close = closesocket;
	static const auto& poll = WSAPoll;
#else
	// UNIX / Linux
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/poll.h>
	#include <netinet/in.h>
	#include <netdb.h> // getaddrinfo
	#include <unistd.h> // close
	#include <errno.h>
	#include <fcntl.h>
#endif

//------------------------------------------------------------------------------	
// macros
//------------------------------------------------------------------------------	
//
//! map POSIX / errno macros to WSA error macros under Windows
//! example EWOULDBLOCK -> WSAEWOULDBLOCK
#ifdef REMO_SYS_WIN
	#define OS_ERROR(name) WSA ## name
#else
	#define OS_ERROR(name) name
#endif 

//------------------------------------------------------------------------------
namespace remo {
	namespace sys {
//------------------------------------------------------------------------------	

//! logger instance
static Logger logger("Socket");

//! constant indicating an invalid socket descriptor
static const int INVALID_SOCKFD = -1;

//------------------------------------------------------------------------------
// static variables
//------------------------------------------------------------------------------
//
#ifdef REMO_SYS_WIN
// singleton for initializing Winsock
static struct WinsockGuard {
	WinsockGuard() {
		WSADATA wsaData = {};
		int err = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err) {
      REMO_FATAL("WSAStartup failed with error: %d", err);
    }
	}
	~WinsockGuard() {
		::WSACleanup();
	}
} winsock_guard;
#endif // REMO_SYS_WIN

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------
//
static int get_last_error();
static std::string get_error_message(int err);
static const char* get_sdflag_str(Socket::ShutdownFlag sdf);


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------
//
// default constructor
Socket::Socket():
	m_sockfd(INVALID_SOCKFD),
	m_receive_ready(),
	m_log_name()
{
}

//------------------------------------------------------------------------------
//
// move constructor
Socket::Socket(Socket&& a_other):
	m_sockfd(a_other.m_sockfd),
	m_receive_ready(std::move(a_other.m_receive_ready)),
	m_log_name(std::move(a_other.m_log_name))
{
	// ensure descriptor is not closed twice
	a_other.m_sockfd = INVALID_SOCKFD;
}

//------------------------------------------------------------------------------
//
// move assignment
Socket& Socket::operator=(Socket&& a_other)
{
	close();

	m_sockfd = a_other.m_sockfd;
	m_receive_ready = std::move(a_other.m_receive_ready);
	m_log_name = std::move(a_other.m_log_name);
	
	// ensure descriptor is not closed twice
	a_other.m_sockfd = INVALID_SOCKFD;

	return *this;
}

//------------------------------------------------------------------------------
//
Socket::Socket(SockProto a_proto, AddrFamily a_family):
	Socket()
{
	// open socket for requested family
	open(a_proto, a_family);
}

//------------------------------------------------------------------------------
//
Socket::Socket(int a_sockfd):
	Socket()
{
	set_sockfd(a_sockfd);
}

//------------------------------------------------------------------------------
//
Socket::~Socket()
{
	close();
}

//------------------------------------------------------------------------------
//
void Socket::open(SockProto a_proto, AddrFamily a_family)
{
	// when family not specified, use family of resolved localhost address
	// TODO not sure if this is the best way to go
	if (a_family == AddrFamily::Unspec) {
		a_family = SockAddr::localhost.get_family();
	}

	int af = 0;
	int type = 0;
	int proto = 0;
	const char* af_str = "?";
	const char* proto_str = "?";

	switch (a_family) {
	case AddrFamily::Unspec:
		af = AF_UNSPEC;
		af_str = "unspec";
		break;
	case AddrFamily::IPv4:
		af = AF_INET;
		af_str = "IPv4";
		break;
	case AddrFamily::IPv6:
		af = AF_INET6;
		af_str = "IPv6";
		break;
	default:
		throw error(ErrorCode::ERR_SOCKET_UNSUPPORTED_FAMILY, 
			"Unsupported socket family: %d", a_family);
	}

	switch (a_proto) {
	case SockProto::UDP:
		type = SOCK_DGRAM;
		proto = IPPROTO_UDP;
		proto_str = "UDP";
		break;
	case SockProto::TCP:
		type = SOCK_STREAM;
		proto = IPPROTO_TCP;
		proto_str = "TCP";
		break;
	default:
		throw error(ErrorCode::ERR_SOCKET_UNSUPPORTED_PROTOCOL, 
			"Unsupported socket protocol: %d", a_proto);		
	}

	// ensure closed first
	close();

	// create socket descriptor
	int sockfd = (int)::socket(af, type, proto);
	if (sockfd < 0) {
		int err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_OPEN_FAILED, 
			"Opening socket failed with error %d: %s", 
			err, get_error_message(err).c_str());
	}

	// success
	set_sockfd(sockfd);
	REMO_INFO("socket #%d opened (%s/%s)", m_sockfd, proto_str, af_str);
}

//------------------------------------------------------------------------------
//
void Socket::close()
{
	if (m_sockfd == INVALID_SOCKFD) {
		// nothing to do
		return;
	}
	int err = ::close(m_sockfd);
	m_sockfd = INVALID_SOCKFD;
	if (err) {
		err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_CLOSE_FAILED, 
			"Closing socket failed with error %d: %s", 
			err, get_error_message(err).c_str());		
	}

	// success
	REMO_INFO("socket closed");
}

//------------------------------------------------------------------------------
//
void Socket::connect(const SockAddr& a_addr)
{
	REMO_VERB("socket connecting to '%s'",
		a_addr.to_string().c_str());

	// connect it
	int err = ::connect(m_sockfd, (const sockaddr*)&a_addr.m_addr, 
		a_addr.m_addrlen);
	if (err) {
		err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_CONNECT_FAILED, 
			"Connecting socket to '%s' failed with error %d: %s", 
			a_addr.to_string().c_str(), 
			err, get_error_message(err).c_str());		
	}

	// success
	REMO_INFO("socket connected: '%s' <-> '%s'",
		get_socket_addr().to_string().c_str(), 
		get_remote_addr().to_string().c_str());
}

//------------------------------------------------------------------------------
//
void Socket::bind(const SockAddr& a_addr)
{
	REMO_VERB("socket binding to '%s'",
		a_addr.to_string().c_str());

	// bind it
	int err = ::bind(m_sockfd, (const sockaddr*)&a_addr.m_addr, 
		a_addr.m_addrlen);
	if (err) {
		err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_BIND_FAILED, 
			"Binding socket to '%s' failed with error %d: %s", 
			a_addr.to_string().c_str(), 
			err, get_error_message(err).c_str());		
	}

	// success
	REMO_INFO("socket bound to '%s'",
		get_socket_addr().to_string().c_str());
}

//------------------------------------------------------------------------------
//
void Socket::listen(int a_backlog)
{
	// use system dependend default value if no backlog specified
	if (a_backlog < 0) {
		a_backlog = SOMAXCONN;
	}
	int err = ::listen(m_sockfd, a_backlog);
	if (err) {
		err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_SYSCALL_FAILED, 
			"Syscall listen() failed with error %d: %s", 
			err, get_error_message(err).c_str());		
	}

	// success
	REMO_INFO("socket listening (backlog=%d)",
		a_backlog);
}

//------------------------------------------------------------------------------
//
Socket Socket::accept()
{
	// we do not need the address of the peer socket, as can be fetched by
	// getpeername() afterwards, see 
	// https://stackoverflow.com/questions/22904179/difference-in-address-returned-by-getpeername-and-accept-c-socket
	int sockfd = (int)::accept(m_sockfd, nullptr, nullptr);
	if (sockfd < 0) {
		int err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_SYSCALL_FAILED, 
			"Syscall accept() failed with error %d: %s", 
			err, get_error_message(err).c_str());		
	}

	// create new socket
	Socket socket(sockfd);

	// success
	REMO_INFO("socket accepted connection from '%s' -> created socket %s",
		socket.get_remote_addr().to_string().c_str(), socket.get_log_name().c_str());
	return socket;
}

//------------------------------------------------------------------------------
//
Socket::IOResult Socket::send(const void* a_buffer, size_t a_bufsize, size_t* o_bytes_sent)
{
	if (o_bytes_sent) {
		*o_bytes_sent = 0;
	}

	int ret = ::send(m_sockfd, (const char*)a_buffer, (int)a_bufsize, 0);
	if (ret < 0) {
		int err = get_last_error();
		if (err == OS_ERROR(EWOULDBLOCK)) {
			REMO_VERB("socket send would block");
			return IOResult::WouldBlock;
		} else {
			throw error(ErrorCode::ERR_SOCKET_SEND_FAILED, 
				"Sending to '%s' failed with error %d: %s", 
				get_remote_addr().to_string().c_str(), 
				err, get_error_message(err).c_str());		
		}
	}
	size_t bytes_sent = (size_t)ret;

	// caller handles number of bytes sent?
	if (o_bytes_sent) {
		// yes -> return it
		*o_bytes_sent = bytes_sent;
	} else {
		// no -> caller expects whole buffer to be sent
		if (bytes_sent != a_bufsize) {
			throw error(ErrorCode::ERR_SOCKET_SEND_INCOMPLETE, 
				"Failed to send complete buffer: Expected %d bytes, actual %d bytes", 
				a_bufsize, bytes_sent);
		}
	}

	// success
	REMO_VERB("socket sent %d bytes (bufsize=%d)",
		bytes_sent, a_bufsize);
	return IOResult::Success;
}

//------------------------------------------------------------------------------
//
Socket::IOResult Socket::recv(void* a_buffer, size_t a_bufsize, size_t* o_bytes_received)
{
	if (o_bytes_received) {
		*o_bytes_received = 0;
	}

	int ret = ::recv(m_sockfd, (char*)a_buffer, (int)a_bufsize, 0);
	if (ret < 0) {
		int err = get_last_error();
		if (err == OS_ERROR(EWOULDBLOCK)) {
			REMO_VERB("socket receive would block");
			return IOResult::WouldBlock;
		} else {
			throw error(ErrorCode::ERR_SOCKET_SEND_FAILED, 
				"Receiving from '%s' failed with error %d: %s", 
				get_remote_addr().to_string().c_str(), 
				err, get_error_message(err).c_str());
		}
	}  else if (ret == 0) {
		// handle peer shutdown
		// TODO what about zero-length UDP datagrams?
		REMO_INFO("Peer performed orderly shutdown during recv()");
		return IOResult::PeerShutdown;
	}
	size_t bytes_received = (size_t)ret;

	// caller handles number of bytes received?
	if (o_bytes_received) {
		// yes -> return it
		*o_bytes_received = bytes_received;
	} else {
		// no -> caller expects whole buffer to be received
		if (bytes_received != a_bufsize) {
			throw error(ErrorCode::ERR_SOCKET_RECV_INCOMPLETE, 
				"Failed to receive complete buffer: Expected %d bytes, actual %d bytes", 
				a_bufsize, bytes_received);
		}
	}

	REMO_VERB("socket received %d bytes (bufsize=%zu)",
		bytes_received, a_bufsize);
	return IOResult::Success;
}

//------------------------------------------------------------------------------
//
void Socket::shutdown(ShutdownFlag how)
{
	int err = ::shutdown(m_sockfd, (int)how);
	if (err) {
		err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_SYSCALL_FAILED, 
			"Syscall shutdown() failed with error %d: %s", 
			err, get_error_message(err).c_str());		
	}

	REMO_INFO("socket shutdown (how=%s)",
		get_sdflag_str(how));
}

//------------------------------------------------------------------------------
//
void Socket::wait_send_ready(int a_timeout_ms)
{
	pollfd pfd {};
	pfd.fd = m_sockfd;
	pfd.events = POLLOUT;

	int ret = ::poll(&pfd, 1, a_timeout_ms);
	if (ret < 0) {
		int err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_POLL_FAILED, 
			"Polling socket for send ready failed with error %d: %s", 
			err, get_error_message(err).c_str());		
	}
}

//------------------------------------------------------------------------------
//
void Socket::set_blocking(bool a_blocking)
{
#ifdef REMO_SYS_WIN
   unsigned long mode = a_blocking ? 0 : 1;
   int ret = ::ioctlsocket(m_sockfd, FIONBIO, &mode);
   if (ret != 0) {
		int err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_SYSCALL_FAILED, 
			"Setting socket blocking mode using ioctlsocket() failed with error %d: %s", 
			err, get_error_message(err).c_str());
   }
#else
   int flags = ::fcntl(m_sockfd, F_GETFL, 0);
   if (flags == -1) {
		int err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_SYSCALL_FAILED, 
			"Getting socket flags using fcntl() failed with error %d: %s", 
			err, get_error_message(err).c_str());
   }
   flags = a_blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
   int ret = ::fcntl(m_sockfd, F_SETFL, flags);
   if (ret != 0) {
		int err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_SYSCALL_FAILED, 
			"Setting socket blocking mode using fcntl() failed with error %d: %s", 
			err, get_error_message(err).c_str());
   }
#endif

	REMO_INFO("socket set no %s mode",
		a_blocking ? "blocking" : "non-blocking");
}

//------------------------------------------------------------------------------
//
void Socket::set_reuse_addr(bool a_reuse_addr)
{
	const int enable = a_reuse_addr ? 1 : 0;
	int ret = ::setsockopt(m_sockfd, 
		SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(enable));
	if (ret < 0) {
		int err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_SYSCALL_FAILED, 
			"Syscall setsockopt(SO_REUSEADDR) failed with error %d: %s", 
			err, get_error_message(err).c_str());		
	}

	REMO_INFO("socket option SO_REUSEADDR %s",
		a_reuse_addr ? "set" : "cleared");
}

//------------------------------------------------------------------------------
//
void Socket::on_receive_ready(const ready_handler& a_handler)
{
	m_receive_ready = a_handler;
}

//------------------------------------------------------------------------------
//
SockAddr Socket::get_socket_addr() const
{
	SockAddr addr;
	addr.m_addrlen = sizeof(addr.m_addr);
	int err = ::getsockname(m_sockfd, (sockaddr*)&addr.m_addr, &addr.m_addrlen);
	if (err) {
		err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_SYSCALL_FAILED, 
			"Syscall getsockname() failed with error %d: %s", 
			err, get_error_message(err).c_str());		
	}
	return addr;
}

//------------------------------------------------------------------------------
//
SockAddr Socket::get_remote_addr() const
{
	SockAddr addr;
	addr.m_addrlen = sizeof(addr.m_addr);
	int err = ::getpeername(m_sockfd, (sockaddr*)&addr.m_addr, &addr.m_addrlen);
	if (err) {
		err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_SYSCALL_FAILED, 
			"Syscall getpeername() failed with error %d: %s", 
			err, get_error_message(err).c_str());		
	}
	return addr;
}

//------------------------------------------------------------------------------
//
void Socket::set_sockfd(int a_sockfd)
{
	m_sockfd = a_sockfd;
	m_log_name = "#" + std::to_string(m_sockfd);
}

//------------------------------------------------------------------------------
//
void Socket::receive_ready()
{
	REMO_VERB("socket ready to receive");

	// invoke callback
	if (m_receive_ready) {
		m_receive_ready();
	}
}


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------
//
struct SocketSet::impl {
	std::vector<Socket*> m_sockets;
	std::vector<pollfd> m_pollfds;
};

//------------------------------------------------------------------------------
//
SocketSet::SocketSet():
	pimpl(new impl)
{
}

//------------------------------------------------------------------------------
//
SocketSet::~SocketSet()
{
	delete pimpl;
}

//------------------------------------------------------------------------------
//
void SocketSet::add(Socket* a_socket)
{
	const size_t n = count();

	REMO_PRECOND({
		REMO_ASSERT(a_socket, 
			"socket must not be null");
	});
	REMO_POSTCOND({
		REMO_ASSERT(count() == n + 1, 
			"size must increase by one");
	});

	pollfd pfd {};
	pfd.fd = a_socket->get_fd();
	pfd.events = POLLIN;

	pimpl->m_sockets.push_back(a_socket);
	pimpl->m_pollfds.push_back(pfd);
}

//------------------------------------------------------------------------------
//
void SocketSet::remove(Socket* a_socket)
{
	const size_t n = count();

	REMO_PRECOND({
		REMO_ASSERT(a_socket, 
			"socket must not be null");
	});
	REMO_POSTCOND({
		REMO_ASSERT(count() == n - 1, 
			"size must decrease by one");
	});

	for (size_t i = 0; i < n; i++) {
		if (pimpl->m_sockets[i] == a_socket) {
			pimpl->m_sockets.erase(pimpl->m_sockets.begin() + i);
			pimpl->m_pollfds.erase(pimpl->m_pollfds.begin() + i);
			return;
		}
	}
}

//------------------------------------------------------------------------------
//
size_t SocketSet::poll(int a_timeout_ms)
{
	// wait for events
	const size_t n = count();
	int ret = ::poll(&pimpl->m_pollfds[0], (nfds_t)n, a_timeout_ms);
	REMO_ASSERT(n == count(),
		"set size must not change during poll");
	if (ret < 0) {
		int err = get_last_error();
		throw error(ErrorCode::ERR_SOCKET_POLL_FAILED, 
			"Polling sockets failed with error %d: %s", 
			err, get_error_message(err).c_str());		
	}

	REMO_ASSERT(pimpl->m_pollfds.size() == n,
		"incosistent array size");
	REMO_ASSERT(pimpl->m_sockets.size() == n,
		"incosistent array size");

	// check for events
	for (size_t i = 0; i < n; i++) {
		REMO_ASSERT((int)pimpl->m_pollfds[i].fd == pimpl->m_sockets[i]->get_fd(),
			"inconsistent socket descriptors");
		if (pimpl->m_pollfds[i].revents & POLLIN) {
			pimpl->m_sockets[i]->receive_ready();
		}
	}

	// return number of sockets that triggered at least one event
	return (size_t)ret;
}

//------------------------------------------------------------------------------
//
size_t SocketSet::count() const
{
	return pimpl->m_sockets.size();
}


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------
//
const SockAddr SockAddr::localhost("localhost");

//------------------------------------------------------------------------------
//
SockAddr::SockAddr():
	m_addr(), m_addrlen(0)
{
	// consistency checks
	// these go here in order to access private types
	static_assert(sizeof(SockAddr::Storage) >= sizeof(sockaddr_storage),
		"sockaddr_storage does not fit into SockAddr::m_storage, increase size in header");
	static_assert(alignof(SockAddr::Storage) == alignof(sockaddr_storage),
		"sockaddr_storage must have same alignment as SockAddr::m_storage");
}

//------------------------------------------------------------------------------
//
SockAddr::SockAddr(const std::string& a_str):
	SockAddr()
{
	from_string(a_str);
}

//------------------------------------------------------------------------------	
//
void SockAddr::from_string(const std::string& a_str)
{
	struct addrinfo hints = {};
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;    
	hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	std::string host;
	std::string service;

	size_t delim = a_str.rfind("]:");
	if (delim != std::string::npos) {
		// "[host]:port"
		host = a_str.substr(1, delim-1);
		service = a_str.substr(delim+2);
	} else {
		delim = a_str.rfind(':');
		if ((delim != std::string::npos) && (a_str.find(':') == delim)) {
			// "host:port"
			host = a_str.substr(0, delim);
			service = a_str.substr(delim+1);
		} else {
			// "host"
			host = a_str;
		}
	}

	// handle empty host -> any/wildcard address
	if (host.empty()) {
		const AddrFamily def_family = localhost.get_family();
		if (def_family == AddrFamily::IPv4) {
			host = "0.0.0.0";
		} else if (def_family == AddrFamily::IPv6) {
			host = "::";
		}
	}
	
	struct addrinfo* result = nullptr;
	int err = ::getaddrinfo(host.c_str(), service.c_str(), &hints, &result);
	if (err) {
		throw error(ErrorCode::ERR_GETADDRINFO_FAILED, "getaddrinfo(\"%s\", \"%s\") failed with code %d: %s",
			host.c_str(), service.c_str(), err, gai_strerror(err)
		);
	}

	// check buffer size
	if (result->ai_addrlen > sizeof(m_addr)) {
		throw error(ErrorCode::ERR_SOCKET_BUFFER_TOO_SMALL, 
			"Buffer to hold socket address too small: Expected %d bytes, got %d",
			sizeof(m_addr), result->ai_addrlen);
	}

	// copy address
	memcpy(m_addr, result->ai_addr, result->ai_addrlen);
	m_addrlen = (socklen_t)result->ai_addrlen;

	// cleanup
	freeaddrinfo(result);
}

//------------------------------------------------------------------------------	
//
std::string SockAddr::to_string() const
{
  char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

  int err = ::getnameinfo(
		(sockaddr*)&m_addr, m_addrlen, 
		hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), 
		NI_NUMERICHOST | NI_NUMERICSERV);
	if (err) {
		throw error(ErrorCode::ERR_GETNAMEINFO_FAILED, "getnameinfo() failed with code %d: %s",
			err, gai_strerror(err)
		);
	}

	// has port?
	if (get_port()) {
		// yes -> format host:port
		std::stringstream ss;
		if (strchr(hbuf, ':')) {
			ss << "[" << hbuf << "]:" << sbuf;
		} else {
			ss << hbuf << ":" << sbuf;
		}
		return ss.str();
	} else {
		// no -> host only
		return hbuf;
	}
}

//------------------------------------------------------------------------------	
//
AddrFamily SockAddr::get_family() const
{
	switch (((sockaddr*)m_addr)->sa_family) {
		case AF_UNSPEC: return AddrFamily::Unspec;
		case AF_INET  : return AddrFamily::IPv4;
		case AF_INET6 : return AddrFamily::IPv6;
		default       : return AddrFamily::Invalid;
	}
}

//------------------------------------------------------------------------------	
//
uint16_t SockAddr::get_port() const
{
	switch (get_family()) {
	case AddrFamily::IPv4: 
		return ntohs(((sockaddr_in*)m_addr)->sin_port);
	case AddrFamily::IPv6: 
		return ntohs(((sockaddr_in6*)m_addr)->sin6_port);
	default:
		return 0;
	}
}


//------------------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------------------
//
static int get_last_error()
{
#ifdef REMO_SYS_WIN
	return ::WSAGetLastError();
#else
	return errno;
#endif
}

//------------------------------------------------------------------------------
//
static std::string get_error_message(int err)
{
	// allocate enough buffer to hold message
	std::string msg(256, 0);
#ifdef REMO_SYS_WIN	
	::FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,   // flags
		NULL,                // lpsource
		err,                 // message id
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),    // languageid
		&msg[0],             // output buffer
		(DWORD)msg.size(),   // size of msgbuf, bytes
		NULL                 // va_list of arguments
	);
#else
	const char* ptr = strerror_r(err, &msg[0], msg.size());
	if (ptr != &msg[0]) {
		snprintf(&msg[0], msg.size(), "%s", ptr);
	}
#endif
	// get actual message length
	size_t len = msg.find('\0');
	if (len != std::string::npos) {
		// remove trailing whitespace (observed under Windows)
		while (len > 0 && msg[len-1] <= ' ') --len;
		// truncate to actual message length
		msg.resize(len);
	}
	// done
	return msg;
}

//------------------------------------------------------------------------------
//
static const char* get_sdflag_str(Socket::ShutdownFlag sdf)
{
	switch (sdf) {
		case Socket::ShutdownFlag::ShutRd: return "ShutRd";
		case Socket::ShutdownFlag::ShutWr: return "ShutWr";
		case Socket::ShutdownFlag::ShutRdWr: return "ShutRdWr";
		default: return "???";
	}
}

//------------------------------------------------------------------------------
	} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
