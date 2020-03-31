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

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
//
// C++ 
#include <stddef.h>
#include <string>
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace trans {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------
//
class Buffer {
// ctor/dtor
public:
	Buffer(uint8_t* a_data = nullptr, size_t a_capacity = 0, size_t a_size = 0);
	virtual ~Buffer();

// public member functions
public:
	void init(uint8_t* a_data, size_t a_capacity, size_t a_size = 0);
	virtual void* grow(size_t a_size) = 0;	

	const void* access_read(size_t a_offset, size_t a_size) const;
	void* access_write(size_t a_offset, size_t a_size);

	uint8_t* get_data() const { return m_data; }
	size_t get_size() const { return m_size; }
	size_t get_capacity() const { return m_capacity; }

	void set_size(size_t a_size);

	std::string to_hex() const;	

// protected members
protected:
	uint8_t* m_data;
	size_t m_size;
	size_t m_capacity;
};


//------------------------------------------------------------------------------
// subclass definition
//------------------------------------------------------------------------------
//
//! right-growing buffer
class RBuffer: public Buffer {
// protected member functions
public:
	virtual void* grow(size_t a_size) override;
};


//------------------------------------------------------------------------------
// subclass definition
//------------------------------------------------------------------------------
//
//! left-growing buffer
class LBuffer: public Buffer {
// protected member functions
public:
	virtual void* grow(size_t a_size) override;
};


//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
