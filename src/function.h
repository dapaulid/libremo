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

#include "dynamic_call.h"
#include "types.h"

#include <string>

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

class function {
public:
	function(const std::string& a_name, const TypeList& a_param_types);
	virtual ~function();

	virtual any call(ArgList args) = 0;

protected:
	void check_args(const ArgList& args);

private:
	std::string m_name;
	TypeList m_param_types;
};

//------------------------------------------------------------------------------	

template <typename Ret, typename...Arg>
class bound_function: public function {
public:
	bound_function(const std::string& a_name, Ret (*a_func)(Arg...)):
		function(a_name, { TypeInfo<Arg>::id()... }), 
		m_func(a_func)
	{
	}

	virtual any call(ArgList args) override
	{
		return dynamic_call(m_func, args);
	}

private:
	Ret (*m_func)(Arg...);
};

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
