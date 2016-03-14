cmake_minimum_required(VERSION 2.8)

project(embedded-comstack)

set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS} -g3 -O0 -DDEBUG")

include_directories(src src/communication src/system test)
include_directories(examples/URSensorhead)
include_directories(examples/URSensorheadSim)

set(SOURCES
    src/communication/CanSocket.cpp
    src/communication/TcpSocket.cpp
    src/communication/TcpClient.cpp
    src/communication/TcpServer.cpp
    src/communication/IpAddress.cpp
)

set(TEST_SOURCES
    test/Packet_Test.cpp
    test/CanSocket_Test.cpp
    test/Tcp_Test.cpp
)

add_executable(UnitTest ${TEST_SOURCES} ${SOURCES})
target_link_libraries(UnitTest rt pthread)

add_executable(URSensorhead ${SOURCES} ${UR_SOURCES})
target_link_libraries(URSensorhead rt pthread)

add_executable(URSensorheadSim ${SOURCES} ${UR_SIM_SOURCES})
target_link_libraries(URSensorheadSim rt pthread)

enable_testing()
add_test(NAME RunTests COMMAND UnitTest)
