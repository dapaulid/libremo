//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "function.h"

#include "error.h"

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class function
//------------------------------------------------------------------------------
//
function::function(const std::string& a_name, const TypeList& a_param_types):
    m_name(a_name), m_param_types(a_param_types)
{
}

//------------------------------------------------------------------------------

function::~function()
{
}

//------------------------------------------------------------------------------

void function::check_args(const ArgList& args)
{
    if (m_param_types.size() != args.size()) {
        throw error(ErrorCode::ERR_PARAM_NUM_MISMATCH, "Parameter number mismatch: expected %zu, got %zu",
            m_param_types.size(), args.size());
    }
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
