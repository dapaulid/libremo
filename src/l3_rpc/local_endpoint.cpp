//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "local_endpoint.h"

#include "remote_endpoint.h"
#include "utils/logger.h"


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------

//! logger instance
static Logger logger("LocalEndpoint");


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------
//
LocalEndpoint::LocalEndpoint():
	Endpoint(),
	m_items(),
	m_remotes()
{
}

//------------------------------------------------------------------------------
//
LocalEndpoint::~LocalEndpoint()
{
	clear_items();
	clear_remotes();
}

//------------------------------------------------------------------------------	
//
RemoteEndpoint* LocalEndpoint::connect(const std::string& a_remote)
{
	// TODO handle address
	(void)a_remote;
	RemoteEndpoint* remote = new RemoteEndpoint(this);
	add_remote(remote);
	return remote;
}

//------------------------------------------------------------------------------	
//
void LocalEndpoint::register_item(Item* a_item)
{
    // item exists?
    if (!a_item) {
        // no -> nothing to do
        return;
    }

    // fail if already registered at some endpoint
    REMO_THROW_IF(a_item->get_endpoint(),
        ErrorCode::ERR_ITEM_ALREADY_REGISTERED, 
        "%s is already registered: '%s'",
        a_item->item_type(), a_item->get_full_name().c_str());

    // insert item
    auto ret = m_items.insert(std::pair<std::string, Item*>(a_item->get_full_name(), a_item));
    REMO_THROW_IF(!ret.second, 
        ErrorCode::ERR_ITEM_ALREADY_EXISTING, 
        "%s with same name already exists: '%s'",
        ret.first->second->item_type(), ret.first->first.c_str());

    // remember us
    a_item->set_endpoint(this);

	// success
	REMO_INFO("Registered '%s'", a_item->to_string().c_str());
}

//------------------------------------------------------------------------------	
//
void LocalEndpoint::unregister_item(Item* a_item)
{
    // item exists?
    if (!a_item) {
        // no -> nothing to do
        return;
    }

    // remove item
    size_t count = m_items.erase(a_item->get_full_name());
    REMO_THROW_IF(count == 0, 
        ErrorCode::ERR_ITEM_NOT_FOUND, 
        "%s not found for unregistration: '%s'",
        a_item->item_type(), a_item->get_full_name().c_str());    

    // forget us
    a_item->set_endpoint(nullptr);

	// success
	REMO_INFO("Unregistered '%s'", a_item->to_string().c_str());
}

//------------------------------------------------------------------------------	
//
void LocalEndpoint::clear_items()
{
    for (auto it = m_items.begin(); it != m_items.end(); it++) {
        it->second->set_endpoint(nullptr);
        delete it->second;
    }
    m_items.clear();
}

//------------------------------------------------------------------------------	
//
Item* LocalEndpoint::find_item(const std::string& a_full_name)
{
    auto it = m_items.find(a_full_name);
    return it != m_items.end() ? it->second : nullptr;
}

//------------------------------------------------------------------------------
//
TypedValue LocalEndpoint::call(const std::string& a_func_name, const ArgList& args)
{
    // get function item
    Item* item = find_item(a_func_name);
    REMO_THROW_IF(!item,
        ErrorCode::ERR_RPC_NOT_FOUND, 
        "remote procedure not found: '%s'",
        a_func_name.c_str());

    // call it
    return item->call(args);	
}

//------------------------------------------------------------------------------
//
void LocalEndpoint::add_remote(RemoteEndpoint* a_remote)
{
	m_remotes.push_back(a_remote);
}

//------------------------------------------------------------------------------
//
void LocalEndpoint::clear_remotes()
{
    for (auto it = m_remotes.begin(); it != m_remotes.end(); it++) {
        delete *it;
    }
    m_remotes.clear();
}


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
