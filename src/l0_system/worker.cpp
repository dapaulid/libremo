//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "worker.h"

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
#include "utils/logger.h"
//
// C++ 
#include <functional>
#include <mutex>
#include <unordered_map>
//
// system
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace sys {
//------------------------------------------------------------------------------	

//! logger instance
static Logger logger("Worker");

//! set of all workers
typedef std::unordered_map<std::thread::id, Worker*> WorkerSet; 
static WorkerSet s_workers;
static std::mutex s_workers_lock;


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------	
//
Worker::Worker():
	m_thread(),
	m_thread_state(ThreadState::idle),
	m_termination_requested(false)
{
}

//------------------------------------------------------------------------------	
//
Worker::~Worker()
{
	REMO_ASSERT(m_thread_state == ThreadState::idle || m_thread_state == ThreadState::joined,
		"worker thread destructor called while not in idle/joined state");
}

//------------------------------------------------------------------------------	
//
void Worker::startup()
{
	REMO_PRECOND({
		REMO_ASSERT(m_thread_state == ThreadState::idle,
			"worker thread startup only allowed in idle state");
	});

	// we're starting up
	enter_thread_state(ThreadState::starting);
	// start the thread
	m_thread = std::thread(std::bind(&Worker::run, this));
}

//------------------------------------------------------------------------------	
//
void Worker::shutdown()
{
	// request termination
	terminate();
	// wait for thread to terminate
	m_thread.join();
	// we're joined now
	enter_thread_state(ThreadState::joined);
}

//------------------------------------------------------------------------------	
//
void Worker::run()
{
	enter_thread_state(ThreadState::running);
	
	register_thread();
	
	try {	
		// let subclasses to their stuff
		do_startup();
	} catch (const std::exception& e) {
		REMO_ERROR("unhandled exception in worker thread during startup: %s", 
			e.what());
	} catch (...) {
		REMO_ERROR("unhandled unknown exception in worker thread during startup");
	}
	
	while (!m_termination_requested) {
		REMO_WARN("Still running");
		try {
			// let subclasses to their stuff
			action();
		} catch (const std::exception& e) {
			REMO_ERROR("unhandled exception in worker thread: %s", 
				e.what());
		} catch (...) {
			REMO_ERROR("unhandled unknown exception in worker thread");
		}
	}
	
	try {	
		// let subclasses to their stuff
		do_shutdown();
	} catch (const std::exception& e) {
		REMO_ERROR("unhandled exception in worker thread during shutdown: %s", 
			e.what());
	} catch (...) {
		REMO_ERROR("unhandled unknown exception in worker thread during shutdown");
	}

	unregister_thread();

	enter_thread_state(ThreadState::terminated);
}

//------------------------------------------------------------------------------	
//
void Worker::terminate()
{
	REMO_INFO("requesting thread to terminate");
	m_termination_requested = true;
}

//------------------------------------------------------------------------------	
//
void Worker::enter_thread_state(ThreadState a_new_state)
{
	m_thread_state = a_new_state;
	REMO_INFO("thread entered state '%s'", get_state_str(m_thread_state));
}

//------------------------------------------------------------------------------	
//
void Worker::join()
{
	m_thread.join();
}

//------------------------------------------------------------------------------	
//
bool Worker::is_self() const
{
	return std::this_thread::get_id() == m_thread.get_id();
}

//------------------------------------------------------------------------------	
//
void Worker::register_thread()
{
	std::lock_guard<std::mutex> lock(s_workers_lock);
	s_workers.insert(WorkerSet::value_type(std::this_thread::get_id(), this));
}

//------------------------------------------------------------------------------	
//
void Worker::unregister_thread()
{
	std::lock_guard<std::mutex> lock(s_workers_lock);
	s_workers.erase(std::this_thread::get_id());
}

//------------------------------------------------------------------------------	
//
Worker* Worker::actual()
{
	std::lock_guard<std::mutex> lock(s_workers_lock);
	auto it = s_workers.find(std::this_thread::get_id());
	return it != s_workers.end() ? it->second : nullptr;
}

//------------------------------------------------------------------------------	
//
const char* Worker::get_state_str(ThreadState a_state)
{
	switch (a_state) {
		case ThreadState::idle: return "idle";
		case ThreadState::starting: return "starting";
		case ThreadState::running: return "running";
		case ThreadState::terminated: return "terminated";
		case ThreadState::joined: return "joined";
	}
	// should not be reached
	return "(unknown)";
}


//------------------------------------------------------------------------------
	} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
