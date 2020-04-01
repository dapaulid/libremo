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
/**
 * system identifier bitsets
 * 
 * usage example:
 * #if REMO_SYSTEM & REMO_SYS_WINDOWS
 *   // code for Windows (32 or 64 bit) goes here
 * #endif
 */
#define REMO_SYS_UNKNOWN         0UL
// Unix family
#define REMO_SYS_POSIX           ((1UL << 0))
#define REMO_SYS_UNIX            ((1UL << 1) | REMO_SYS_POSIX)
#define REMO_SYS_LINUX           ((1UL << 2) | REMO_SYS_UNIX)
// Windows family
#define REMO_SYS_WINDOWS         ((1UL << 4))
#define REMO_SYS_WINDOWS_32      ((1UL << 5) | REMO_SYS_WINDOWS)
#define REMO_SYS_WINDOWS_64      ((1UL << 6) | REMO_SYS_WINDOWS)


//! determine actual system
#ifndef REMO_SYSTEM
	//! https://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor
	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
		#ifdef _WIN64
			#define REMO_SYSTEM REMO_SYS_WINDOWS_64
		#else
			#define REMO_SYSTEM REMO_SYS_WINDOWS_32
		#endif
	#elif __linux__
		// Linux
		#define REMO_SYSTEM REMO_SYS_LINUX
	#elif __unix__ // all unices not caught above
		// Unix
		#define REMO_SYSTEM REMO_SYS_UNIX
	#elif defined(_POSIX_VERSION)
		// POSIX
		#define REMO_SYSTEM REMO_SYS_POSIX
	#else
		// unknown system
		#define REMO_SYSTEM REMO_SYS_UNKNOWN
	#endif
#endif

//! platform specific macros
#if REMO_SYSTEM & REMO_SYS_WINDOWS
	#define REMO_MSVC_WARN_SUPPRESS(code) __pragma(warning(suppress: code))
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
