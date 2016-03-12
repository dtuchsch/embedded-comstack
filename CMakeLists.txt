cmake_minimum_required(VERSION 2.8)

project(embedded-comstack)

set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS} -g3 -O0")

include_directories(src src/communication src/system test)

set(SOURCES
    src/communication/CanSocket.cpp
    src/communication/TcpSocket.cpp
    src/communication/TcpClient.cpp
    src/communication/TcpServer.cpp
    src/communication/IpAddress.cpp
#    examples/RControl.cpp
#    examples/RMonitor.cpp
)

set(TEST_SOURCES
    test/Packet_Test.cpp
    test/CanSocket_Test.cpp
    test/Tcp_Test.cpp
#    test/RControl_Test.cpp
)

add_executable(UnitTest ${TEST_SOURCES} ${SOURCES})
target_link_libraries(UnitTest rt pthread)

enable_testing()
add_test(NAME RunTests COMMAND UnitTest)
