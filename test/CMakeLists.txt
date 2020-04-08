add_executable(
    unit_tests
    integration.test.cpp
    failure.test.cpp
    l0_system/logger.test.cpp
    l0_system/socket.test.cpp
    l1_transport/url.test.cpp
    l1_transport/transport.test.cpp
    utils/list.test.cpp
    utils/timer.test.cpp
)

target_link_libraries(
    unit_tests
    gtest_main
    remo
    )

add_test(
  NAME
    unit
  COMMAND
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests
  )
