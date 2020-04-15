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
//
// C++
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace utils {
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
template<typename T>
class ActiveQueue
{
public:

	void put(const T& item) {
		// lock the mutex
		m_mutex.lock();
		// add item to queue
		m_queue.push(item);
		// unlock the mutex
		m_mutex.unlock();
		// mutex must be released before notifying, to avoid waking up
		// the waiting thread only to block again		
		m_cv.notify_one();
	}

	T get() {
		// lock the mutex
		std::unique_lock<std::mutex> lock(m_mutex);
		// mutex is released during actual wait (suspend) and reacquired after wait(lock) exits. 
		// Therefore, we can safely inspect the queue state to protect against
		// spurious awakenings.
		while (m_queue.empty()) {
			m_cv.wait(lock);
		}
		// when we get here, mutex is still locked and queue is not empty
		// get item from queue and remove it
		T item = m_queue.front();
		m_queue.pop();
		return item;
		// mutex will be released by unique_lock upon returning
	}

protected:
	std::queue<T> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_cv;
};


//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
class ActiveObject: public sys::Worker
{
// public types
public:
	typedef std::function<void()> deferred_call;

// ctor/dtor
public:
	ActiveObject();
	virtual ~ActiveObject();

// public member functions
public:
	void enqueue(deferred_call a_call);

	//! requests termination of the worker thread
	virtual void shutdown() override;

// protected member functions
protected:
	//! process calls
	virtual void action() override;

// private members
private:
	ActiveQueue<deferred_call> m_queue;
};


//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
template<typename Object, typename Ret, typename...Args>
class ActiveFunction
{
// ctor/dtor
public:
	ActiveFunction(Object* a_obj, void (Object::*a_func)(Args...)):
		m_obj(a_obj), m_func(a_func) {}
	~ActiveFunction() {}

	void operator()(Args&&... args) {
		ActiveObject::deferred_call f = std::bind(m_func, m_obj, args...);
		m_obj->enqueue(f);
	}

public:
	Object* m_obj;
	void (Object::*m_func)(Args...);
};


//------------------------------------------------------------------------------
	} // end namespace utils
} // end namespace remo
//------------------------------------------------------------------------------
