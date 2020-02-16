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

#include <chrono>
#include <string>


//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------	
//
#ifdef _WIN32
#define REMO_SYS_WIN
#endif

#ifdef REMO_SYS_WIN
	#define REMO_MSVC_WARN_SUPPRESS(code)   #pragma warning(suppress: code)
#else
	#define REMO_MSVC_WARN_SUPPRESS(code)
#endif

//------------------------------------------------------------------------------
namespace remo {
namespace sys {
//------------------------------------------------------------------------------	

//! shorthand for timestamp type
typedef std::chrono::system_clock::time_point time_point;

//! gets the current time
time_point get_timestamp();
//! returns elapsed time between two timestamps in microseconds
int64_t micros_between(const time_point& a_now, const time_point& a_then);
//! formats a timestamp in format HH:MM:SS.MICROS
std::string format_timestamp(const time_point& a_timestamp);

//! gets the value of the specified environment variable
std::string get_env(const char* a_name);

//------------------------------------------------------------------------------
} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
