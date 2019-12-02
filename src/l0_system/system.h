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

#include <string>


//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------	
//
#ifdef _WIN32
#define REMO_SYS_WIN
#endif


//------------------------------------------------------------------------------
namespace remo {
namespace sys {
//------------------------------------------------------------------------------	

//! returns the current time in format HH:MM:SS.MILLI
std::string get_timestamp();

//------------------------------------------------------------------------------
} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
