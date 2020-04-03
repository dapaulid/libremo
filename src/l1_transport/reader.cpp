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

#include "utils/logger.h"

#include <stdio.h>
#include <sstream>
#include <iomanip>

//------------------------------------------------------------------------------
namespace remo {
	namespace trans {
//------------------------------------------------------------------------------

//! logger instance
static Logger logger("Reader");


//------------------------------------------------------------------------------
// class Reader
//------------------------------------------------------------------------------

Reader::Reader(const Buffer& a_buffer):
	m_buffer(a_buffer), m_offset(0)
{
}

//------------------------------------------------------------------------------

void Reader::skip_array(size_t a_arraylength, size_t a_item_size)
{
	size_t total_size = a_arraylength * a_item_size;
	if (m_offset + total_size > m_buffer.get_size()) {
		REMO_THROW(ErrorCode::ERR_INVALID_ARRAY_LENGTH, 
			"invalid array length: %zu", a_arraylength);
	}
	m_offset += total_size;
}


//------------------------------------------------------------------------------
// class BinaryReader
//------------------------------------------------------------------------------

void BinaryReader::read_call()
{
	// locals
	uint8_t modifier = 0;

	// expect 'call' packet
	if (read<uint8_t>() != PacketType::packet_call) {
		REMO_THROW(ErrorCode::ERR_BAD_PACKET, "not a 'call' packet");
	}

	// expect function name
	if (read_type(modifier) != TypeId::type_cstr) {
		REMO_THROW(ErrorCode::ERR_FUNC_NAME_MISSING, "Function name missing");
	}

	// read function name
	size_t start_offset = m_offset;
	const char* a_name = read_cstr();
	m_function.assign(a_name, m_offset - start_offset - 1); // no trailing NUL

	// read arguments
	while (has_more()) {
		m_args.push_back(read_typed_value());
	}
}

//------------------------------------------------------------------------------

TypedValue BinaryReader::read_typed_value()
{
	// read header byte
	uint8_t modifier = 0;
	TypeId type = read_type(modifier);

	switch (type) {
	case type_null: 
		return TypedValue(TypeId::type_null); 
	case type_uint8:
		return TypedValue(read_value<uint8_t>(modifier));
	case type_uint16:
		return TypedValue(read_value<uint16_t>(modifier));
	case type_uint32:
		return TypedValue(read_value<uint32_t>(modifier));
	case type_uint64:
		return TypedValue(read_value<uint64_t>(modifier));
	case type_int8:
		return TypedValue(read_value<int8_t>(modifier));
	case type_int16:
		return TypedValue(read_value<int16_t>(modifier));
	case type_int32:
		return TypedValue(read_value<int32_t>(modifier));
	case type_int64:
		return TypedValue(read_value<int64_t>(modifier));
	case type_void:
		return TypedValue(TypeId::type_void);
//			case type_any:
	case type_bool:
		return TypedValue(modifier != 0);
	case type_cstr:
		return TypedValue(read_cstr());
	case type_double:
		return TypedValue(read_value<double>(modifier));
//			case type_error:
	case type_float:
		return TypedValue(read_value<float>(modifier));
	// pointer types
	case type_uint8_ptr:
		return TypedValue(read_ptr<uint8_t>(modifier));
	case type_uint16_ptr:
		return TypedValue(read_ptr<uint16_t>(modifier));
	case type_uint32_ptr:
		return TypedValue(read_ptr<uint32_t>(modifier));
	case type_uint64_ptr:
		return TypedValue(read_ptr<uint64_t>(modifier));
	case type_int8_ptr:
		return TypedValue(read_ptr<int8_t>(modifier));
	case type_int16_ptr:
		return TypedValue(read_ptr<int16_t>(modifier));
	case type_int32_ptr:
		return TypedValue(read_ptr<int32_t>(modifier));
	case type_int64_ptr:
		return TypedValue(read_ptr<int64_t>(modifier));
//			case type_void:
//			case type_any:
	case type_bool_ptr:
		return TypedValue(read_ptr<bool>(modifier));
	//case type_cstr_ptr:
	case type_double_ptr:
		return TypedValue(read_ptr<double>(modifier));
//			case type_error:
	case type_float_ptr:
		return TypedValue(read_ptr<float>(modifier));
	default:
		REMO_THROW(ErrorCode::ERR_PARAM_TYPE_INVALID, 
			"invalid parameter type: %s (0x%02X)", get_type_name(type), type);
	} // end switch
}

//------------------------------------------------------------------------------

std::string BinaryReader::to_string()
{
	if (m_buffer.get_size() > 0) {
		// check packet type
		switch (read<uint8_t>()) {
		case PacketType::packet_call:
			// call
			return "call: " + format_call();
		case PacketType::packet_result:
			// result
			return "result: " + format_result();
		default:
			// unknown packet type
			std::stringstream ss;
			ss << "unknown packet of size " << m_buffer.get_size() << ": ";
			ss << '[' << m_buffer.to_hex() << ']';
			return ss.str();
		}
	} else {
		return "(empty packet)";
	}
}

//------------------------------------------------------------------------------

std::string BinaryReader::format_call()
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
		ss << format_value();
		if (has_more()) {
			ss << ", ";
		} // end if
	} // end while
	ss << ')';

