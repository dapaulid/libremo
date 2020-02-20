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
		startup,
		running,
		shutdown
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

// protected member functions
protected:
	//! implemented by subclasses to do the actual work
	virtual void action() = 0;
	//! implemented by subclasses to do anything necessary for startup
	virtual void do_startup() {};
	//! implemented by subclasses to do anything necessary for shutdown
	virtual void do_shutdown() {};

// private member functions
private:
	//! worker thread entry function
	void run();

// private members
private:
	//! the worker thread
	std::thread m_thread;
	//! state of worker thread
	ThreadState m_thread_state;
};

//------------------------------------------------------------------------------
	} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
