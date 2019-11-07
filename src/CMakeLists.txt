add_library(remo "")

target_sources(
    remo
    PRIVATE
        remo.cpp
        types.cpp
        packet.cpp
        reader.cpp
        writer.cpp
        error.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/remo.h
    )

target_include_directories(
    remo
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    )
