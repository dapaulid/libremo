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
	Buffer(void* a_data = nullptr, size_t a_capacity = 0, size_t a_size = 0);
	virtual ~Buffer();

// public member functions
public:
	void init(void* a_data, size_t a_capacity, size_t a_size = 0);

	virtual void* grow(size_t a_size) = 0;
	
	void* get_data() const { return m_data; }
	size_t get_size() const { return m_size; }
	size_t get_capacity() const { return m_capacity; }

	void set_size(size_t a_size);


// protected members
protected:
	void* m_data;
	size_t m_size;
	size_t m_capacity;
};


//------------------------------------------------------------------------------
// subclass definition
//------------------------------------------------------------------------------
//
//! right-growing buffer
class RBuffer: public Buffer {
// public member functions
public:
	virtual void* grow(size_t a_size) override;
};


//------------------------------------------------------------------------------
// subclass definition
//------------------------------------------------------------------------------
//
//! left-growing buffer
class LBuffer: public Buffer {
// public member functions
public:
	virtual void* grow(size_t a_size) override;
};


//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
