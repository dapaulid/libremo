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
#include "utils/logger.h"
#include "utils/contracts.h"
//
// C++ 
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
Buffer::Buffer(void* a_data, size_t a_capacity, size_t a_size)
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
void Buffer::init(void* a_data, size_t a_capacity, size_t a_size)
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
// subclass implementation
//------------------------------------------------------------------------------	
//
void* RBuffer::grow(size_t a_size)
{
	if (m_size + a_size > m_capacity) {
		// full
		return nullptr;
	}
	void* ptr = (uint8_t*)m_data + m_size;
	set_size(m_size + a_size);
	return ptr;
}


//------------------------------------------------------------------------------
// subclass implementation
//------------------------------------------------------------------------------	
//
void* LBuffer::grow(size_t a_size)
{
	if (m_size + a_size > m_capacity) {
		// full
		return nullptr;
	}
	(uint8_t*&)m_data -= a_size;
	set_size(m_size + a_size);
	return m_data;
}


//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
