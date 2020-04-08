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

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
#include "l0_system/worker.h"
#include "utils/list.h"
//
// C++ 
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <functional>
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace utils {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// forward declaration
//------------------------------------------------------------------------------
//
class TimerThread;

//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
class Timer: public utils::ListItem
{
// public types
public:
	typedef std::function<void(Timer*)> callback;
	typedef std::chrono::steady_clock clock;
	typedef clock::time_point time_point;
	typedef clock::duration duration;

// ctor/dtor
public:
	Timer(duration a_interval, callback a_callback, TimerThread* a_thread);
	Timer(double a_interval, callback a_callback, TimerThread* a_thread);
	virtual ~Timer();

// public member functions
public:
	//! reset timer interval
	void reset();
	//! stop timer from firing
	void cancel();
	//! enable timer to fire again
	void restart();

	//! name for logging purposes
	const std::string& get_log_name() const { return m_log_name; }

// protected member functions called by TimerThread
protected:
	friend class TimerThread;
	//! invoke callback function
	void notify();
	//! get time when this timer is scheduled to fire
	time_point get_trigger_time() const;

// private members
private:
	TimerThread* m_thread;
	//! timer interval in seconds
	duration m_interval;
	//! function to call when timer interval elapsed 
	callback m_callback;
	//! lock for protecting shared state
	mutable std::mutex m_lock;
	//! time to invoke callback
	time_point m_trigger_time;
	//! name for logging purposes
	std::string m_log_name;
	//! counter used to generate object ids for logging purposes
	static std::atomic_uint s_id;
};


//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
class TimerThread: public sys::Worker
{
// ctor/dtor
public:
	TimerThread();
	virtual ~TimerThread();

// public member functions
public:
	void add(Timer* a_timer);
	void remove(Timer* a_timer);

	//! requests termination of the worker thread
	virtual void shutdown() override;

// protected member functions
protected:
	//! handle timers
	virtual void action() override;

// private member functions
private:
	// returns the timer with the earliest trigger time
	Timer* get_next_timer();

// private members
private:
	//! timers to handle
	List<Timer> m_timers;
	//! we're using a recursive mutex, to allow
	//! calling public member functions from the callback
	std::recursive_mutex m_lock;
	//! condition variable used for cancellable sleep
	//! use *_any to support recursive mutex
	std::condition_variable_any m_cv;
	//! flag indicating if sleep was cancelled
	bool m_cancel;
};


//------------------------------------------------------------------------------
	} // end namespace utils
} // end namespace remo
//------------------------------------------------------------------------------
