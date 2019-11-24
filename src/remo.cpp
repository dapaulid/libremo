//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "remo.h"

#include "logger.h"

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

//! logger instance
static Logger logger("controller");


//------------------------------------------------------------------------------	
// constants
//------------------------------------------------------------------------------	
//
//! maximum number of packets available
//! NOTE: this was selected as an arbitrary value and probably depends
//!       on the "concurrencyness" of the system (e.g. max. number of threads)
const size_t PACKET_POOL_SIZE = 16;


//------------------------------------------------------------------------------	
// class Controller
//------------------------------------------------------------------------------	
//
Controller::Controller():
    m_items(),
    m_packet_pool()
{
    // pre-allocate packets
    for (size_t i = 0; i < PACKET_POOL_SIZE; i++) {
        m_packet_pool.add(new Packet());
    }

}

//------------------------------------------------------------------------------	
//
Controller::~Controller()
{
    clear_items();
}

//------------------------------------------------------------------------------	
//
void Controller::register_item(Item* a_item)
{
    // item exists?
    if (!a_item) {
        // no -> nothing to do
        return;
    }

    // already registered at some controller?
    if (a_item->get_ctrl()) {
        // yes -> not allowed
        throw error(ErrorCode::ERR_ITEM_ALREADY_REGISTERED, "%s is already registered: '%s'",
            a_item->item_type(), a_item->get_full_name().c_str());
    } // end if

    // try to insert item
    auto ret = m_items.insert(std::pair<std::string, Item*>(a_item->get_full_name(), a_item));
    // inserted?
    if (ret.second) {
        // yes -> remember us
        a_item->set_ctrl(this);
    } else {
        // no -> item with same name already existing
        throw error(ErrorCode::ERR_ITEM_ALREADY_EXISTING, "%s with same name already exists: '%s'",
            ret.first->second->item_type(), ret.first->first.c_str());
    } // end if
}

//------------------------------------------------------------------------------	
//
void Controller::unregister_item(Item* a_item)
{
    // item exists?
    if (!a_item) {
        // no -> nothing to do
        return;
    }

    // try to remove item
    size_t count = m_items.erase(a_item->get_full_name());
    // removed?
    if (count == 1) {
        // yes -> forget us
        a_item->set_ctrl(nullptr);
    } else {
        // no -> item not found
        throw error(ErrorCode::ERR_ITEM_NOT_FOUND, "%s not found for unregistration: '%s'",
            a_item->item_type(), a_item->get_full_name().c_str());
    }
}

//------------------------------------------------------------------------------	
//
void Controller::clear_items()
{
    for (auto it = m_items.begin(); it != m_items.end(); it++) {
        it->second->set_ctrl(nullptr);
        delete it->second;
    }
    m_items.clear();
}

//------------------------------------------------------------------------------	
//
Item* Controller::find_item(const std::string& a_full_name)
{
    auto it = m_items.find(a_full_name);
    return it != m_items.end() ? it->second : nullptr;
}

//------------------------------------------------------------------------------	
//
void Controller::send_packet(Packet* a_packet)
{
    logger.info(">> %s", a_packet->to_string().c_str());

    // "loopback"
    handle_packet(a_packet);
}

//------------------------------------------------------------------------------	
//
void Controller::receive_packet(Packet* a_packet)
{
    logger.info("<< %s", a_packet->to_string().c_str());
}

//------------------------------------------------------------------------------	
//
void Controller::handle_packet(Packet* a_packet)
{
    if (a_packet->get_size() == 0) {
        logger.warning("ignoring packet of size 0");
        return;
    }

    uint8_t type = a_packet->get_byte(0);
    switch (type) {
    case PacketType::packet_call:
        handle_call(a_packet);
        break;
    case PacketType::packet_result:
        handle_result(a_packet);
        break;
    default:
        logger.warning("ignoring packet of unknown type 0x%02X", type);
    }
}

//------------------------------------------------------------------------------	
//
void Controller::handle_call(Packet* a_packet)
{
    BinaryReader reader(*a_packet);
    reader.read_call();

    std::string function_name = reader.get_function();
    
    // get function item
    Item* item = find_item(function_name);
    if (!item) {
        throw error(ErrorCode::ERR_RPC_NOT_FOUND, "remote procedure not found: '%s'",
            function_name.c_str());
    }

    // call it
    TypedValue result = item->call(reader.get_args());

    Packet* reply = take_packet();
    BinaryWriter reply_writer(*reply);

    ArgList args = reader.get_args(); // TODO const? separate variable;
    reply_writer.write_result(result, args);

    send_packet(reply);
    
}

//------------------------------------------------------------------------------	
//
void Controller::handle_result(Packet* a_packet)
{
    // TODO use some data structure
    m_received_result = a_packet;
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
