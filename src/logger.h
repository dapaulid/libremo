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

#include <string>

#include <stdarg.h> // va_list

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------

enum LogLevel
{
	eLogNone     = -1,
	eLogFatal    = 0,
	eLogError    = 1,
	eLogWarning  = 2,
	eLogInfo     = 3,
	eLogVerbose  = 4
};

//------------------------------------------------------------------------------
//
class Logger
{
public:
	Logger(const std::string& a_name);
	virtual ~Logger();

	void fatal(const char* a_format, ...);
	void error(const char* a_format, ...);
	void warning(const char* a_format, ...);
	void info(const char* a_format, ...);
	void verbose(const char* a_format, ...);

	static LogLevel get_global_level() {
		return s_global_level;
	}

	static void set_global_level(LogLevel a_level) {
		s_global_level = a_level;
	}

protected:
	virtual void log(LogLevel a_level, const char* a_format, va_list args);

	static std::string get_timestamp();

private:
	std::string m_name;

	static LogLevel s_global_level;	
};

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
