add_library(remo "")

if(MSVC)
  target_compile_options(remo PRIVATE /W4 /WX)
else()
  target_compile_options(remo PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

target_sources(
    remo
    PRIVATE
        remo.cpp
        types.cpp
        packet.cpp
        reader.cpp
        writer.cpp
        function.cpp
        error.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/remo.h
    )

target_include_directories(
    remo
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    )
