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

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
#include "system.h"
//
// C++ 
#include <stdint.h>
#include <cstring> // memcpy
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace sys {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------
//
#if REMO_SYSTEM & REMO_SYS_WINDOWS
	// Windows
	#include <stdlib.h>
	#define REMO_BSWAP16(x) _byteswap_ushort(x)
	#define REMO_BSWAP32(x) _byteswap_ulong(x)
	#define REMO_BSWAP64(x) _byteswap_uint64(x)
#else
	// UNIX / Linux
	#define REMO_BSWAP16(x) __builtin_bswap16(x)
	#define REMO_BSWAP32(x) __builtin_bswap32(x)
	#define REMO_BSWAP64(x) __builtin_bswap64(x)	
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

#define REMO_LE16(x) (x)
#define REMO_LE32(x) (x)
#define REMO_LE64(x) (x)

#define REMO_BE16(x) REMO_BSWAP16(x)
#define REMO_BE32(x) REMO_BSWAP32(x)
#define REMO_BE64(x) REMO_BSWAP64(x)

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

#define REMO_LE16(x) REMO_BSWAP16(x)
#define REMO_LE32(x) REMO_BSWAP32(x)
#define REMO_LE64(x) REMO_BSWAP64(x)

#define REMO_BE16(x) (x)
#define REMO_BE32(x) (x)
#define REMO_BE64(x) (x)

#else
#error "unsupported endianness"
#endif


//------------------------------------------------------------------------------
// function declarations
//------------------------------------------------------------------------------
//
//! native to little-endian conversion
inline uint8_t get_le(const uint8_t* a_src);
inline uint16_t get_le(const uint16_t* a_src);
inline uint32_t get_le(const uint32_t* a_src);
inline uint64_t get_le(const uint64_t* a_src);
//! little-endian to native conversion
inline void set_le(uint8_t* a_dst, uint8_t a_value);
inline void set_le(uint16_t* a_dst, uint16_t a_value);
inline void set_le(uint32_t* a_dst, uint32_t a_value);
inline void set_le(uint64_t* a_dst, uint64_t a_value);

//! native to little-endian conversion for unaligned pointers
inline uint8_t get_le_ua(const uint8_t* a_src);
inline uint16_t get_le_ua(const uint16_t* a_src);
inline uint32_t get_le_ua(const uint32_t* a_src);
inline uint64_t get_le_ua(const uint64_t* a_src);
//! little-endian to native conversion for unaligned pointers
inline void set_le_ua(uint8_t* a_dst, uint8_t a_value);
inline void set_le_ua(uint16_t* a_dst, uint16_t a_value);
inline void set_le_ua(uint32_t* a_dst, uint32_t a_value);
inline void set_le_ua(uint64_t* a_dst, uint64_t a_value);


//------------------------------------------------------------------------------
// function implementations
//------------------------------------------------------------------------------
//
inline uint8_t get_le(const uint8_t* a_src)
{
	return *a_src;
}

//------------------------------------------------------------------------------
//
inline uint16_t get_le(const uint16_t* a_src)
{
	return REMO_LE16(*a_src);
}

//------------------------------------------------------------------------------
//
inline uint32_t get_le(const uint32_t* a_src)
{
	return REMO_LE32(*a_src);
}

//------------------------------------------------------------------------------
//
inline uint64_t get_le(const uint64_t* a_src)
{
	return REMO_LE64(*a_src);
}

//------------------------------------------------------------------------------
//
inline void set_le(uint8_t* a_dst, uint8_t a_value)
{
	*a_dst = a_value;
}

//------------------------------------------------------------------------------
//
inline void set_le(uint16_t* a_dst, uint16_t a_value)
{
	*a_dst = REMO_LE16(a_value);
}

//------------------------------------------------------------------------------
//
inline void set_le(uint32_t* a_dst, uint32_t a_value)
{
	*a_dst = REMO_LE32(a_value);
}

//------------------------------------------------------------------------------
//
inline void set_le(uint64_t* a_dst, uint64_t a_value)
{
	*a_dst = REMO_LE64(a_value);
}


//------------------------------------------------------------------------------
//
inline uint8_t get_le_ua(const uint8_t* a_src)
{
	return *a_src;
}

//------------------------------------------------------------------------------
//
inline uint16_t get_le_ua(const uint16_t* a_src)
{
	uint16_t x;
	std::memcpy(&x, a_src, sizeof(x));
	return REMO_LE16(x);
}

//------------------------------------------------------------------------------
//
inline uint32_t get_le_ua(const uint32_t* a_src)
{
	uint32_t x;
	std::memcpy(&x, a_src, sizeof(x));
	return REMO_LE32(x);
}

//------------------------------------------------------------------------------
//
inline uint64_t get_le_ua(const uint64_t* a_src)
{
	uint64_t x;
	std::memcpy(&x, a_src, sizeof(x));
	return REMO_LE64(x);
}

//------------------------------------------------------------------------------
//
inline void set_le_ua(uint8_t* a_dst, uint8_t a_value)
{
	*a_dst = a_value;
}

//------------------------------------------------------------------------------
//
inline void set_le_ua(uint16_t* a_dst, uint16_t a_value)
{
	a_value = REMO_LE16(a_value);
	std::memcpy(a_dst, &a_value, sizeof(a_value));
}

//------------------------------------------------------------------------------
//
inline void set_le_ua(uint32_t* a_dst, uint32_t a_value)
{
	a_value = REMO_LE32(a_value);
	std::memcpy(a_dst, &a_value, sizeof(a_value));
}

//------------------------------------------------------------------------------
//
inline void set_le_ua(uint64_t* a_dst, uint64_t a_value)
{
	a_value = REMO_LE64(a_value);
	std::memcpy(a_dst, &a_value, sizeof(a_value));
}


//------------------------------------------------------------------------------
	} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
