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
template<typename Ret, typename... Args>
Ret RemoteEndpoint::call(const std::string& a_function, Args... args)
{
    Packet* packet = take_packet();
    BinaryWriter writer(*packet);
    writer.write_call(a_function, args...);

    send_packet(packet);
    packet->recycle();
    //receive_packet(&packet);

    Packet* reply = m_received_result;

    BinaryReader reader(*reply);
    Ret result = reader.read_result<Ret>(args...);

    reply->recycle();

    return result;
}

//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
