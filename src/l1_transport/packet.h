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
// defines
//------------------------------------------------------------------------------
//
//! maximum allowed packet size (header and payload)
#ifndef REMO_MAX_PACKET_SIZE
#define REMO_MAX_PACKET_SIZE           1024
#endif 

//! maximum allowed packet header size
#ifndef REMO_MAX_PACKET_HEADER_SIZE
#define REMO_MAX_PACKET_HEADER_SIZE    128
#endif 

//! maximum allowed packet payload size
static_assert(REMO_MAX_PACKET_HEADER_SIZE < REMO_MAX_PACKET_SIZE,
    "max header size must be smaller than total packet size");
#define REMO_MAX_PACKET_PAYLOAD_SIZE   (REMO_MAX_PACKET_SIZE - REMO_MAX_PACKET_HEADER_SIZE) 

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
    const Buffer& get_header() const { return m_header; }
    
    Buffer& get_payload() { return m_payload; }
    const Buffer& get_payload() const { return m_payload; }

    uint8_t* get_data() { return m_header.get_data(); }
    size_t get_size() const { return m_header.get_size() + m_payload.get_size(); }        
    size_t get_buffer_size() const { return sizeof(m_buffer); }

    void drop_header(size_t a_size);

    std::string to_string() const;

protected:
    void recycle() override;

private:
	REMO_MSVC_WARN_SUPPRESS(4324) // structure was padded -> sure
    alignas(std::max_align_t) uint8_t m_buffer [REMO_MAX_PACKET_SIZE];
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
