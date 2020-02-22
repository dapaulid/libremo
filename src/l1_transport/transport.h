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
#include "channel.h"
//
// C++ 
#include <functional>
#include <string>
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace trans {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
class Transport
{
// types
public:
	//! callback type for accept events
	typedef std::function<void(Channel*)> accept_handler;

// ctor/dtor
public:
	Transport();
	virtual ~Transport();

// public member functions
public:
	//! create a new channel that connects to the given endpoint
	Channel* connect(const std::string& a_endpoint);

	//! register a callback function that is invoked when an incoming connection was established
	void on_accept(const accept_handler& a_handler);


// private member functions
private:

// private members
private:
	//! callback function that is invoked when an incoming connection was established
	accept_handler m_accept_handler;
};

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
