//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------

#include <assert.h>
#include <functional>
#include <exception>

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------
//

// assert wrapper
#define REMO_ASSERT(cond, msg) assert(msg && (cond))

// precondition block: list assertions to be evaluated at function scope entry
#define REMO_PRECOND(block) block

// postcondition block: list assertions to be evaluated at function scope exit
// NOTE: postconditions are (intentionally) NOT checked on exceptions
#define REMO_POSTCOND(block) PostCondGuard _postcondguard([&]()block);


//------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------
//
struct PostCondGuard
{
	typedef std::function<void(void)> callback; 
	PostCondGuard(const callback& a_postcond): 
		m_postcond(a_postcond) {
	}
	~PostCondGuard() {
		if (!std::uncaught_exception()) {
			m_postcond();
		}
	}
	callback m_postcond;
};


//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------