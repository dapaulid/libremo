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

#include "system.h"

#include <string>

#include <stdarg.h> // va_list

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------

// use macro instead of function to avoid unnecessary parameter evaluation
// NOTE: a_format is part of variadic parameter to avoid the error
// "ISO C++11 requires at least one argument for the "..." in a variadic macro"
#define REMO_LOG(a_level, /*a_format,*/ ...)                                   \
	if (logger.level_enabled((remo::LogLevel)a_level)) {                       \
		logger.log((remo::LogLevel)a_level, __VA_ARGS__);                      \
	}

#define REMO_FATAL(/*a_format,*/ ...) REMO_LOG(remo::LogLevel::eLogFatal, __VA_ARGS__)
#define REMO_ERROR(/*a_format,*/ ...) REMO_LOG(remo::LogLevel::eLogError, __VA_ARGS__)
#define REMO_WARN(/*a_format,*/ ...)  REMO_LOG(remo::LogLevel::eLogWarning, __VA_ARGS__)
#define REMO_INFO(/*a_format,*/ ...)  REMO_LOG(remo::LogLevel::eLogInfo, __VA_ARGS__)
#define REMO_VERB(/*a_format,*/ ...)  REMO_LOG(remo::LogLevel::eLogVerbose, __VA_ARGS__)

enum LogLevel
{
	eLogNone     = -1,
	eLogFatal    = 0,
	eLogError    = 1,
	eLogWarning  = 2,
	eLogInfo     = 3,
	eLogVerbose  = 4,
	eLogAll      = 9999
};

//------------------------------------------------------------------------------
//
class Logger
{
public:
	Logger(const std::string& a_name);
	virtual ~Logger();

	void log(LogLevel a_level, const char* a_format, ...);
	
	bool level_enabled(LogLevel a_level);

	static LogLevel get_global_level() {
		return s_global_level;
	}

	static void set_global_level(LogLevel a_level) {
		s_global_level = a_level;
	}

private:
	std::string m_name;
	sys::time_point m_last_timestamp;
	bool m_invert;
	// time between two tracings for inverting background in microseconds
	int m_invert_threshold;

	static LogLevel s_global_level;	
};

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
