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

// pointer types
	type_uint8_ptr      = 0xA1,
	type_uint16_ptr     = 0xA2,
	type_uint32_ptr     = 0xA3,
	type_uint64_ptr     = 0xA4,
	type_int8_ptr       = 0xA5,
	type_int16_ptr      = 0xA6,
	type_int32_ptr      = 0xA7,
	type_int64_ptr      = 0xA8,
	type_void_ptr       = 0xA9,	
	type_any_ptr        = 0xAA,
	type_bool_ptr       = 0xAB,
	type_cstr_ptr       = 0xAC,
	type_double_ptr     = 0xAD,
	type_error_ptr      = 0xAE,	
	type_float_ptr      = 0xAF,

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
struct TypeInfo<uint8_t> {
	static TypeId id() { return type_uint8; }
};

template<>
struct TypeInfo<uint16_t> {
	static TypeId id() { return type_uint16; }
};

template<>
struct TypeInfo<uint32_t> {
	static TypeId id() { return type_uint32; }
};

template<>
struct TypeInfo<uint64_t> {
	static TypeId id() { return type_uint64; }
};

template<>
struct TypeInfo<int8_t> {
	static TypeId id() { return type_int8; }
};

template<>
struct TypeInfo<int16_t> {
	static TypeId id() { return type_int16; }
};

template<>
struct TypeInfo<int32_t> {
	static TypeId id() { return type_int32; }
};

template<>
struct TypeInfo<int64_t> {
	static TypeId id() { return type_int64; }
};

template<>
struct TypeInfo<bool> {
	static TypeId id() { return type_bool; }
};

template<>
struct TypeInfo<double> {
	static TypeId id() { return type_double; }
};

template<>
struct TypeInfo<float> {
	static TypeId id() { return type_float; }
};

// pointer types

template<>
struct TypeInfo<uint8_t*> {
	static TypeId id() { return type_uint8_ptr; }
};

template<>
struct TypeInfo<uint16_t*> {
	static TypeId id() { return type_uint16_ptr; }
};

template<>
struct TypeInfo<uint32_t*> {
	static TypeId id() { return type_uint32_ptr; }
};

template<>
struct TypeInfo<uint64_t*> {
	static TypeId id() { return type_uint64_ptr; }
};

template<>
struct TypeInfo<int8_t*> {
	static TypeId id() { return type_int8_ptr; }
};

template<>
struct TypeInfo<int16_t*> {
	static TypeId id() { return type_int16_ptr; }
};

template<>
struct TypeInfo<int32_t*> {
	static TypeId id() { return type_int32_ptr; }
};

template<>
struct TypeInfo<int64_t*> {
	static TypeId id() { return type_int64_ptr; }
};

template<>
struct TypeInfo<bool*> {
	static TypeId id() { return type_bool_ptr; }
};

template<>
struct TypeInfo<double*> {
	static TypeId id() { return type_double_ptr; }
};

template<>
struct TypeInfo<float*> {
	static TypeId id() { return type_float_ptr; }
};

template<>
struct TypeInfo<const char*> {
	static TypeId id() { return type_cstr; }
};



// TODO better place
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	#define LITTLE_ENDIAN_FOR(i, size) for (int i = 0; i < (int)size; i++)
#else
	#define LITTLE_ENDIAN_FOR(i, size) for (int i = (int)size-1; i >= 0; i--)
#endif


typedef std::any any;

struct TypedValue {
	TypedValue(TypeId a_type, const any& a_value):
		type(a_type), value(a_value) {}
	TypeId type;
	any value;
};

typedef std::vector<TypedValue> ArgList;


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
