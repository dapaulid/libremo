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

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------
//

// assert wrapper
#define REMO_ASSERT(cond, msg) assert(((void) msg, cond))

// precondition block: list assertions to be evaluated at function scope entry
#define REMO_PRECOND(block) struct PreCond { PreCond() block } _precond;

// postcondition block: list assertions to be evaluated at function scope exit
#define REMO_POSTCOND(block) struct PostCond { ~PostCond() block } _postcond;


//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
