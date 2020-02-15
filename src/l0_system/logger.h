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

// TODO good way to output sourcefile:line without redundancy to class name?
//#define __FILENAME__ (strrchr("/" __FILE__, '/') + 1)

// use macro instead of function to avoid unnecessary parameter evaluation
// NOTE: a_format is part of variadic parameter to avoid the error
// "ISO C++11 requires at least one argument for the "..." in a variadic macro"
#define REMO_LOG(a_level, /*a_format,*/ ...)                                   \
	if (logger.level_enabled((remo::LogLevel)a_level)) {                       \
		logger.log((remo::LogLevel)a_level, get_log_name(), __VA_ARGS__);      \
	}

#define REMO_FATAL(/*a_format,*/ ...) REMO_LOG(remo::LogLevel::eLogFatal, __VA_ARGS__)
#define REMO_ERROR(/*a_format,*/ ...) REMO_LOG(remo::LogLevel::eLogError, __VA_ARGS__)
#define REMO_WARN(/*a_format,*/ ...)  REMO_LOG(remo::LogLevel::eLogWarn, __VA_ARGS__)
#define REMO_NOTE(/*a_format,*/ ...)  REMO_LOG(remo::LogLevel::eLogNote, __VA_ARGS__)
#define REMO_INFO(/*a_format,*/ ...)  REMO_LOG(remo::LogLevel::eLogInfo, __VA_ARGS__)
#define REMO_VERB(/*a_format,*/ ...)  REMO_LOG(remo::LogLevel::eLogVerb, __VA_ARGS__)

enum LogLevel
{
	eLogNone     = -1,
	eLogFatal    = 0,
	eLogError    = 1,
	eLogWarn  = 2,
	eLogNote     = 3,
	eLogInfo     = 4,
	eLogVerb  = 5,
	eLogAll      = 9999
};

/**
 * Default declaration for function used in REMO_LOG macro for getting the object name.
 * The idea is that classes can implement their own member function with the same name,
 * shadowing this function, so that an object name can be logged along the category.
 */ 
std::string get_log_name();

//------------------------------------------------------------------------------
//
class Logger
{
public:
	Logger(const std::string& a_name);
	virtual ~Logger();

	void log(LogLevel a_level, const std::string& a_log_name, 
		const char* a_format, ...);
	
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
