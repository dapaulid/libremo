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

#include <future>


//------------------------------------------------------------------------------
namespace remo {
	namespace utils {
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------
//
//! runs a function asynchronously, without (implicitly) waiting for it (fire and forget)
void async(std::function<void()> a_func);

//------------------------------------------------------------------------------
	} // end namespace utils
} // end namespace remo
//------------------------------------------------------------------------------
