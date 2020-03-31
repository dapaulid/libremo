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

    void set_header_capacity(size_t a_capacity);
    size_t get_header_capacity() const;

    Buffer& get_header() { return m_header; }
    Buffer& get_payload() { return m_payload; }

    char* get_buffer() { return m_buffer; }
    size_t get_size() const { return m_header.get_size() + m_payload.get_size(); }
    size_t get_buffer_size() const { return sizeof(m_buffer); }

    std::string to_string(); //const;

protected:
    void recycle() override;

private:
	REMO_MSVC_WARN_SUPPRESS(4324) // structure was padded -> sure
    alignas(std::max_align_t) char m_buffer [1024];
    LBuffer m_header;
    RBuffer m_payload;
};


//! smart pointer to safely pass around packages and finally recylce them
typedef std::unique_ptr<Packet, Packet::Recycler> packet_ptr;

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, /*const*/ Packet& a_packet);


//------------------------------------------------------------------------------
    } // end namespace trans

// some aliases for library namespace
typedef trans::packet_ptr packet_ptr;

} // end namespace remo
//------------------------------------------------------------------------------
