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

/*
	Based on https://stackoverflow.com/questions/26575303/create-function-call-dynamically-in-c
*/

#include "types.h"

#include <iostream>
#include <any>
#include <functional>

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


static_assert(__cplusplus >= 201703L, "The following code requires -std=c++17 to compile");

template <typename T>
auto fetch_back(T& t) -> typename std::remove_reference<decltype(t.back())>::type
{
    typename std::remove_reference<decltype(t.back())>::type ret = t.back();
    t.pop_back();
    return ret;
}

template <typename X>
struct any_ref_cast
{
    X do_cast(std::any y)
    {
        return std::any_cast<X>(y);
    }
};

template <typename X>
struct any_ref_cast<X&>
{
    X& do_cast(std::any y)
    {
        std::reference_wrapper<X> ref = std::any_cast<std::reference_wrapper<X>>(y);
        return ref.get();
    }
};

template <typename X>
struct any_ref_cast<const X&>
{
    const X& do_cast(std::any y)
    {
        std::reference_wrapper<const X> ref = std::any_cast<std::reference_wrapper<const X>>(y);
        return ref.get();
    }
};

// free function
template <typename Ret, typename...Arg>
Ret dynamic_call (Ret (*func)(Arg...), ArgList args)
{
    if (sizeof...(Arg) != args.size())
        throw "Argument number mismatch!";

    return func(any_ref_cast<Arg>().do_cast(fetch_back(args))...);
}

// member function
template <typename Class, typename Ret, typename...Arg>
Ret dynamic_call (Ret (Class::*func)(Arg...), Class* obj, ArgList args)
{
    if (sizeof...(Arg) != args.size())
        throw "Argument number mismatch!";

    return (obj->*func)(any_ref_cast<Arg>().do_cast(fetch_back(args))...);
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
