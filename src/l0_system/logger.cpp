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

#include "system.h"

#include <sstream> // stringstream
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
	if ((a_level > s_global_level) && (s_global_level != eLogAll)) {
		// yes -> filter it
		return;
	}

	// format message
	char message [1024];
	vsnprintf(message, sizeof(message), a_format, args);

	// output prefix and timestamp
	std::cerr << library_prefix << "[" << sys::get_timestamp() << "] ";

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
//
void Logger::custom(int a_level, const char* a_format, ...)
{
	va_list args;
	va_start(args, a_format);
	log(LogLevel(a_level), a_format, args);
	va_end (args);
}


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
