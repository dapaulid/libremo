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
#include "../l0_system/types.h"
#include "../l0_system/endianness.h"

#include <stdlib.h>
#include <iostream> // debugging

//------------------------------------------------------------------------------
namespace remo {
	namespace trans {
//------------------------------------------------------------------------------	


class Writer {
public:
	Writer(Packet& a_packet);

	void write(unsigned char byte);

private:
	Packet& m_packet;
};

class BinaryWriter: public Writer
{
public:
	BinaryWriter(Packet& a_packet): Writer(a_packet) {}

	template<typename... Args>
	void write_call(const std::string a_function, Args... args)
	{
		// write packet type
		write(PacketType::packet_call);
		// write function name
		write_value(a_function.c_str());
		// write arguments
		REMO_FOREACH_ARG(args, write_value);
	}

	void write_result(const TypedValue& a_result, const ArgList& a_args)
	{
		// write packet type
		write(PacketType::packet_result);
		// write function result
		write_value(a_result);
		// write output parameters
		for (const TypedValue& arg : a_args) {
			if (is_ptr_type(arg.type())) {
				write_value(arg);
			}
		}
	}

	// write scalar value
	template<typename T>
	void write_value(const T& a_value)
	{
		// make copy of value, as we need to modify and restore it
		T value = a_value;
		unsigned char* p = reinterpret_cast<unsigned char*>(&value);
		
		// determine number of leading non-zero bytes
		uint8_t wire_size = 0;
		LITTLE_ENDIAN_FOR(i, sizeof(value)) {
			if (!value) {
				break;
			}
			++wire_size;
			p[i] = 0;
		}
		// restore value
		value = a_value;

		// write type info byte
		write((wire_size << 4) | TypeInfo<T>::id());

		// output actual bytes		
		LITTLE_ENDIAN_FOR(i, wire_size) {
			write(p[i]);
		}
	}

	// write array
	template<typename T, int N>
	void write_value(T(&a_array)[N]) {
		static_assert(dependent_false<T>::value, "array types not yet supported by libremo");
	}

	// write pointer
	template<typename T>
	void write_value(T* a_ptr)
	{
		// assume size of 1 if not specified
		if (!m_has_arraysize) {
			m_arraysize = 1;
		}

		// get pointer to array data
		unsigned char* p = reinterpret_cast<unsigned char*>(a_ptr);
		
		// write type info byte
		write(TypeInfo<T*>::id());

		// output actual bytes
		for (size_t j = 0; j < m_arraysize.value; j++) {
			LITTLE_ENDIAN_FOR(i, sizeof(*a_ptr)) {
				write(p[i]);
			}
		}
	}	

	// write array size
	void write_value(arraysize_t a_size);

	// write string
	void write_value(char* a_string);
	void write_value(const char* a_string);
	
	// write boolean
	void write_value(bool a_bool);
	
	// write typed value
	void write_value(const TypedValue& a_value);

private:
	bool m_has_arraysize = false;
	arraysize_t m_arraysize = {};
};

//------------------------------------------------------------------------------
	} // namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
