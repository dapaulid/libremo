//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "system.h"

#include <sstream> // stringstream
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <iomanip> // put_time


//------------------------------------------------------------------------------
namespace remo {
namespace sys {
//------------------------------------------------------------------------------	

//------------------------------------------------------------------------------	
//
time_point get_timestamp()
{
    return std::chrono::system_clock::now();
}

//------------------------------------------------------------------------------	
//
int64_t micros_between(const time_point& a_now, const time_point& a_then)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(a_now - a_then).count();
}

//------------------------------------------------------------------------------	
//
std::string format_timestamp(const time_point& a_timestamp)
{
	// get local time (in seconds precision)
    time_t ttimestamp = std::chrono::system_clock::to_time_t(a_timestamp);	
    struct tm local_time{};
#if REMO_SYSTEM & REMO_SYS_WINDOWS
	localtime_s(&local_time, &ttimestamp);
#else
	localtime_r(&ttimestamp, &local_time);
#endif
    auto now_secs = std::chrono::system_clock::from_time_t(std::mktime(&local_time));

    // get microsecond difference
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(a_timestamp - now_secs).count();

	// format timestamp
    std::stringstream ss;
    ss << std::put_time(&local_time, "%H:%M:%S.") << std::setw(6) << std::setfill('0') << micros;
    return ss.str();
}

//------------------------------------------------------------------------------	
//
std::string get_env(const char* a_name)
{
    REMO_MSVC_WARN_SUPPRESS(4996)
    const char* ev = std::getenv(a_name);
    return ev ? std::string(ev) : std::string();
}

//------------------------------------------------------------------------------
} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
