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
#include <unistd.h> // isatty


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
// helpers
//------------------------------------------------------------------------------	
//
namespace color {
	enum class Color {
		none           = 0,

		black          = 30,
		red            = 31,
		green          = 32,
		yellow         = 33,
		blue           = 34,
		magenta        = 35,
		cyan           = 36,
		white          = 37,

		bright_black   = 90,
		bright_red     = 91,
		bright_green   = 92,
		bright_yellow  = 93,
		bright_blue    = 94,
		bright_magenta = 95,
		bright_cyan    = 96,
		bright_white   = 97,
	};

	enum class Style {
		none           = 0,
		bold           = 1,
		underline      = 4
	};

	std::string colorize(const std::string& a_str, Color a_forecol, Color a_backcol = Color::none, Style a_style = Style::none);
	bool use_colors();

	#define DECL_COLOR_FUNC(COL) \
		std::string COL(const std::string& s, Style style = Style::none) { \
			return colorize(s, Color::COL, Color::none, style);            \
		}
	DECL_COLOR_FUNC(black)
	DECL_COLOR_FUNC(red)
	DECL_COLOR_FUNC(green)
	DECL_COLOR_FUNC(yellow)
	DECL_COLOR_FUNC(blue)
	DECL_COLOR_FUNC(magenta)
	DECL_COLOR_FUNC(cyan)
	DECL_COLOR_FUNC(white)
	DECL_COLOR_FUNC(bright_black)
	DECL_COLOR_FUNC(bright_red)
	DECL_COLOR_FUNC(bright_green)
	DECL_COLOR_FUNC(bright_yellow)
	DECL_COLOR_FUNC(bright_blue)
	DECL_COLOR_FUNC(bright_magenta)
	DECL_COLOR_FUNC(bright_cyan)
	DECL_COLOR_FUNC(bright_white)
	#undef DECL_COLOR_FUNC
}


//------------------------------------------------------------------------------
// class implementation
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
	std::cerr << library_prefix << "[" << color::bright_black(sys::get_timestamp()) << "] ";

	// output log level
	std::string level;
	color::Color fcol = color::Color::none;
	color::Color bcol = color::Color::none;
	color::Style style = color::Style::none;
	switch (a_level) {
	case eLogFatal:
		level = "fatal";
		fcol = color::Color::bright_white;
		bcol = color::Color::red;
		style = color::Style::underline;
		break;
	case eLogError:
		level = "error";
		fcol = color::Color::bright_red;
		style = color::Style::bold;
		break;
	case eLogWarning:
		level = "warn ";
		fcol = color::Color::yellow;
		break;
	case eLogInfo:
		level = "info ";
		fcol = color::Color::none;
		break;
	case eLogVerbose:
		level = "verb ";
		fcol = color::Color::bright_black;
		break;
	default:
		level = "L" + std::to_string(a_level);
		fcol = color::Color::none;
	}

	// output log level
	std::cerr << "[" << color::colorize(level, fcol, bcol, style) << "] ";
	// output logger name
	std::cerr << "[" << color::cyan(m_name) << "] ";
	// output message
	std::cerr << color::colorize(message, fcol, bcol, style) << std::endl;
}

//------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------	
//
namespace color {

	// colorizes the given string using Ansi escape codes
	std::string colorize(const std::string& a_str, Color a_forecol, Color a_backcol, Style a_style)
	{
		if (use_colors() && (a_forecol != Color::none || a_backcol != Color::none || a_style != Style::none)) {
			std::string colorized("\x1B[");
			const char* sep = "";
			if (a_forecol != Color::none) {
				colorized += std::to_string((int)a_forecol);
				sep = ";";
			}
			if (a_backcol != Color::none) {
				colorized += sep + std::to_string((int)a_backcol + 10);
				sep = ";";
			}
			if (a_style != Style::none) {
				colorized += sep + std::to_string((int)a_style);
				sep = ";";
			}
			colorized += "m" + a_str + "\x1B[0m";
			return colorized;
		} else {
			return a_str;
		}
	}

	// determines if colors should be used
	bool use_colors()
	{
		return ::isatty(::fileno(stderr));
	}
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
