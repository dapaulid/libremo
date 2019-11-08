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

void Writer::write(unsigned char byte)
{
	m_packet.append(byte);
}

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

int BinaryWriter::write_value(char* a_string)
{
	return write_value((const char*)a_string);
}

int BinaryWriter::write_value(bool a_bool)
{
	// header: combine value and type id
	write(((a_bool & 1) << 4) | TypeId::type_bool);
	return 0;
}

int BinaryWriter::write_value(arraysize_t a_size)
{
	if (has_arraysize_) {
		throw error(ErrorCode::ERR_POINTER_NEEDS_SIZE, 
			"arraysize_t parameter required before pointer type");
	}
	arraysize_ = a_size;
	has_arraysize_ = true;
	return 0;
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
