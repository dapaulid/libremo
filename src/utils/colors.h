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
#include <iostream>


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------


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

enum class UseColors {
	never,
	always,
	automatic
};

//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
class Colors
{
public:
	Colors(std::ostream& a_out, UseColors a_use_colors = UseColors::automatic);
	~Colors();

	//! colorizes the given string using ANSI escape codes if supported and enabled
	std::string colorize(const std::string& a_str, Color a_forecol, 
		Color a_backcol = Color::none, Style a_style = Style::none);

	//! determines if colors are used
	bool use_colors() { return m_use_colors; };

public:
	//! shorthand wrapper functions for available colors
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

private:
	//! determines if colors are requested by the application
	bool colors_requested() const;
	//! attempts to enable color support and returns true is system supports colors
	bool request_color_support();

private:
	//! output stream on which colors are to be used
	std::ostream& m_out;
	//! flag indicating if colors are used
	bool m_use_colors;
};


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
