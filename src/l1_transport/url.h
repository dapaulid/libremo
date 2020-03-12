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
//
// C++ 
#include <string>
#include <deque>
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
struct Url
{
// ctor/dtor
public:
	Url(const std::string& a_str = std::string());
	~Url();

// public member functions
public:
	void from_string(const std::string& a_str);
	std::string to_string();

// public members
public:
	std::deque<std::string> proto;
	std::string host;
	std::string path;
	std::string query;
	std::string fragment;
};

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
