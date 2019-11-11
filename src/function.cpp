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

#include <sstream>

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class function
//------------------------------------------------------------------------------
//
function::function(const std::string& a_name, TypeId a_result_type, const TypeList& a_param_types):
    m_name(a_name), m_result_type(a_result_type), m_param_types(a_param_types)
{
    // check function name
    if (!is_valid_name(m_name)) {
        throw error(ErrorCode::ERR_INVALID_FUNC_NAME, 
            "invalid function name: \"%s\"", m_name.c_str());
    }
}

//------------------------------------------------------------------------------

function::~function()
{
}

//------------------------------------------------------------------------------

std::string function::to_string() const
{
    std::stringstream ss;
    ss << get_type_name(m_result_type) << ' ' << m_name << '(';
    for (size_t i = 0; i < m_param_types.size(); i++) {
        if (i > 0) {
            ss << ", ";
        }
        ss << get_type_name(m_param_types[i]);
    }
    ss << ')';

    return ss.str();
}

//------------------------------------------------------------------------------

void function::check_args(const ArgList& args)
{
    // check number of arguments
    if (m_param_types.size() != args.size()) {
        throw error(ErrorCode::ERR_PARAM_NUM_MISMATCH, "Parameter number mismatch: expected %zu, got %zu",
            m_param_types.size(), args.size());
    }

    // check if argument and parameter types match
    // TODO allow cast here? For now we do an exact match
    for (size_t i = 0; i < m_param_types.size(); i++) {
        if (args[i].type != m_param_types[i]) {
            throw error(ErrorCode::ERR_PARAM_TYPE_MISMATCH, 
                "cannot convert '%s' to '%s' for argument %zu to remote function '%s'",
                get_type_name(args[i].type), get_type_name(m_param_types[i]),
                i+1, to_string().c_str());
        }
    }
}

//------------------------------------------------------------------------------

bool function::is_valid_name(const std::string& a_name)
{
    // must not be empty
    if (a_name.empty()) {
        return false;
    }

    // must start with a letter
    if (!(
        (a_name[0] >= 'a' && a_name[0] <= 'z') ||
        (a_name[0] >= 'A' && a_name[0] <= 'Z')
    )) {
        return false;
    }

    // remainer must be alphanumeric or '_'
    for (size_t i = 1; i < a_name.size(); i++) {
        if (!(
            (a_name[i] >= 'a' && a_name[i] <= 'z') ||
            (a_name[i] >= 'A' && a_name[i] <= 'Z') ||
            (a_name[i] >= '0' && a_name[i] <= '9') ||
            (a_name[i] == '_')
        )) {
            return false;
        }        
    }

    // valid
    return true;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
