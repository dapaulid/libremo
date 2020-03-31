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
#include <stdint.h>
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace sys {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------
//
inline uint16_t get_le(const uint16_t* a_ptr);
inline uint32_t get_le(const uint32_t* a_ptr);
inline uint64_t get_le(const uint64_t* a_ptr);

inline uint16_t get_le_ua(const uint16_t* a_ptr);
inline uint32_t get_le_ua(const uint32_t* a_ptr);
inline uint64_t get_le_ua(const uint64_t* a_ptr);

//------------------------------------------------------------------------------
	} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
