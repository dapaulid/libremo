//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "remote_endpoint.h"

#include "../l1_transport/reader.h"
#include "../l1_transport/writer.h"


//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// template implementation
//------------------------------------------------------------------------------
//
template<typename... Args>
TypedValue RemoteEndpoint::call(const std::string& a_function, Args... args)
{
    packet_ptr packet = take_packet();
    trans::BinaryWriter writer(packet->get_payload());
    writer.write_call(a_function, args...);

    send_packet(packet);
    //receive_packet(&packet);

    packet_ptr reply = std::move(m_received_result);

    trans::BinaryReader reader(reply->get_payload());
    return reader.read_result(args...);
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
