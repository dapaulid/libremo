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

#include <stdlib.h>
#include <iostream> // debugging

//------------------------------------------------------------------------------
namespace remo {
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
		int dummy[] = { write_value(args)... };	(void) dummy;
	}

	// write scalar value
	template<typename T>
	int write_value(const T& a_value)
	{
		// make copy of value, as we need to modify and restore it
		T value = a_value;
		unsigned char* p = reinterpret_cast<unsigned char*>(&value);
		
		// determine number of leading non-zero bytes
		size_t wire_size = 0;
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

		return 0;
	}

	// write array
	template<typename T, int N>
	int write_value(T(&a_array)[N]) {
		static_assert(dependent_false<T>::value, "array types not yet supported by libremo");
		return 0;
	}

	// write pointer
	template<typename T>
	int write_value(T* a_ptr) {
		//static_assert(dependent_false<T>::value, "pointer types not yet supported by libremo");
		if (!has_arraysize_) {
			// TODO exception
			// TODO check statically using fold expression or similar?
			printf("ERROR: pointer must be preceded by arraysize\n");
			return 0;
		}

		printf("pointer\n");
		return 0;
	}	

	// write array size
	int write_value(arraysize_t a_size);

	// write string
	int write_value(char* a_string);
	int write_value(const char* a_string);
	
	// write boolean
	int write_value(bool a_bool);

private:
	bool has_arraysize_ = false;
	arraysize_t arraysize_ = {};
};

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
