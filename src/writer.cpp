//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "writer.h"


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

Writer::Writer(Packet& a_packet):
	m_packet(a_packet)
{
}

//------------------------------------------------------------------------------

void Writer::write(unsigned char byte)
{
	m_packet.append(byte);
}

//------------------------------------------------------------------------------

int BinaryWriter::write_value(const char* a_string)
{
	// write type info byte
	write(TypeId::type_cstr);
	
	// output characters
	for (const char* p = a_string; *p; ++p) {
		write(*p);
	}

	// output terminating NUL character
	write(0);

	return 0;
}

//------------------------------------------------------------------------------

int BinaryWriter::write_value(char* a_string)
{
	return write_value((const char*)a_string);
}

//------------------------------------------------------------------------------

int BinaryWriter::write_value(bool a_bool)
{
	// header: combine value and type id
	write(((a_bool & 1) << 4) | TypeId::type_bool);
	return 0;
}

//------------------------------------------------------------------------------

int BinaryWriter::write_value(arraysize_t a_size)
{
	if (m_has_arraysize) {
		throw error(ErrorCode::ERR_POINTER_NEEDS_SIZE, 
			"arraysize_t parameter required before pointer type");
	}
	m_arraysize = a_size;
	m_has_arraysize = true;
	return 0;
}

//------------------------------------------------------------------------------

int BinaryWriter::write_value(const TypedValue& a_value)
{
	switch (a_value.type) {
	case type_null: 
		break;
	case type_uint8:
		return write_value(std::any_cast<uint8_t>(a_value.value));
	case type_uint16:
		return write_value(std::any_cast<uint16_t>(a_value.value));
	case type_uint32:
		return write_value(std::any_cast<uint32_t>(a_value.value));
	case type_uint64:
		return write_value(std::any_cast<uint64_t>(a_value.value));
	case type_int8:
		return write_value(std::any_cast<int8_t>(a_value.value));
	case type_int16:
		return write_value(std::any_cast<int16_t>(a_value.value));
	case type_int32:
		return write_value(std::any_cast<int32_t>(a_value.value));
	case type_int64:
		return write_value(std::any_cast<int64_t>(a_value.value));
	case type_void:
		break;
	case type_any:
		break;
	case type_bool:
		return write_value(std::any_cast<bool>(a_value.value));
	case type_cstr:
		return write_value(std::any_cast<const char*>(a_value.value));
	case type_double:
		return write_value(std::any_cast<double>(a_value.value));
//	case type_error:
	case type_float:
		return write_value(std::any_cast<float>(a_value.value));
	// pointer types
	case type_uint8_ptr:
		return write_value(std::any_cast<uint8_t*>(a_value.value));
	case type_uint16_ptr:
		return write_value(std::any_cast<uint16_t*>(a_value.value));
	case type_uint32_ptr:
		return write_value(std::any_cast<uint32_t*>(a_value.value));
	case type_uint64_ptr:
		return write_value(std::any_cast<uint64_t*>(a_value.value));
	case type_int8_ptr:
		return write_value(std::any_cast<int8_t*>(a_value.value));
	case type_int16_ptr:
		return write_value(std::any_cast<int16_t*>(a_value.value));
	case type_int32_ptr:
		return write_value(std::any_cast<int32_t>(a_value.value));
	case type_int64_ptr:
		return write_value(std::any_cast<int64_t>(a_value.value));
//			case type_void:
//			case type_any:
	case type_bool_ptr:
		return write_value(std::any_cast<bool*>(a_value.value));
	//case type_cstr_ptr:
	case type_double_ptr:
		return write_value(std::any_cast<double*>(a_value.value));
//			case type_error:
	case type_float_ptr:
		return write_value(std::any_cast<float*>(a_value.value));
	default:
		throw error(ErrorCode::ERR_INVALID_VALUE_TYPE, 
			"invalid value type: %s (0x%02X)", get_type_name(a_value.type), a_value.type);
	} // end switch

	return 0;
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
