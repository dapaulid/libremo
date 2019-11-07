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

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

Reader::Reader(Packet& a_packet):
	m_packet(a_packet), m_offset(0)
{
}

unsigned char Reader::read()
{
	return m_packet.get_byte(m_offset++);
}

bool Reader::has_more() const
{
	return m_offset < m_packet.get_size();
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
