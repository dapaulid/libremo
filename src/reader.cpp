//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "reader.h"

#include <stdio.h>
#include <sstream>
#include <iomanip>

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class Reader
//------------------------------------------------------------------------------

Reader::Reader(const Packet& a_packet):
	m_packet(a_packet), m_offset(0)
{
}

//------------------------------------------------------------------------------

unsigned char Reader::read()
{
	return m_packet.get_byte(m_offset++);
}

//------------------------------------------------------------------------------

bool Reader::has_more() const
{
	return m_offset < m_packet.get_size();
}


//------------------------------------------------------------------------------
// class BinaryReader
//------------------------------------------------------------------------------

void BinaryReader::read_call()
{
	// locals
	uint8_t modifier = 0;

	// expect function name
	if (read_type(modifier) != TypeId::type_cstr) {
		throw error(ErrorCode::ERR_FUNC_NAME_MISSING, "Function name missing");
	}

	// read function name
	size_t start_offset = m_offset;
	const char* a_name = read_cstr();
	m_function.assign(a_name, m_offset - start_offset - 1); // no trailing NUL

	// read arguments
	while (has_more()) {

		// read header byte
		TypeId type = read_type(modifier);

		switch (type) {
		case type_null: 
			break;
		case type_uint8:
			m_args.push_back(read_value<uint8_t>(modifier));
			break;
		case type_uint16:
			m_args.push_back(read_value<uint16_t>(modifier));
			break;
		case type_uint32:
			m_args.push_back(read_value<uint32_t>(modifier));
			break;
		case type_uint64:
			m_args.push_back(read_value<uint64_t>(modifier));
			break;
		case type_int8:
			m_args.push_back(read_value<int8_t>(modifier));
			break;
		case type_int16:
			m_args.push_back(read_value<int16_t>(modifier));
			break;
		case type_int32:
			m_args.push_back(read_value<int32_t>(modifier));
			break;
		case type_int64:
			m_args.push_back(read_value<int64_t>(modifier));
			break;
//			case type_void:
//			case type_any:
		case type_bool:
			m_args.push_back(modifier != 0);
			break;
		case type_cstr:
			m_args.push_back(read_cstr());
			break;
		case type_double:
			m_args.push_back(read_value<double>(modifier));
			break;
//			case type_error:
		case type_float:
			m_args.push_back(read_value<float>(modifier));
			break;
		default:
			throw error(ErrorCode::ERR_PARAM_TYPE_INVALID, 
				"Invalid parameter type: %s (%02x)", get_type_name(type), type);
		} // end switch
	} // end while
}

std::string BinaryReader::to_string()
{
	// locals
	std::stringstream ss;
	uint8_t modifier = 0;

	// print function name
	if (read_type(modifier) == TypeId::type_cstr) {
		ss << read_cstr();
	} else {
		ss << "???";
	}

	// print arguments
	ss << '(';
	while (has_more()) {

		// read header byte
		TypeId type = read_type(modifier);

		// print value
		switch (type) {
		case type_null:
			break;
		case type_uint8:
			ss << '(' << get_type_name(type) << ')';
			ss << read_value<uint8_t>(modifier);
			break;
		case type_uint16:
			ss << '(' << get_type_name(type) << ')';
			ss << read_value<uint16_t>(modifier);
			break;
		case type_uint32:
			ss << '(' << get_type_name(type) << ')';
			ss << read_value<uint32_t>(modifier);
			break;
		case type_uint64:
			ss << '(' << get_type_name(type) << ')';
			ss << read_value<uint64_t>(modifier);
			break;
		case type_int8:
			ss << '(' << get_type_name(type) << ')';
			ss << read_value<int8_t>(modifier);
			break;
		case type_int16:
			ss << '(' << get_type_name(type) << ')';
			ss << read_value<int16_t>(modifier);
			break;
		case type_int32:
			ss << '(' << get_type_name(type) << ')';
			ss << read_value<int32_t>(modifier);
			break;
		case type_int64:
			ss << '(' << get_type_name(type) << ')';
			ss << read_value<int64_t>(modifier);
			break;
		case type_void:
			ss << '(' << get_type_name(type) << ')';
			break;
		case type_any:
			ss << '(' << get_type_name(type) << ')';
			break;
		case type_bool:
			ss << (modifier != 0 ? "true" : "false");
			break;
		case type_cstr:
			ss << std::quoted(read_cstr()); // TODO escaping
			break;
		case type_double:
			ss << '(' << get_type_name(type) << ')';
			ss << read_value<double>(modifier);
			break;
		case type_error:
			ss << '(' << get_type_name(type) << ')';
			break;
		case type_float:
			ss << '(' << get_type_name(type) << ')';
			ss << read_value<float>(modifier);
			break;
		default:
			break;
		} // end switch

		if (has_more()) {
			ss << ", ";
		}
	} // end while
	ss << ')';

	return ss.str();
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
