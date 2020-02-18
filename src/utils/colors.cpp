//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "colors.h"

#include "l0_system/system.h"

#include <stdlib.h>

#ifdef REMO_SYS_WIN
	#include <windows.h> // GetConsoleMode
	#include <io.h> // _isatty
#else
	#include <unistd.h> // isatty
#endif


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------	
//

//------------------------------------------------------------------------------	
//
Colors::Colors(std::ostream& a_out, UseColors a_use_colors):
	m_out(a_out),
	m_use_colors()
{
	// check environment for "auto" mode
	if (a_use_colors == UseColors::automatic) {
		std::string ev = sys::get_env("REMO_COLORS");
		if (ev == "always") {
			a_use_colors = UseColors::always;
		} else if (ev ==  "never") {
			a_use_colors = UseColors::never;
		} else if (ev == "automatic") {
			a_use_colors = UseColors::automatic;
		}
	}

	// determine if colors to be used
	if (a_use_colors == UseColors::always) {
		request_color_support();
		m_use_colors = true;
	} else if (a_use_colors == UseColors::never) {
		m_use_colors = false;
	} else {
		if (colors_requested()) {
			m_use_colors = request_color_support();
		} else {
			m_use_colors = false;
		}
	}
}

//------------------------------------------------------------------------------	
//
Colors::~Colors()
{
}

//------------------------------------------------------------------------------	
//
std::string Colors::colorize(const std::string& a_str, Color a_forecol, Color a_backcol, Style a_style)
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
		}
		colorized += "m" + a_str + "\x1B[0m";
		return colorized;
	} else {
		return a_str;
	}
}

//------------------------------------------------------------------------------	
//
bool Colors::colors_requested() const
{
	FILE* stream = nullptr;
	if (&m_out == &std::cout) {
		stream = stdout;
	} else if (&m_out == &std::cerr) {
		stream = stderr;
	} else {
		// no standard stream
		return false;
	}
#ifdef REMO_SYS_WIN
	return ::_isatty(::_fileno(stream)) != 0;
#else
	return ::isatty(::fileno(stream));
#endif
}

//------------------------------------------------------------------------------	
//
bool Colors::request_color_support()
{
#ifdef REMO_SYS_WIN
	// get standard device handle
	DWORD stdhandle = 0;
	if (&m_out == &std::cout) {
		stdhandle = STD_OUTPUT_HANDLE;
	} else if (&m_out == &std::cerr) {
		stdhandle = STD_ERROR_HANDLE;
	} else {
		// no standard stream
		return false;
	}

    // set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(stdhandle);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
    }

	// check current output mode
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return false;
    }

	// enable virtual terminal processing if disabled
	if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
    	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!SetConsoleMode(hOut, dwMode)) {
			return false;
		}
	}

	// success
	return true;
#else
	// Linux: Assume colors always supported for now
	return true;
#endif
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
