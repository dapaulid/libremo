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
#include <vector>

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------	
//
class RemoteEndpoint;


//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------	
//
class LocalEndpoint: public Endpoint {
public:
	LocalEndpoint();
	virtual ~LocalEndpoint();

	RemoteEndpoint* connect(const std::string& a_remote);

	template <typename Ret, typename...Arg>
	void bind(const std::string& a_name, Ret (*a_func)(Arg...));

	template<typename Lambda>
	void bind(const std::string& a_name, Lambda a_lambda);


protected:
	friend class Item;
	void register_item(Item* a_item);
	void unregister_item(Item* a_item);
	void clear_items();
	Item* find_item(const std::string& a_full_name);

	void add_remote(RemoteEndpoint* a_remote);
	void clear_remotes();

protected:
	friend class RemoteEndpoint;
	TypedValue call(const std::string& a_func_name, const ArgList& args);

private:
	//! container for looking up items by full name
	std::unordered_map<std::string, Item*> m_items;	
	//! list of remote endpoints that represent this endpoint to the outside
	std::vector<RemoteEndpoint*> m_remotes;
};


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------

// template implementation
#include "local_endpoint.tpp.h"
