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

#include "../l0_system/types.h"


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


//------------------------------------------------------------------------------
// struct definition
//------------------------------------------------------------------------------	
//
struct ArgListIterator
{
    ArgListIterator(const ArgList& a_arglist):
        m_arglist(a_arglist), m_index(a_arglist.size()) {}
    
    const TypedValue& next() {
        return m_arglist[--m_index];
    }
private:
    const ArgList& m_arglist;
    size_t m_index;
};

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------	
//

//------------------------------------------------------------------------------	
//
/*
    call function with dynamic arguments.

    IMPORTANT: caller is responsible to check argument types, otherwise 
    this function will throw cryptic exceptions or even core dump.

    Inspired by https://stackoverflow.com/questions/26575303/create-function-call-dynamically-in-c
*/
template <typename Func, typename Ret, typename...Arg>
Ret dynamic_call (Func& a_func, const ArgList& args)
{
    // do the magic...
    ArgListIterator it(args);
    return a_func(it.next().get<Arg>()...);
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
