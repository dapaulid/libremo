//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
#include "buffer.h"
#include "l0_system/error.h"
#include "utils/logger.h"
#include "utils/contracts.h"
//
// C++
#include <sstream>
#include <iomanip>
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace trans {
//------------------------------------------------------------------------------

//! logger instance
static Logger logger("Buffer");


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------	
//
Buffer::Buffer(uint8_t* a_data, size_t a_capacity, size_t a_size)
{
	init(a_data, a_capacity, a_size);
}

//------------------------------------------------------------------------------	
//
Buffer::~Buffer()
{
}

//------------------------------------------------------------------------------	
//
void Buffer::init(uint8_t* a_data, size_t a_capacity, size_t a_size)
{
	m_data = a_data;
	m_capacity = a_capacity;
	set_size(a_size);
}

//------------------------------------------------------------------------------	
//
void Buffer::set_size(size_t a_size)
{
	REMO_PRECOND({
		REMO_ASSERT(a_size <= m_capacity, 
			"buffer size must not exceed capacity");
	});
	m_size = a_size;
}

//------------------------------------------------------------------------------	
//
const void* Buffer::access_read(size_t a_offset, size_t a_size) const
{
	// check range
	if (a_offset + a_size > m_size) {
		throw error(ErrorCode::ERR_BAD_PACKET_ACCESS, "Bad packet buffer read access: offset=%zu, size=%zu, bufsize=%zu", 
			a_offset, a_size, m_size);
	}
	// calculate pointer into buffer
	return m_data + a_offset;
}

//------------------------------------------------------------------------------	
//
void* Buffer::access_write(size_t a_offset, size_t a_size)
{
	// check range
	if (a_offset + a_size > m_size) {
		throw error(ErrorCode::ERR_BAD_PACKET_ACCESS, "Bad packet buffer write access: offset=%zu, size=%zu, bufsize=%zu", 
			a_offset, a_size, m_size);
	}
	// calculate pointer into buffer
	return m_data + a_offset;
}

//------------------------------------------------------------------------------	
//
std::string Buffer::to_hex() const
{
    std::stringstream ss;
    for (size_t i = 0; i < m_size; i++) {
        if (i > 0) {
            ss << ' ';
        }
        ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << m_data[i];
    }
    return ss.str();
}

//------------------------------------------------------------------------------	
//
void* RBuffer::grow(size_t a_size)
{
	if (m_size + a_size > m_capacity) {
		// full
		throw error(ErrorCode::ERR_PACKET_FULL, "Packet payload is full (%zu bytes)", m_capacity);
	}
	size_t offset = m_size;
	set_size(m_size + a_size);
	return access_write(offset, a_size);
}

//------------------------------------------------------------------------------	
//
void* LBuffer::grow(size_t a_size)
{
	if (m_size + a_size > m_capacity) {
		// full
		throw error(ErrorCode::ERR_PACKET_FULL, "Packet header is full (%zu bytes)", m_capacity);
	}
	set_size(m_size + a_size);
	m_data -= a_size;
	return access_write(0, a_size);
}

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
