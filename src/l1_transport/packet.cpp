//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "packet.h"
#include "reader.h"
#include "utils/contracts.h"

#include <iostream>
#include <iomanip>
#include <sstream>

//------------------------------------------------------------------------------
namespace remo {
    namespace trans {
//------------------------------------------------------------------------------	


Packet::Packet():
    m_buffer(),
    m_header(),
    m_payload()
{
    set_header_capacity(0);
}

//------------------------------------------------------------------------------

Packet::~Packet()
{
}

//------------------------------------------------------------------------------

void Packet::recycle()
{
    // reset state
    set_header_capacity(0);
    // call base
    Recyclable::recycle();
}

//------------------------------------------------------------------------------

void Packet::set_header_capacity(size_t a_capacity)
{
    REMO_PRECOND({
        REMO_ASSERT(a_capacity <= get_buffer_size(),
            "header capacity must not exceed buffer size");
    });
    uint8_t* border = &m_buffer[a_capacity];
    m_header.init(border, a_capacity);
    m_payload.init(border, get_buffer_size() - a_capacity);
}

//------------------------------------------------------------------------------

size_t Packet::get_header_capacity() const
{
    return m_header.get_capacity();
}

//------------------------------------------------------------------------------

std::string Packet::to_string() const
{
    // TODO refactor to other class that can take const reference
    return BinaryReader(get_payload()).to_string();
}


//------------------------------------------------------------------------------
    } // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
