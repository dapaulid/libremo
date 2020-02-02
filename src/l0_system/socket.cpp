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

#include "error.h"

#include <sstream>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

//------------------------------------------------------------------------------
namespace remo {
namespace sys {
//------------------------------------------------------------------------------	

//------------------------------------------------------------------------------	
//
SockAddr::SockAddr(const std::string& a_str):
	m_storage()
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
		size_t delim = a_str.rfind(':');
		if ((delim != std::string::npos) && (a_str.find(':') == delim)) {
			// "host:port"
			host = a_str.substr(0, delim);
			service = a_str.substr(delim+1);
		} else {
			// "host"
			host = a_str;
		}
	}
	
	struct addrinfo* result = nullptr;
	int err = getaddrinfo(host.c_str(), service.c_str(), &hints, &result);
	if (err) {
		throw error(ErrorCode::ERR_GETADDRINFO_FAILED, "getaddrinfo(\"%s\", \"%s\") failed with code %d: %s",
			host.c_str(), service.c_str(), err, gai_strerror(err)
		);
	}

	memcpy(m_storage, result->ai_addr, result->ai_addrlen);

	// cleanup
	freeaddrinfo(result);
}

//------------------------------------------------------------------------------	
//
std::string SockAddr::to_string() const
{
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

    int err = getnameinfo(
		(sockaddr*)&m_storage, sizeof(m_storage), 
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
	switch (((sockaddr*)m_storage)->sa_family) {
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
		return ntohs(((sockaddr_in*)m_storage)->sin_port);
	case AddrFamily::IPv6: 
		return ntohs(((sockaddr_in6*)m_storage)->sin6_port);
	default:
		return 0;
	}
}


//------------------------------------------------------------------------------
} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
