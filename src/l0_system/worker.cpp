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
//
// C++ 
#include <functional>
//
// system
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace sys {
//------------------------------------------------------------------------------	


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
		"Worker thread destructor called while not in idle state");
}

//------------------------------------------------------------------------------	
//
void Worker::startup()
{
	REMO_PRECOND({
		REMO_ASSERT(m_thread_state == ThreadState::idle,
			"Worker thread startup only allowed in idle state");
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
			"Worker thread shutdown only allowed in running or startup state");
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
	m_thread_state = ThreadState::running;
	while (m_thread_state == ThreadState::running) {
		action();
	}
}


//------------------------------------------------------------------------------
	} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
