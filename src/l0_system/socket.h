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

#include <string>

//------------------------------------------------------------------------------
namespace remo {
namespace sys {
//------------------------------------------------------------------------------


enum class AddrFamily {
	Unspec,
	IPv4,
	IPv6,
	Invalid
};

//------------------------------------------------------------------------------
//
struct SockAddr
{
public:
	SockAddr(const std::string& a_str);

	void from_string(const std::string& a_str);
	std::string to_string() const;

	AddrFamily get_family() const;
	uint16_t get_port() const;

private:
	char m_storage [32];
};

//------------------------------------------------------------------------------
} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
