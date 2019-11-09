add_executable(
    unit_tests
    serialization.test.cpp
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
