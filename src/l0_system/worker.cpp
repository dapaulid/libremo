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
	m_thread_state(ThreadState::idle)
{
}

//------------------------------------------------------------------------------	
//
Worker::~Worker()
{
	REMO_ASSERT(m_thread_state == ThreadState::idle,
		"worker thread destructor called while not in idle state");
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
	m_thread_state = ThreadState::startup;
	// let subclasses to their stuff
	do_startup();
	// start the thread
	m_thread = std::thread(std::bind(&Worker::run, this));
}

//------------------------------------------------------------------------------	
//
void Worker::shutdown()
{
	REMO_PRECOND({
		REMO_ASSERT(m_thread_state == ThreadState::running 
			|| m_thread_state == ThreadState::startup,
			"worker thread shutdown only allowed in running or startup state");
	});

	// we're shutting down
	m_thread_state = ThreadState::shutdown;
	// let subclasses do their stuff
	do_shutdown();
	// wait for thread to terminate
	m_thread.join();
	// we're idle now
	m_thread_state = ThreadState::idle;
}

//------------------------------------------------------------------------------	
//
void Worker::run()
{
	register_thread();
	m_thread_state = ThreadState::running;
	while (m_thread_state == ThreadState::running) {
		try {
			action();
		} catch (const std::exception& e) {
			REMO_ERROR("unhandled exception in worker thread: %s", 
				e.what());
		} catch (...) {
			REMO_ERROR("unhandled unknown exception in worker thread");
		}
	}
	unregister_thread();
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
	} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
