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

#include "error.h"

#include <stdint.h>
#include <stddef.h> // size_t

#include <vector>


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

enum TypeModifier {
	modifier_ptr = 0xA0
};

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
	type_uint8_ptr      = type_uint8   | modifier_ptr,
	type_uint16_ptr     = type_uint16  | modifier_ptr,
	type_uint32_ptr     = type_uint32  | modifier_ptr,
	type_uint64_ptr     = type_uint64  | modifier_ptr,
	type_int8_ptr       = type_int8    | modifier_ptr,
	type_int16_ptr      = type_int16   | modifier_ptr,
	type_int32_ptr      = type_int32   | modifier_ptr,
	type_int64_ptr      = type_int64   | modifier_ptr,
	type_void_ptr       = type_void    | modifier_ptr,	
	type_any_ptr        = type_any     | modifier_ptr,
	type_bool_ptr       = type_bool    | modifier_ptr,
	type_cstr_ptr       = type_cstr    | modifier_ptr,
	type_double_ptr     = type_double  | modifier_ptr,
	type_error_ptr      = type_error   | modifier_ptr,	
	type_float_ptr      = type_float   | modifier_ptr,

};

typedef std::vector<TypeId> TypeList;

const char* get_type_name(TypeId id);
size_t get_type_size(TypeId id);
bool is_ptr_type(TypeId id);

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

// TODO better place
// NOTE: the "0," avoids the "ISO C++ forbids zero-size array" pendantic warning
#define REMO_FOREACH_ARG(args, what) { int dummy[] = { 0,(what(args),0)... }; (void)dummy; }

//------------------------------------------------------------------------------
// struct definition
//------------------------------------------------------------------------------
//
//! type-safe union type to hold any supported type, similar to std::variant
struct TypedValue
{
	template<typename T>
	TypedValue(const T& value) {
		m_type = TypeInfo<T>::id();
		static_assert(sizeof(T) <= sizeof(m_value), 
			"type too large for TypedValue");
		reinterpret_cast<T&>(m_value) = value;
	}

	template<typename T>
	T get() const {
		// check type
		TypeId expected = TypeInfo<T>::id();
		if (expected != m_type) {
			throw error(ErrorCode::ERR_BAD_VALUE_ACCESS, 
				"bad typed value access: expected '%s', got '%s'",
				get_type_name(expected), get_type_name(m_type));
		}
		// return value
		return reinterpret_cast<const T&>(m_value);
	}

	TypeId type() const {
		return m_type;
	}

private:
	//! value type
	TypeId m_type;
	//! value storage. must be large enough to hold any supported value
	uint64_t m_value;
};

typedef std::vector<TypedValue> ArgList;


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
