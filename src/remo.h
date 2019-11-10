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
#include "recycling.h"

#include <string>


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


class Controller {
public:
	Controller();
	virtual ~Controller();

	template<typename... Args>
	void call(const std::string a_function, Args... args)
	{
	}

protected:

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
