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

#include <sstream> // stringstream
#include <stdio.h>

#ifdef REMO_SYS_WIN
	#include <io.h> // _isatty
#else
	#include <unistd.h> // isatty
#endif

#include <chrono>


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
LogLevel Logger::s_global_level = eLogVerb;


//------------------------------------------------------------------------------	
//
std::string get_log_name() {
	// https://stackoverflow.com/questions/26587110/returning-an-empty-string-efficient-way-in-c
	return std::string();
}


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------	
//
Logger::Logger(const std::string& a_name, std::ostream& a_out):
	m_name(a_name),
	m_out(a_out),
	m_colors(a_out, UseColors::automatic),
	m_last_timestamp(),
	m_invert(false),
	m_invert_threshold(5000) // microseconds
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
void Logger::log(LogLevel a_level,  const std::string& a_log_name, 
	const char* a_format, ...)
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

	// determine log level label and color
	std::string level;
	Color fcol = Color::none;
	Color bcol = Color::none;
	Style style = Style::none;
	switch (a_level) {
	case eLogFatal:
		level = "fatal";
		fcol = Color::bright_white;
		bcol = Color::red;
		style = Style::underline;
		break;
	case eLogError:
		level = "error";
		fcol = Color::bright_red;
		style = Style::bold;
		break;
	case eLogWarn:
		level = "warn ";
		fcol = Color::yellow;
		break;
	case eLogNote:
		level = "note ";
		fcol = Color::bright_white;
		style = Style::bold;
		break;
	case eLogInfo:
		level = "info ";
		fcol = Color::none;
		break;
	case eLogVerb:
		level = "verb ";
		fcol = Color::bright_black;
		break;
	default:
		level = "L" + std::to_string(a_level);
		fcol = Color::none;
	}

	// get current timestamp
	sys::time_point ts = sys::get_timestamp();
	if ( sys::micros_between(ts, m_last_timestamp) > m_invert_threshold) {
		m_invert = !m_invert;
	}
	m_last_timestamp = ts;

	// output prefix and timestamp
	m_out << library_prefix;
	// output timestamp
	m_out << m_colors.colorize(
		"[" + sys::format_timestamp(ts) + "]", 
		m_invert ? Color::bright_white : Color::black, 
		m_invert ? Color::bright_black : Color::white
	) << " ";
	// output log level
	m_out << "[" << m_colors.colorize(level, fcol, bcol, style) << "] ";
	// output logger name and optional object name
	m_out << "[" << m_colors.cyan(m_name);
	if (!a_log_name.empty()) {
		m_out << ":" << m_colors.bright_magenta(a_log_name);
	}
	m_out << "] ";
	// output message
	m_out << m_colors.colorize(message, fcol, bcol, style) << std::endl;
}


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
