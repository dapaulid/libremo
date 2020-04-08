//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "timer.h"

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
#include "utils/logger.h"
//
// C++ 
//
// system
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace utils {
//------------------------------------------------------------------------------	

//! logger instance
static Logger logger("Timer");

//------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------	
//
static Timer::duration double_to_duration(const double& x)
{
	return std::chrono::duration_cast<Timer::duration>(std::chrono::duration<double>(x));
}

//------------------------------------------------------------------------------	
//
static double duration_to_double(const Timer::duration& x)
{
	return std::chrono::duration_cast<std::chrono::duration<double>>(x).count();
}

//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------	
//
std::atomic_uint Timer::s_id;

//------------------------------------------------------------------------------	
//
Timer::Timer(duration a_interval, callback a_callback, TimerThread* a_thread):
	m_thread(a_thread),
	m_interval(a_interval),
	m_callback(a_callback),
	m_lock(),
	m_trigger_time(),
	m_log_name("#" + std::to_string(++s_id))
{
	REMO_INFO("creating timer: interval=%0.6fs", 
		duration_to_double(m_interval));

	restart();
}

//------------------------------------------------------------------------------	
//
Timer::Timer(double a_interval, callback a_callback, TimerThread* a_thread):
	Timer(double_to_duration(a_interval), a_callback, a_thread)
{
}

//------------------------------------------------------------------------------	
//
Timer::~Timer()
{
	REMO_INFO("destroying timer");
	cancel();
}

//------------------------------------------------------------------------------	
//
void Timer::reset()
{
	std::lock_guard<std::mutex> lock(m_lock);
	m_trigger_time = clock::now() + m_interval;
}

//------------------------------------------------------------------------------	
//
void Timer::restart()
{
	reset();
	m_thread->add(this);
}

//------------------------------------------------------------------------------	
//
void Timer::cancel()
{
	m_thread->remove(this);
}

//------------------------------------------------------------------------------	
//
void Timer::notify()
{
	REMO_VERB("timer fired");
	m_callback();
}

//------------------------------------------------------------------------------	
//
Timer::time_point Timer::get_trigger_time() const
{
	std::lock_guard<std::mutex> lock(m_lock);
	return m_trigger_time;
}

//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------	
//
TimerThread::TimerThread():
	m_timers(),
	m_lock(),
	m_cv(),
	m_cancel(false)
{
	startup();
}

//------------------------------------------------------------------------------	
//
TimerThread::~TimerThread()
{
	shutdown();
	join();
}

//------------------------------------------------------------------------------	
//
void TimerThread::shutdown()
{
	Worker::shutdown();
	{
		std::lock_guard<decltype(m_lock)> lock(m_lock);
		m_cancel = true;
	}
	m_cv.notify_all();
}

//------------------------------------------------------------------------------	
//
void TimerThread::add(Timer* a_timer)
{
	{
		std::lock_guard<decltype(m_lock)> lock(m_lock);
		if (m_timers.contains(a_timer)) {
			// nothing to do
			return;
		}
		m_timers.push_back(a_timer);
		m_cancel = true;
	}
	m_cv.notify_all();
}

//------------------------------------------------------------------------------	
//
void TimerThread::remove(Timer* a_timer)
{
	{
		std::lock_guard<decltype(m_lock)> lock(m_lock);
		if (!m_timers.contains(a_timer)) {
			// nothing to do
			return;
		}
		m_timers.remove(a_timer);
		m_cancel = true;
	}
	m_cv.notify_all();
}

//------------------------------------------------------------------------------	
//
void TimerThread::action()
{
	// hold the lock. it will be released during wait on condition variable
	std::unique_lock<decltype(m_lock)> lock(m_lock);

	// repeat until a cancel signal received
	// this avoids the cost of unnecessary release/request cycles on the lock 
	// due to leaving this function
	while (!m_cancel) {
		// determine next timer to handle
		Timer* timer = get_next_timer();
		if (timer) {
			// move timer to back of list, to retrieve it easily after wait
			m_timers.move_to_back(timer);
			
			// wait for trigger time
			const Timer::time_point trigger_time = timer->get_trigger_time();
			std::cv_status status = m_cv.wait_until(lock, trigger_time);
			if (status == std::cv_status::timeout) {
				// trigger time reached
				// check if timer was neither removed nor reset in the meantime
				// this should be sufficient to avoid the ABA problem in our case
				if (
					(timer == m_timers.get_last()) &&
					(timer->get_trigger_time() == trigger_time)
				) {
					// still good -> fire it!
					timer->reset();
					timer->notify();
				}
			}
		} else {
			// no timers -> wait for cancel signal 
			m_cv.wait(lock);
		}
	}
	// reset cancel signal
	m_cancel = false;
}

//------------------------------------------------------------------------------	
//
Timer* TimerThread::get_next_timer()
{
	/**
	 * Note that this algorithm is O(n), in favor of an efficient Timer::reset() 
	 * function in O(1).
	 * 
	 * This seems like a reasonable trade-off, since we will call reset() often
	 * when measuring idle time, and this thread does otherwise nothing else
	 * than sleeping anyways.
	 */ 

	// find timer with minimum trigger time
	Timer* timer = m_timers.get_first();
	if (timer) {
		Timer::time_point trigger_time = timer->get_trigger_time();
		for (Timer* t = timer; t != nullptr; t = (Timer*)t->get_next()) {
			Timer::time_point tt = t->get_trigger_time();
			if (trigger_time > tt) {
				trigger_time = tt;
				timer = t;
			}
		}
	}
	return timer;
}


//------------------------------------------------------------------------------
	} // end namespace utils
} // end namespace remo
//------------------------------------------------------------------------------
