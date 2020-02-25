add_library(remo "")

target_sources(
    remo
    PRIVATE
        l3_rpc/endpoint.cpp
        l3_rpc/local_endpoint.cpp
        l3_rpc/remote_endpoint.cpp
        l3_rpc/item.cpp
        l3_rpc/function.cpp
        l1_transport/transport.cpp
        l1_transport/channel.cpp
        l1_transport/packet.cpp
        l1_transport/reader.cpp
        l1_transport/writer.cpp
        l1_transport/tcp/tcp-transport.cpp
        l1_transport/tcp/tcp-channel.cpp
        l0_system/error.cpp
        l0_system/socket.cpp
        l0_system/system.cpp
        l0_system/types.cpp
        l0_system/worker.cpp
        utils/async.cpp
        utils/colors.cpp
        utils/logger.cpp
        utils/settings.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/remo.h
    )

target_include_directories(
    remo
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    )
