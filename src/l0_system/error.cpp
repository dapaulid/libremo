//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "error.h"

#include <stdarg.h>

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


error::error(ErrorCode a_code, const char* a_message, ...): 
	std::runtime_error(""), 
	m_code(a_code)
{
	va_list args;
	va_start(args, a_message);
	vsnprintf(m_message,sizeof(m_message), a_message, args);
	va_end (args);
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
