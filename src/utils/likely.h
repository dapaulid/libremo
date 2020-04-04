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
// macros
//------------------------------------------------------------------------------
//
/**
 * branch prediction macros
 * 
 * intended to be used in other macros like assertions or exceptions,
 * and not to be cluttered everywhere in the code.
 */
#if defined(__GNUC__) && (__GNUC__ >= 3)
	// available
	#define REMO_LIKELY(x)             (__builtin_expect(!!(x), 1))
	#define REMO_UNLIKELY(x)           (__builtin_expect(!!(x), 0))
#else
	// not available
	#define REMO_LIKELY(x)             (x)
	#define REMO_UNLIKELY(x)           (x)
#endif


//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
