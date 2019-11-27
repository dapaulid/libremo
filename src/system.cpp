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
std::string get_timestamp()
{
	// get current time
    auto now = std::chrono::system_clock::now();

	// get local time (in seconds precision)
    time_t tnow = std::chrono::system_clock::to_time_t(now);	
    struct tm local_time{};
#ifdef REMO_SYS_WIN
	localtime_s(&local_time, &tnow);
#else
	localtime_r(&tnow, &local_time);
#endif
    auto now_secs = std::chrono::system_clock::from_time_t(std::mktime(&local_time));

    // get millisecond difference
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - now_secs).count();

	// format timestamp
    std::stringstream ss;
    ss << std::put_time(&local_time, "%H:%M:%S.") << std::setw(3) << std::setfill('0') << millis;
    return ss.str();
}

//------------------------------------------------------------------------------
} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
