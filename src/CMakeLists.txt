add_library(remo "")

target_sources(
    remo
    PRIVATE
        remo.cpp
        endpoint.cpp
        local_endpoint.cpp
        remote_endpoint.cpp
        types.cpp
        packet.cpp
        reader.cpp
        writer.cpp
        item.cpp
        function.cpp
        error.cpp
        logger.cpp
        system.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/remo.h
    )

target_include_directories(
    remo
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    )
