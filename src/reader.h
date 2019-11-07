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

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

class Reader {
public:
	Reader(Packet& a_packet);

	unsigned char read();

	bool has_more() const;

protected:
	Packet& m_packet;
	size_t m_offset;
};

class BinaryReader: public Reader
{
public:
	BinaryReader(Packet& a_packet): Reader(a_packet), 
		m_args() {}

	void read_call()
	{
		while (has_more()) {
			// read header byte
			unsigned char h = read();
			TypeId tid = static_cast<TypeId>(h & 0xF);
			size_t wire_size = (h >> 4) & 0xF;

			switch (tid) {
			case type_null: 
				break;
			case type_uint8:
				m_args.push_back(read_value<uint8_t>(wire_size));
				break;
			case type_uint16:
				m_args.push_back(read_value<uint16_t>(wire_size));
				break;
			case type_uint32:
				m_args.push_back(read_value<uint32_t>(wire_size));
				break;
			case type_uint64:
				m_args.push_back(read_value<uint64_t>(wire_size));
				break;
			case type_int8:
				m_args.push_back(read_value<int8_t>(wire_size));
				break;
			case type_int16:
				m_args.push_back(read_value<int16_t>(wire_size));
				break;
			case type_int32:
				m_args.push_back(read_value<int32_t>(wire_size));
				break;
			case type_int64:
				m_args.push_back(read_value<int64_t>(wire_size));
				break;
//			case type_void:
//			case type_any:
			case type_bool:
				m_args.push_back(wire_size != 0);
				break;
			case type_cstr:
				m_args.push_back(m_packet.get_ptr<char>(m_offset));
				// forward until NUL byte
				while (read()) {};
				break;
			case type_double:
				m_args.push_back(read_value<double>(wire_size));
				break;
//			case type_error:
			case type_float:
				m_args.push_back(read_value<float>(wire_size));
				break;
			default:;
				throw error(ErrorCode::ERR_PARAM_TYPE_INVALID, 
					"Invalid parameter type: %d", tid);
			} // end switch
		} // end while
	}

	// write scalar value
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

	const ArgList& get_args() const { return m_args; }

private:
	ArgList m_args;
};

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
