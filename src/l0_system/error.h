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

#include "utils/utils.h" // basename
#include "utils/likely.h"

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
	ERR_GETNAMEINFO_FAILED = 23,

	ERR_SOCKET_UNSUPPORTED_FAMILY = 24,
	ERR_SOCKET_UNSUPPORTED_TYPE = 25,
	ERR_SOCKET_UNSUPPORTED_PROTOCOL = 26,
	ERR_SOCKET_OPEN_FAILED = 27,
	ERR_SOCKET_CLOSE_FAILED = 28,
	ERR_SOCKET_CONNECT_FAILED = 29,
	ERR_SOCKET_BIND_FAILED = 30,
	ERR_SOCKET_POLL_FAILED = 31,
	ERR_SOCKET_SYSCALL_FAILED = 32,
	ERR_SOCKET_BUFFER_TOO_SMALL = 33,
	ERR_SOCKET_SEND_FAILED = 34,
	ERR_SOCKET_SEND_INCOMPLETE = 35,
	ERR_SOCKET_RECV_FAILED = 36,
	ERR_SOCKET_RECV_INCOMPLETE = 37,
	ERR_WORKER_BAD_THREAD_STATE = 38,
};

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------
//
//! throw an exception
#define REMO_THROW(code, /*msg,*/ ...) {                                       \
	auto e = remo::error(code, /*msg,*/ __VA_ARGS__);                          \
	REMO_EXCPT("exception at %s:%d: %s", __FILE__, __LINE__, e.what());        \
	throw e;	                                                               \
}
//! throw an exception without logging
#define REMO_THROW_NOLOG(code, /*msg,*/ ...) {                                 \
	auto e = remo::error(code, /*msg,*/ __VA_ARGS__);                          \
	throw e;	                                                               \
}

//! throw an exception if condition is fulfilled
#define REMO_THROW_IF(cond, code, /*msg,*/ ...) \
	if REMO_UNLIKELY(cond) { REMO_THROW(code, /*msg,*/ __VA_ARGS__); }


//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------
//
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
