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

#include "reader.h"
#include "writer.h"
#include "packet.h"
#include "recycling.h"

#include <string>


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
	RecyclingPool<Packet> m_packet_pool;
};

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
