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
#include "utils/contracts.h"
//
// C++ 
#include <thread>
#include <atomic>
//
// system
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace sys {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
class Worker
{
// types
public:
	enum class ThreadState {
		idle,
		starting,
		running,
		terminated,
		joined,
	};

// ctor/dtor
public:
	Worker();
	virtual ~Worker();

// public member functions
public:
	//! starts the worker thread
	void startup();
	//! requests and waits for termination of the worker thread
	void shutdown();
	//! returns the current thread state
	ThreadState get_thread_state() const { return m_thread_state; }

	//! returns the worker instance associated with the current thread, if any
	//! WARNING: Beware of dangling pointers!
	static Worker* actual();

	//! returns the given state name
	static const char* get_state_str(ThreadState a_state);

// protected member functions
protected:
	//! implemented by subclasses to do the actual work
	virtual void action() = 0;
	//! implemented by subclasses to do anything necessary for startup
	virtual void do_startup() {};
	//! implemented by subclasses to do anything necessary for shutdown
	virtual void do_shutdown() {};

	//! requests the thread to terminate
	virtual void terminate();
	//! gets the termination requested flag
	bool termination_requested() const { return m_termination_requested; }

// private member functions
private:
	//! worker thread entry function
	void run();

	//! add/remove current thread to/from our bookkeeping
	void register_thread();
	void unregister_thread();

	//! enters the given thread state
	void enter_thread_state(ThreadState a_new_state);

// private members
private:
	//! the worker thread
	std::thread m_thread;
	//! state of worker thread
	std::atomic<ThreadState> m_thread_state;
	//! flag indicating if termination was requested
	std::atomic<bool>m_termination_requested;
};

//------------------------------------------------------------------------------
	} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
