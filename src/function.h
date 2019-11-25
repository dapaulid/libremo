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

#include "item.h"
#include "dynamic_call.h"

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

class function: public Item {
public:
	function(const std::string& a_name, TypeId a_result_type, const TypeList& a_param_types);
	virtual ~function();

	std::string to_string() const override;

protected:
	void check_args(const ArgList& args);

	virtual const char* item_type() override { return "function"; }

protected:
	TypeId m_result_type;
	TypeList m_param_types;
};

//------------------------------------------------------------------------------	

template <typename Ret, typename...Arg>
class bound_function: public function {
public:
	bound_function(const std::string& a_name, Ret (*a_func)(Arg...)):
		function(a_name, TypeInfo<Ret>::id(), { TypeInfo<Arg>::id()... }), 
		m_func(a_func)
	{
	}

	virtual TypedValue call(ArgList args) override
	{
		check_args(args);
		return TypedValue(TypeInfo<Ret>::id(), dynamic_call(m_func, args));
	}

private:
	Ret (*m_func)(Arg...);
};

//------------------------------------------------------------------------------	

template<typename Lambda>//, typename Class, typename Ret, typename... Args>
class lambda_function: public function {
public:
	lambda_function(const std::string& a_name, Lambda& a_lambda)://, Ret (Class::*)(Args...) const):
		function(a_name, TypeInfo<uint32_t>::id(), { }), 
		m_lambda(a_lambda)
	{
		fuck(&Lambda::operator());
	}

	template<typename Class, typename Ret, typename... Args>
	void fuck(Ret (Class::*)(Args...) const) {
		m_result_type = TypeInfo<Ret>::id();
		m_param_types = { TypeInfo<Args>::id()... };
	}

	virtual TypedValue call(ArgList args) override
	{
		check_args(args);
		return fuck2(args, &Lambda::operator());
	}

	template<typename Class, typename Ret, typename... Args>
	TypedValue fuck2(ArgList args, Ret (Class::*)(Args...) const) {
		return TypedValue(TypeInfo<Ret>::id(), dynamic_call<Lambda, Ret, Args...>(m_lambda, args));
	}	

private:
	Lambda m_lambda;
};


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
