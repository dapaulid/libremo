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

#include "writer.h"
#include "dynamic_call.h"
#include "packet.h"

#include <string>


double add_numbers(const double f1, const double f2);

double subtract_numbers(const double f1, const double f2);

double multiply_numbers(const double f1, const double f2);

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


class Controller {
public:
	template<typename... Args>
	void call(const std::string a_function, Args... args)
	{
	}

private:

};

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
