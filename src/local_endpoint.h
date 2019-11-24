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

#include "endpoint.h"
#include "item.h"

#include <unordered_map>

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------	
//
class LocalEndpoint: public Endpoint {
public:
	LocalEndpoint();
	virtual ~LocalEndpoint();

	template <typename Ret, typename...Arg>
	void bind(const std::string& a_name, Ret (*a_func)(Arg...));

protected:
	friend class Item;
	void register_item(Item* a_item);
	void unregister_item(Item* a_item);
	void clear_items();
	Item* find_item(const std::string& a_full_name);

protected:
	friend class RemoteEndpoint;
	TypedValue call(const std::string& a_func_name, ArgList args);

private:
	//! container for looking up items by full name
	std::unordered_map<std::string, Item*> m_items;	
};


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------

// template implementation
#include "local_endpoint.tpp.h"
