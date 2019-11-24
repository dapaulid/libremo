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
#include "packet.h"

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------	
//
class LocalEndpoint;


//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------	
//
class RemoteEndpoint: public Endpoint {
public:
	RemoteEndpoint(LocalEndpoint* a_local);
	virtual ~RemoteEndpoint();

	template<typename Ret, typename... Args>
	Ret call(const std::string& a_function, Args... args);

protected:
	Packet* take_packet();

	void send_packet(Packet* a_packet);
	void receive_packet(Packet* a_packet);

	void handle_packet(Packet* a_packet);
	void handle_call(Packet* a_packet);
	void handle_result(Packet* a_packet);

private:
	void alloc_packets();

private:
	//! the local endpoint that this endpoint represents to the outside
	LocalEndpoint* m_local;
	//! packet pool to avoid heap allocations
	RecyclingPool<Packet> m_packet_pool;
	//! TODO use some data structure
	Packet* m_received_result = nullptr;

};


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------

// template implementation
#include "remote_endpoint.tpp.h"
