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

#include <stdint.h>
#include <stddef.h> // size_t

#include <vector>
#include <any>

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


enum TypeId: uint8_t {
	type_null       = 0x00,
	type_uint8      = 0x01,
	type_uint16     = 0x02,
	type_uint32     = 0x03,
	type_uint64     = 0x04,
	type_int8       = 0x05,
	type_int16      = 0x06,
	type_int32      = 0x07,
	type_int64      = 0x08,
	type_void       = 0x09,	
	type_any        = 0x0A, // reserved for extensions
	type_bool       = 0x0B,
	type_cstr       = 0x0C,
	type_double     = 0x0D,
	type_error      = 0x0E,	
	type_float      = 0x0F,
};

typedef std::vector<TypeId> TypeList;

const char* get_type_name(TypeId id);

template <typename T>
struct dependent_false { static constexpr bool value = false; };

struct arraysize_t {
	arraysize_t(size_t a_value = 0): value(a_value) {}
	size_t value;
};

template<typename T>
struct TypeInfo {
	typedef T type;
	static TypeId id() { 
		static_assert(dependent_false<T>::value, "type not supported by libremo");
		return type_null;
	}
};

template<>
struct TypeInfo<uint32_t> {
	static TypeId id() { return type_uint32; }
};

template<>
struct TypeInfo<bool> {
	static TypeId id() { return type_bool; }
};

template<>
struct TypeInfo<double> {
	static TypeId id() { return type_double; }
};

// TODO better place
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	#define LITTLE_ENDIAN_FOR(i, size) for (int i = 0; i < (int)size; i++)
#else
	#define LITTLE_ENDIAN_FOR(i, size) for (int i = (int)size-1; i >= 0; i--)
#endif


typedef std::any any;
typedef std::vector<any> ArgList;

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
