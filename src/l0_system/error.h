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
	ERR_PARAM_TYPE_INVALID = 3,
	ERR_PARAM_NUM_MISMATCH = 4,
	ERR_FUNC_NAME_MISSING = 5,
	ERR_POINTER_NEEDS_SIZE = 6,
	ERR_MUST_RECYCLE = 7,
	ERR_CANNOT_RECYCLE = 8,
	ERR_OUT_OF_PACKETS = 9,
	ERR_BAD_PACKET = 10,
	ERR_INVALID_IDENTIFIER = 11,
	ERR_INVALID_ARRAY_LENGTH = 12,
	ERR_INVALID_VALUE_TYPE = 13,
	ERR_RESULT_TYPE_MISMATCH = 14,
	ERR_PARAM_TYPE_MISMATCH = 15,
	ERR_OUTPARAM_TYPE_MISMATCH = 16,
	ERR_ITEM_ALREADY_EXISTING = 17,
	ERR_ITEM_ALREADY_REGISTERED = 18,
	ERR_ITEM_NOT_FOUND = 19,
	ERR_RPC_NOT_FOUND = 20,
	ERR_BAD_VALUE_ACCESS = 21,

	ERR_GETADDRINFO_FAILED = 22,
	ERR_GETNAMEINFO_FAILED = 23
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
