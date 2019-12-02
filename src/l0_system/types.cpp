//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "types.h"

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


bool is_ptr_type(TypeId id)
{
	return (id & modifier_ptr) == modifier_ptr;
}

const char* get_type_name(TypeId id)
{
	switch (id) {
		case type_null       : return "null";
		case type_uint8      : return "uint8_t";
		case type_uint16     : return "uint16_t";
		case type_uint32     : return "uint32_t";
		case type_uint64     : return "uint64_t";
		case type_int8       : return "int8_t";
		case type_int16      : return "int16_t";
		case type_int32      : return "int32_t";
		case type_int64      : return "int64_t";
		case type_void       : return "void";
		case type_any        : return "any";
		case type_bool       : return "bool";
		case type_cstr       : return "char*";
		case type_double     : return "double";
		case type_error      : return "error";
		case type_float      : return "float";
	// pointer types
		case type_uint8_ptr  : return "uint8_t*";
		case type_uint16_ptr : return "uint16_t*";
		case type_uint32_ptr : return "uint32_t*";
		case type_uint64_ptr : return "uint64_t*";
		case type_int8_ptr   : return "int8_t*";
		case type_int16_ptr  : return "int16_t*";
		case type_int32_ptr  : return "int32_t*";
		case type_int64_ptr  : return "int64_t*";
		case type_void_ptr   : return "void*";
		case type_any_ptr    : return "any*";
		case type_bool_ptr   : return "bool*";
		case type_cstr_ptr   : return "char**";
		case type_double_ptr : return "double*";
		case type_error_ptr  : return "error*";
		case type_float_ptr  : return "float*";
	// unknown
		default              : return "(unknown)";
	}
}

//------------------------------------------------------------------------------	

size_t get_type_size(TypeId id)
{
	switch (id) {
		case type_null       : return 0;
		case type_uint8      : return 1;
		case type_uint16     : return 2;
		case type_uint32     : return 4;
		case type_uint64     : return 8;
		case type_int8       : return 1;
		case type_int16      : return 2;
		case type_int32      : return 4;
		case type_int64      : return 8;
		case type_void       : return 0;
		case type_any        : return 0;
		case type_bool       : return 1;
		case type_cstr       : return 0;
		case type_double     : return 8;
		case type_error      : return 0;
		case type_float      : return 4;
		default              : return 0;
	}
}


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
