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

#include "reader.h"
#include "writer.h"


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
    BinaryWriter writer(*packet);
    writer.write_call(a_function, args...);

    send_packet(packet);
    //receive_packet(&packet);

    packet_ptr reply = std::move(m_received_result);

    BinaryReader reader(*reply);
    return reader.read_result(args...);
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
