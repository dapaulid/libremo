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
bool Logger::level_enabled(LogLevel a_level)
{
	return (a_level <= s_global_level) || (s_global_level == eLogAll);
}

//------------------------------------------------------------------------------	
//
void Logger::log(LogLevel a_level, const char* a_format, ...)
{
	// check log level
	if (!level_enabled(a_level)) {
		return;
	}

	// format message
	char message [1024];
	va_list args;
	va_start(args, a_format);
	vsnprintf(message, sizeof(message), a_format, args);
	va_end (args);	

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
} // end namespace remo
//------------------------------------------------------------------------------
