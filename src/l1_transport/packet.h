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

#include "buffer.h"
#include "l0_system/system.h" // REMO_MSVC_WARN_SUPPRESS
#include "l0_system/error.h"
#include "utils/recycling.h"

#include <iostream>
#include <memory>
#include <cstddef> // max_align_t

//------------------------------------------------------------------------------
namespace remo {
    namespace trans {
//------------------------------------------------------------------------------	

enum PacketType: uint8_t
{
    packet_ack     = 0x3A, // ':'
    packet_call    = 0x3E, // '>'
    packet_result  = 0x3C, // '<'
    packet_query   = 0x3F, // '?'
    packet_info    = 0x21, // '!'
};

class Packet: public Recyclable<Packet>
{
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

    template<typename T>
    T* get_ptr(size_t a_offset) {
        // TODO check alignment
        if (a_offset + sizeof(T) <= m_size) {
            return reinterpret_cast<T*>(&m_buffer[a_offset]);
        } else {
            throw error(ErrorCode::ERR_BAD_PACKET_ACCESS, "Bad packet access: size=%zu, offset=%zu, ptrsize=%zu", 
                m_size, a_offset, sizeof(T));
        }
    }

    void set_size(size_t a_size);

    char* get_buffer() { return m_buffer; }
    size_t get_size() const { return m_size; }
    size_t get_buffer_size() const { return sizeof(m_buffer); }

    std::string to_string(); //const;
    std::string to_hex() const;

protected:
    void recycle() override;

private:
	REMO_MSVC_WARN_SUPPRESS(4324) // structure was padded -> sure
    alignas(std::max_align_t) char m_buffer [1024];
    size_t m_size;
    trans::LBuffer m_header;
    trans::RBuffer m_payload;
};


//! smart pointer to safely pass around packages and finally recylce them
typedef std::unique_ptr<Packet, Packet::Recycler> packet_ptr;

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, /*const*/ Packet& a_packet);


//------------------------------------------------------------------------------
    } // end namespace trans

typedef trans::packet_ptr packet_ptr;

} // end namespace remo
//------------------------------------------------------------------------------
