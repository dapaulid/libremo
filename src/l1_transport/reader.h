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

#include "packet.h"
#include "buffer.h"
#include "../l0_system/types.h"

#include <iostream>
#include <stdio.h>

//------------------------------------------------------------------------------
namespace remo {
	namespace trans {
//------------------------------------------------------------------------------	

//------------------------------------------------------------------------------
// class Reader
//------------------------------------------------------------------------------
//
class Reader
{
public:
	Reader(Buffer& a_buffer);

	unsigned char peek() {
		return *(unsigned char*)m_buffer.access_read(m_offset, 1);
	}

	unsigned char read_byte() {
		return *(unsigned char*)m_buffer.access_read(m_offset++, 1);
	}

	bool has_more() const {
		return m_offset < m_buffer.get_size();
	}

	void skip_array(size_t a_arraylength, size_t a_item_size);

protected:
	Buffer& m_buffer;
	size_t m_offset;
};

//------------------------------------------------------------------------------
// class BinaryReader
//------------------------------------------------------------------------------
//
class BinaryReader: public Reader
{
public:
	BinaryReader(Buffer& a_buffer): Reader(a_buffer), 
		m_args() {}

	void read_call();

	template<typename... Args>
	TypedValue read_result(Args... args)
	{
		// expect 'call' packet
		if (read_byte() != PacketType::packet_result) {
			throw error(ErrorCode::ERR_BAD_PACKET, "not a 'result' packet");
		}

		// read result
		TypedValue result = read_typed_value();
		// read "out" parameters
		REMO_FOREACH_ARG(args, read_outparam);

		return result;
	}

	// read "out" parameter
	template<typename T>
	void read_outparam(T* arg)
	{
		TypeId expected_type = TypeInfo<T*>::id();

		// check if out parameter type matches
		uint8_t modifier = 0;
		TypeId actual_type = read_type(modifier);
		if (actual_type != expected_type) {
			throw error(ErrorCode::ERR_OUTPARAM_TYPE_MISMATCH, 
				"out parameter type mismatch: expected %s, got %s", 
				get_type_name(expected_type), get_type_name(actual_type));
		}

		copy_to_ptr(arg, 1); // TODO handle array length
	}

	// template used to filter out non-pointer types in "out" parameters
	template<typename T>
	void read_outparam(T)
	{
	}

	// template used to filter out const-pointer types in "out" parameters
	template<typename T>
	void read_outparam(const T*)
	{
	}

	// read type id
	TypeId read_type(uint8_t& o_modifier)
	{
		// TODO use modifier constant
		unsigned char h = read_byte();
		o_modifier = (h >> 4) & 0xF;
		if (o_modifier < 0xA) {
			// wire size
			h &= 0xF;
		} else {
			// pointer type
			o_modifier = (uint8_t) get_type_size(static_cast<TypeId>(h & 0xF));
		}
		return static_cast<TypeId>(h);
	}

	// read typed value
	TypedValue read_typed_value();

	// read scalar value
	template<typename T>
	T read_value(size_t a_wire_size)
	{
		T value{};
		unsigned char* p = reinterpret_cast<unsigned char*>(&value);
		
		// read actual bytes
		LITTLE_ENDIAN_FOR(i, a_wire_size) {
			p[i] = read_byte();
		}

		return value;
	}

	// read pointer type
	template<typename T>
	T* read_ptr(size_t a_arraylength)
	{
		a_arraylength = 1; // TODO modify caller for correct value
		T* ptr = (T*)m_buffer.access_write(m_offset, sizeof(T));
		// TODO convert endianness
		skip_array(a_arraylength, sizeof(T));
		return ptr;
	}

	// read pointer type
	template<typename T>
	void copy_to_ptr(T a_dest, size_t a_arraylength)
	{
		unsigned char* p = reinterpret_cast<unsigned char*>(a_dest);
		// read actual bytes
		for (size_t i = 0; i < a_arraylength; i++) {
			LITTLE_ENDIAN_FOR(j, sizeof(*a_dest)) {
				p[j] = read_byte();
			} // end for
		} // end for
	}	

	// read string
	const char* read_cstr()
	{
		const char* str = (const char*)m_buffer.access_write(m_offset, 1);
		// forward until NUL byte
		while (read_byte()) {};
		return str;
	}

	std::string to_string();
	std::string format_value();
	std::string format_call();
	std::string format_result();

	const std::string& get_function() { return m_function; }
	const ArgList& get_args() const { return m_args; }

private:
	std::string m_function;
	ArgList m_args;
};

//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
