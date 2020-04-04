//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "item.h"

#include "local_endpoint.h"
#include "utils/logger.h"


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------

//! logger instance
static Logger logger("Item");


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------
//
Item::Item(const std::string& a_name):
    m_endpoint(nullptr), 
    m_name(a_name)
{
    // check item name
    REMO_THROW_IF(!is_valid_name(m_name),
        ErrorCode::ERR_INVALID_IDENTIFIER, 
        "invalid identifier: \"%s\"", m_name.c_str());
}

//------------------------------------------------------------------------------
//
Item::~Item()
{
    // unregister from endpoint
    if (m_endpoint) {
        m_endpoint->unregister_item(this);
    }
}

//------------------------------------------------------------------------------
//
std::string Item::get_full_name() const
{
    return m_name;
}

//------------------------------------------------------------------------------
//
std::string Item::to_string() const
{
    return m_name;
}

//------------------------------------------------------------------------------
//
bool Item::is_valid_name(const std::string& a_name)
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
