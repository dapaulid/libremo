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

#include <stdexcept>

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------

enum ErrorCode {
	ERR_PACKET_FULL = 1,
	ERR_BAD_PACKET_ACCESS = 2,
	ERR_PARAM_TYPE_INVALID = 3
};

class error: public std::runtime_error
{
public:
	error(ErrorCode a_code, const char* a_message, ...);

	const char* what() const throw() {
    	return m_message;
    }

	ErrorCode code() const {
		return m_code;
	}

private:
	ErrorCode m_code;
	char m_message [256];
};

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
