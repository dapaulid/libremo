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

#include "packet.h"
#include "reader.h"
#include "writer.h"
#include "item.h"

#include <string>
#include <unordered_map>


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


class Controller {
public:
	Controller();
	virtual ~Controller();

	template<typename Ret, typename... Args>
	Ret call(const std::string& a_function, Args... args)
	{
        Packet packet;
        remo::BinaryWriter writer(packet);
        writer.write_call(a_function, args...);

		send_packet(&packet);
		receive_packet(&packet);

		remo::BinaryReader reader(packet);
        return reader.read_result<Ret>(args...);
	}

protected:
	friend class Item;
	void register_item(Item* a_item);
	void unregister_item(Item* a_item);
	Item* find_item(const std::string& a_full_name);

protected:
	void send_packet(Packet* a_packet);
	void receive_packet(Packet* a_packet);

	void handle_packet(Packet* a_packet);
	void handle_call(Packet* a_packet);
	void handle_result(Packet* a_packet);

	Packet* take_packet() {
		Packet* packet = m_packet_pool.take();
		if (!packet) {
			throw error(ErrorCode::ERR_OUT_OF_PACKETS, 
				"out of packets. maybe some transport plugin leaking?");
		}
		return packet;
	}

private:
	//! container for looking up items by full name
	// TODO use smart pointer?
	std::unordered_map<std::string, Item*> m_items;
	//! packet pool to avoid heap allocations
	RecyclingPool<Packet> m_packet_pool;	
};

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
