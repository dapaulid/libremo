//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "local_endpoint.h"

#include "function.h"


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// template implementation
//------------------------------------------------------------------------------
//
template <typename Ret, typename...Arg>
void LocalEndpoint::bind(const std::string& a_name, Ret (*a_func)(Arg...))
{
    register_item(new bound_function<Ret, Arg...>(a_name, a_func));
}

//------------------------------------------------------------------------------
//
template<typename Lambda>
void LocalEndpoint::bind(const std::string& a_name, Lambda& a_lambda)
{
    register_item(new lambda_function<Lambda>(a_name, a_lambda));
}


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
