//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "logger.h"

#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <iostream>


//------------------------------------------------------------------------------
// constants
//------------------------------------------------------------------------------	
//
//! prefix for identifying our traces
static const std::string library_prefix = "libremo: ";


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

//! initialize default log level
LogLevel Logger::s_global_level = eLogVerbose;

//------------------------------------------------------------------------------	
//
Logger::Logger(const std::string& a_name):
	m_name(a_name)
{
}

//------------------------------------------------------------------------------	
//
Logger::~Logger()
{
}

//------------------------------------------------------------------------------	
//
void Logger::log(LogLevel a_level, const char* a_format, va_list args)
{
	// given level above global level?
	if (a_level > s_global_level) {
		// yes -> filter it
		return;
	}

	// format message
	char message [1024];
	vsnprintf(message, sizeof(message), a_format, args);

	// output prefix and timestamp
	std::cerr << library_prefix << "[" << get_timestamp() << "] ";

	// output log level
	switch (a_level) {
	case eLogFatal:
		std::cerr << "[fatal] ";
		break;
	case eLogError:
		std::cerr << "[error] ";
		break;
	case eLogWarning:
		std::cerr << "[warn]  ";
		break;
	case eLogInfo:
		std::cerr << "[info]  ";
		break;
	case eLogVerbose:
		std::cerr << "[verb]  ";
		break;
	default:
		std::cerr << "[L" << a_level << "] ";
	}

	// output logger name and message
	std::cerr << "[" << m_name << "] " << message << std::endl;
}

//------------------------------------------------------------------------------	
//
std::string Logger::get_timestamp()
{
	// get current time
    auto now = std::chrono::system_clock::now();

	// get local time (in seconds precision)
    time_t tnow = std::chrono::system_clock::to_time_t(now);	
    struct tm local_time{};
	::localtime_s(&local_time, &tnow);
    auto now_secs = std::chrono::system_clock::from_time_t(std::mktime(&local_time));

    // get millisecond difference
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - now_secs).count();

	// format timestamp
    std::stringstream ss;
    ss << std::put_time(&local_time, "%H:%M:%S.") << std::setw(3) << std::setfill('0') << millis;
    return ss.str();
}

//------------------------------------------------------------------------------	
//
void Logger::fatal(const char* a_format, ...)
{
	va_list args;
	va_start(args, a_format);
	log(eLogFatal, a_format, args);
	va_end (args);
}

//------------------------------------------------------------------------------	
//
void Logger::error(const char* a_format, ...)
{
	va_list args;
	va_start(args, a_format);
	log(eLogError, a_format, args);
	va_end (args);
}

//------------------------------------------------------------------------------	
//
void Logger::warning(const char* a_format, ...)
{
	va_list args;
	va_start(args, a_format);
	log(eLogWarning, a_format, args);
	va_end (args);
}

//------------------------------------------------------------------------------	
//
void Logger::info(const char* a_format, ...)
{
	va_list args;
	va_start(args, a_format);
	log(eLogInfo, a_format, args);
	va_end (args);
}

//------------------------------------------------------------------------------	
//
void Logger::verbose(const char* a_format, ...)
{
	va_list args;
	va_start(args, a_format);
	log(eLogVerbose, a_format, args);
	va_end (args);
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
