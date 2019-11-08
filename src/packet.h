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

#include "error.h"

#include <stddef.h>

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


class Packet {
public:
    Packet();
    virtual ~Packet();

    void append(char a_byte);

    char get_byte(size_t a_offset) const;

    template<typename T>
    const T* get_ptr(size_t a_offset) const {
        // TODO check alignment
        if (a_offset + sizeof(T) <= m_size) {
            return reinterpret_cast<const T*>(&m_buffer[a_offset]);
        } else {
            throw error(ErrorCode::ERR_BAD_PACKET_ACCESS, "Bad packet access: size=%zu, offset=%zu, ptrsize=%zu", 
                m_size, a_offset, sizeof(T));
        }
    }

    char* get_buffer() { return m_buffer; }
    size_t get_size() const { return m_size;  }

    void log() const;

    std::string to_string() const;

private:
    char m_buffer [1024];
    size_t m_size;
};

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
