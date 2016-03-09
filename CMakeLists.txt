cmake_minimum_required(VERSION 2.8)

project(embedded-comstack)

set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")

include_directories(src test)

set(SOURCES
    src/CanSocket.cpp
)

set(TEST_SOURCES
    test/Packet_Test.cpp
    test/CanSocket_Test.cpp
)

add_executable(UnitTest ${TEST_SOURCES} ${SOURCES})

enable_testing()
add_test(NAME RunTests COMMAND UnitTest)
