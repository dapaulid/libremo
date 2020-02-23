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
#include "l1_transport/transport.h"
//
// C++ 
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
class TcpTransport: public Transport
{
// ctor/dtor
public:
	TcpTransport();
	virtual ~TcpTransport();

// public member functions
public:
	//! create a new channel that connects to the given endpoint
	virtual Channel* connect(const std::string& a_endpoint) override;

// private member functions
private:

// private members
private:
};

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
