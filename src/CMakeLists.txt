add_library(remo "")

target_sources(
    remo
    PRIVATE
        l3_rpc/endpoint.cpp
        l3_rpc/local_endpoint.cpp
        l3_rpc/remote_endpoint.cpp
        l3_rpc/item.cpp
        l3_rpc/function.cpp
        l1_transport/packet.cpp
        l1_transport/reader.cpp
        l1_transport/writer.cpp
        l0_system/error.cpp
        l0_system/logger.cpp
        l0_system/socket.cpp
        l0_system/system.cpp
        l0_system/types.cpp        
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/remo.h
    )

target_include_directories(
    remo
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    )
