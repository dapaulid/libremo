//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
#include "endianness.h"
//
// C++ 
#include <cstring>
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
#define REMO_BSWAP16(x) __builtin_bswap16(x)
#define REMO_BSWAP32(x) __builtin_bswap32(x)
#define REMO_BSWAP64(x) __builtin_bswap64(x)

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
// functions
//------------------------------------------------------------------------------
//
inline uint16_t get_le(const uint16_t* a_ptr)
{
	return REMO_BE16(*a_ptr);
}

//------------------------------------------------------------------------------
//
inline uint32_t get_le(const uint32_t* a_ptr)
{
	return REMO_BE32(*a_ptr);
}

//------------------------------------------------------------------------------
//
inline uint64_t get_le(const uint64_t* a_ptr)
{
	return REMO_BE64(*a_ptr);
}

//------------------------------------------------------------------------------
//
inline uint16_t get_le_ua(const uint16_t* a_ptr)
{
	uint16_t x;
	std::memcpy(&x, a_ptr, sizeof(x));
	return REMO_LE16(x);
}

//------------------------------------------------------------------------------
//
inline uint32_t get_le_ua(const uint32_t* a_ptr)
{
	uint32_t x;
	std::memcpy(&x, a_ptr, sizeof(x));
	return REMO_LE32(x);
}

//------------------------------------------------------------------------------
//
inline uint64_t get_le_ua(const uint64_t* a_ptr)
{
	uint64_t x;
	std::memcpy(&x, a_ptr, sizeof(x));
	return REMO_LE64(x);
}

//------------------------------------------------------------------------------
	} // end namespace sys
} // end namespace remo
//------------------------------------------------------------------------------
