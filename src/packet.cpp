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
#include "error.h"
#include "reader.h"

#include <iostream>
#include <iomanip>
#include <sstream>

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


Packet::Packet():
    m_size(0)
{
}

//------------------------------------------------------------------------------

Packet::~Packet()
{
}

//------------------------------------------------------------------------------

void Packet::append(char a_byte)
{
    if (m_size < sizeof(m_buffer)) {
        m_buffer[m_size++] = a_byte;
    } else {
        throw error(ErrorCode::ERR_PACKET_FULL, "Packet is full (%zu bytes)", m_size);
    }
}

//------------------------------------------------------------------------------

char Packet::get_byte(size_t a_offset) const
{
    if (a_offset < m_size) {
        return m_buffer[a_offset];
    } else {
        throw error(ErrorCode::ERR_BAD_PACKET_ACCESS, "Bad packet access: size=%zu, offset=%zu", 
            m_size, a_offset);
    }
}

//------------------------------------------------------------------------------

void Packet::log() const
{
    std::cerr << "packet of size " << m_size << ": [ ";
    for (size_t i = 0; i < m_size; i++) {
        std::cerr << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (m_buffer[i] & 0xFF) << " ";
    }
    std::cerr << "]" << std::endl;
    std::cerr << "=> " << to_string() << std::endl;
}

//------------------------------------------------------------------------------

std::string Packet::to_string() const
{
    return BinaryReader(*this).to_string();
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
