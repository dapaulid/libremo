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

#include "types.h"

#include <string>

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

// forward declaration
class Controller;

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------	
//
class Item {
public:
	Item(const std::string& a_name);
	virtual ~Item();

	virtual TypedValue call(ArgList args) = 0;

	virtual std::string to_string() const;

	const std::string& get_name() const { return m_name; }
	std::string get_full_name() const;

	static bool is_valid_name(const std::string& a_name);

protected:
	friend class Controller;
	void set_ctrl(Controller* a_ctrl) { m_ctrl = a_ctrl; }
	const Controller* get_ctrl() const { return m_ctrl; }

protected:
	//! to be overriden by subclass
	virtual const char* item_type() { return "item"; }

private:
	//! controller to which this item belongs
	Controller* m_ctrl;
	//! item name
	std::string m_name;
};


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
