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
#include "utils/logger.h"
#include "utils/contracts.h"

#include <iostream>
#include <iomanip>
#include <sstream>

//------------------------------------------------------------------------------
namespace remo {
    namespace trans {
//------------------------------------------------------------------------------	

//! logger instance
static Logger logger("Packet");


//------------------------------------------------------------------------------	
// class Packet
//------------------------------------------------------------------------------	
//
Packet::Packet():
    m_buffer(),
    m_header(),
    m_payload()
{
    set_header_capacity(REMO_MAX_PACKET_HEADER_SIZE);
}

//------------------------------------------------------------------------------

Packet::~Packet()
{
}

//------------------------------------------------------------------------------

void Packet::recycle()
{
    // reset state
    set_header_capacity(REMO_MAX_PACKET_HEADER_SIZE);
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

void Packet::drop_header(size_t a_size)
{
    REMO_PRECOND({
        REMO_ASSERT(a_size <= m_payload.get_size(),
            "header size to drop must not exceed payload size");
    });

    m_payload.init(m_payload.get_data() + a_size, m_payload.get_capacity(),
        m_payload.get_size() - a_size);
}

//------------------------------------------------------------------------------

std::string Packet::to_string() const
{
    return BinaryReader(get_payload()).to_string();
}


//------------------------------------------------------------------------------
    } // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
