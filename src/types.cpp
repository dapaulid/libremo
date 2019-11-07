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


const char* get_type_name(TypeId id)
{
	switch (id) {
		case type_null       : return "null";
		case type_uint8      : return "uint8";
		case type_uint16     : return "uint16";
		case type_uint32     : return "uint32";
		case type_uint64     : return "uint64";
		case type_int8       : return "int8";
		case type_int16      : return "int16";
		case type_int32      : return "int32";
		case type_int64      : return "int64";
		case type_void       : return "void";
		case type_any        : return "any";
		case type_bool       : return "bool";
		case type_cstr       : return "char*";
		case type_double     : return "double";
		case type_error      : return "error";
		case type_float      : return "float";
		default              : return "(unknown)";
	}
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
