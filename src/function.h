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


//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------
//
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
// class definition
//------------------------------------------------------------------------------
//
template <typename Ret, typename...Arg>
class bound_function: public function
{
public:
	bound_function(const std::string& a_name, Ret (*a_func)(Arg...)):
		function(a_name, TypeInfo<Ret>::id(), { TypeInfo<Arg>::id()... }),
		m_func(a_func)
	{
	}

	virtual TypedValue call(const ArgList& args) override
	{
		check_args(args);
		return TypedValue(dynamic_call<decltype(m_func), Ret, Arg...>(m_func, args));
	}

private:
	Ret (*m_func)(Arg...);
};

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------
//
template<typename Lambda>
class lambda_function: public function
{
public:
	lambda_function(const std::string& a_name, Lambda a_lambda):
		lambda_function(a_name, a_lambda, &Lambda::operator())
	{
	}

	virtual TypedValue call(const ArgList& args) override
	{
		check_args(args);
		return call(args, &Lambda::operator());
	}


// helper overloads to capture result and parameter types
private:
	template<typename Class, typename Ret, typename... Args>
	lambda_function(const std::string& a_name, Lambda a_lambda, Ret (Class::*)(Args...) const):
		function(a_name, TypeInfo<Ret>::id(), { TypeInfo<Args>::id()... }),
		m_lambda(a_lambda)	
	{
	}

	template<typename Class, typename Ret, typename... Args>
	TypedValue call(const ArgList& args, Ret (Class::*)(Args...) const)
	{
		return TypedValue(dynamic_call<Lambda, Ret, Args...>(m_lambda, args));
	}

private:
	Lambda m_lambda;
};


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
