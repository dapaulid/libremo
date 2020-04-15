//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "active.h"

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
#include "utils/contracts.h"
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
static Logger logger("Active");


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------
//
ActiveObject::ActiveObject():
	Worker(),
	m_queue()
{
	startup();
}

//------------------------------------------------------------------------------
//
ActiveObject::~ActiveObject()
{
	shutdown();
	join();
}

//------------------------------------------------------------------------------	
//
void ActiveObject::shutdown()
{
	REMO_INFO("requesting thread to terminate");
	m_queue.put(deferred_call());
}

//------------------------------------------------------------------------------
//
void ActiveObject::enqueue(deferred_call a_call)
{
	m_queue.put(a_call);
}

//------------------------------------------------------------------------------
//
void ActiveObject::action()
{
	deferred_call call = m_queue.get();
	if (call) {
		call();
	} else {
		terminate();
	}
}

//------------------------------------------------------------------------------
	} // end namespace utils
} // end namespace remo
//------------------------------------------------------------------------------
