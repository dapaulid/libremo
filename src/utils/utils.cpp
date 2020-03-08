//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "async.h"

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
//
// C++
#include <algorithm>
#include <cstring>
//
// system
//
//------------------------------------------------------------------------------
namespace remo {
	namespace utils {
//------------------------------------------------------------------------------	

const char* basename(const char* a_filename)
{
	const char* p = a_filename + std::strlen(a_filename);
	while ((p >= a_filename) && (*p != '/') && (*p != '\\')) {
		--p;
	}
	return p+1;
}

//------------------------------------------------------------------------------
	} // end namespace utils
} // end namespace remo
//------------------------------------------------------------------------------
