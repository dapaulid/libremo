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
class LocalEndpoint;

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------	
//
class Item {
public:
	Item(const std::string& a_name);
	virtual ~Item();

	virtual TypedValue call(const ArgList& args) = 0;

	virtual std::string to_string() const;

	const std::string& get_name() const { return m_name; }
	std::string get_full_name() const;

	static bool is_valid_name(const std::string& a_name);

protected:
	friend class LocalEndpoint;
	void set_endpoint(LocalEndpoint* a_endpoint) { m_endpoint = a_endpoint; }
	const LocalEndpoint* get_endpoint() const { return m_endpoint; }

protected:
	//! to be overriden by subclass
	virtual const char* item_type() { return "item"; }

private:
	//! endpoint to which this item belongs
	LocalEndpoint* m_endpoint;
	//! item name
	std::string m_name;
};


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
