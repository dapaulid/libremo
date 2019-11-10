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

#include "types.h"
#include "packet.h"

#include <iostream>
#include <stdio.h>

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

//------------------------------------------------------------------------------
// class Reader
//------------------------------------------------------------------------------
//
class Reader
{
public:
	Reader(Packet& a_packet);

	unsigned char read();
	void skip_array(size_t a_arraylength, size_t a_item_size);

	bool has_more() const;

protected:
	Packet& m_packet;
	size_t m_offset;
};

//------------------------------------------------------------------------------
// class BinaryReader
//------------------------------------------------------------------------------
//
class BinaryReader: public Reader
{
public:
	BinaryReader(Packet& a_packet): Reader(a_packet), 
		m_args() {}

	void read_call();

	// read type id
	TypeId read_type(uint8_t& o_modifier)
	{
		unsigned char h = read();
		o_modifier = (h >> 4) & 0xF;
		if (o_modifier < 0xA) {
			// wire size
			h &= 0xF;
		} else {
			// pointer type
			// TODO fake size
			o_modifier = 4;
		}
		return static_cast<TypeId>(h);
	}

	// read scalar value
	template<typename T>
	T read_value(size_t a_wire_size)
	{
		T value{};
		unsigned char* p = reinterpret_cast<unsigned char*>(&value);
		
		// read actual bytes
		LITTLE_ENDIAN_FOR(i, a_wire_size) {
			p[i] = read();
		}

		return value;
	}

	// read pointer type
	template<typename T>
	T* read_ptr(size_t a_arraylength)
	{
		a_arraylength = 1; // TODO modify caller for correct value
		T* ptr = m_packet.get_ptr<T>(m_offset);
		// TODO convert endianness
		skip_array(a_arraylength, sizeof(T));
		return ptr;
	}

	// read string
	const char* read_cstr()
	{
		const char* str = m_packet.get_ptr<const char>(m_offset);
		// forward until NUL byte
		while (read()) {};
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
} // end namespace remo
//------------------------------------------------------------------------------