	return ss.str();
}

//------------------------------------------------------------------------------

std::string BinaryReader::format_result()
{
	// locals
	std::stringstream ss;

	// print actual result
	ss << format_value();

	// print out parameters
	ss << " (";
	while (has_more()) {
		ss << format_value();
		if (has_more()) {
			ss << ", ";
		} // end if
	} // end while
	ss << ')';	

	return ss.str();
}

//------------------------------------------------------------------------------

std::string BinaryReader::format_value()
{
	// locals
	std::stringstream ss;
	uint8_t modifier = 0;

	// read header byte
	TypeId type = read_type(modifier);

	// print value
	switch (type) {
	case type_null:
		ss << '(' << get_type_name(type) << ')';
		break;
	case type_uint8:
	case type_uint8_ptr:
		ss << '(' << get_type_name(type) << ')';
		ss << read_value<uint8_t>(modifier);
		break;
	case type_uint16:
	case type_uint16_ptr:
		ss << '(' << get_type_name(type) << ')';
		ss << read_value<uint16_t>(modifier);
		break;
	case type_uint32:
	case type_uint32_ptr:
		ss << '(' << get_type_name(type) << ')';
		ss << read_value<uint32_t>(modifier);
		break;
	case type_uint64:
	case type_uint64_ptr:
		ss << '(' << get_type_name(type) << ')';
		ss << read_value<uint64_t>(modifier);
		break;
	case type_int8:
	case type_int8_ptr:
		ss << '(' << get_type_name(type) << ')';
		ss << read_value<int8_t>(modifier);
		break;
	case type_int16:
	case type_int16_ptr:
		ss << '(' << get_type_name(type) << ')';
		ss << read_value<int16_t>(modifier);
		break;
	case type_int32:
	case type_int32_ptr:
		ss << '(' << get_type_name(type) << ')';
		ss << read_value<int32_t>(modifier);
		break;
	case type_int64:
	case type_int64_ptr:
		ss << '(' << get_type_name(type) << ')';
		ss << read_value<int64_t>(modifier);
		break;
	case type_void:
	case type_void_ptr:
		ss << '(' << get_type_name(type) << ')';
		break;
	case type_any:
	case type_any_ptr:
		ss << '(' << get_type_name(type) << ')';
		break;
	case type_bool:
		ss << (modifier != 0 ? "true" : "false");
		break;
	case type_bool_ptr:
		ss << '(' << get_type_name(type) << ')';
		ss << (read_value<int64_t>(modifier) ? "true" : "false");
		break;
	case type_cstr:
	case type_cstr_ptr:
		ss << "\"" << read_cstr() << "\""; // TODO escaping (for C++14 we could use std::quote...)
		break;
	case type_double:
	case type_double_ptr:
		ss << '(' << get_type_name(type) << ')';
		ss << read_value<double>(modifier);
		break;
	case type_error:
	case type_error_ptr:
		ss << '(' << get_type_name(type) << ')';
		break;
	case type_float:
	case type_float_ptr:
		ss << '(' << get_type_name(type) << ')';
		ss << read_value<float>(modifier);
		break;
	default:
		ss << "(0x" << std::hex << std::setw(2) << std::setfill('0') << type << ')';
		break;
	} // end switch

	return ss.str();
}

//------------------------------------------------------------------------------
//
void BinaryReader::check_param_type(TypeId a_actual_type, TypeId a_expected_type) const
{
	if (a_actual_type != a_expected_type) {
		REMO_THROW(ErrorCode::ERR_OUTPARAM_TYPE_MISMATCH, 
			"out parameter type mismatch: expected %s, got %s", 
			get_type_name(a_expected_type), get_type_name(a_actual_type));
	}
}

//------------------------------------------------------------------------------
//
void BinaryReader::check_result_packet(PacketType a_packet_type) const
{
	if (a_packet_type != PacketType::packet_result) {
		REMO_THROW(ErrorCode::ERR_BAD_PACKET, "not a 'result' packet");
	}
}


//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
