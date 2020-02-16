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
#include <thread> // this_thread
#include <mutex>
#include <unordered_map>
#include <array>
#include <stdio.h>

#ifdef REMO_SYS_WIN
	#include <io.h> // _isatty
#else
	#include <unistd.h> // isatty
#endif


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


//------------------------------------------------------------------------------
// constants
//------------------------------------------------------------------------------	
//
//! prefix for identifying our traces
static const std::string library_prefix = "libremo: ";

//! colors used for different thread numbers
static const std::array<Color, 8> thread_colors = {
	Color::bright_white,
	Color::bright_red,
	Color::bright_green,
	Color::bright_yellow,
	Color::bright_blue,
	Color::bright_magenta,
	Color::bright_cyan,
	Color::bright_black
};


//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------	
//
//! helper functions for mapping thread ids to consecutive (short) numbers
static int get_thread_number(const std::thread::id& a_tid = std::this_thread::get_id());


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------	
//
//! initialize default log level
LogLevel Logger::s_global_level = eLogVerb;


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

	// get sequence number of current thread
	// we use a mapping instead of the actual thread id to keep the log lines compact
	int thread_num = get_thread_number();
	Color thread_color = thread_colors[thread_num % thread_colors.size()];

	/**
	 * build log entry
	 * 
	 * NOTE: Use stringstream instead of writing to
	 * our output stream directly, in order to:
	 * 1. avoid unnecessary flushes on stderr
	 * 2. avoid interleaved output from different threads
	 */
	std::stringstream ss;

	// output prefix and timestamp
	ss << library_prefix;
	// output timestamp
	ss << m_colors.colorize(
		"[" + sys::format_timestamp(ts) + "]", 
		m_invert ? Color::bright_white : Color::black, 
		m_invert ? Color::bright_black : Color::white
	) << " ";
	// output log level
	ss << "[" << m_colors.colorize(level, fcol, bcol, style) << "] ";
	// output thread id
	ss << "[" << m_colors.colorize('T' + std::to_string(thread_num), thread_color) << "] ";
	// output logger name and optional object name
	ss << "[" << m_colors.cyan(m_name);
	if (!a_log_name.empty()) {
		ss << ":" << m_colors.bright_magenta(a_log_name);
	}
	ss << "] ";
	// output message
	ss << m_colors.colorize(message, fcol, bcol, style);
	// output trailing newline
	ss << std::endl;

	// and finally output everything at once
	m_out << ss.str();
}


//------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------	
//
static int get_thread_number(const std::thread::id& a_tid)
{
	// static variables
	typedef std::unordered_map<std::thread::id, int> Map;
	static Map map;
	static std::mutex mutex;

	// thread number to be determined
	int thread_num = -1;

	// critical section
	const std::lock_guard<std::mutex> lock(mutex);
	
	// lookup thread id
	auto it = map.find(a_tid);
	if (it == map.end()) {
		// unknown thread -> give it a sequence number and add it to our map
		thread_num = (int)map.size();
		map.insert(Map::value_type(a_tid, thread_num));
	} else {
		// known thread -> return sequence number
		thread_num = it->second;
	}

	return thread_num;
}

//------------------------------------------------------------------------------	
//
std::string get_log_name() {
	// https://stackoverflow.com/questions/26587110/returning-an-empty-string-efficient-way-in-c
	return std::string();
}


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
