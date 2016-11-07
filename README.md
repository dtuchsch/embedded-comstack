# What is embedded-comstack?
`embedded-comstack` is a lightweight cross-platform communication and operating system library. Its purpose is to provide a reusable middleware. It is programmed in Modern C++ (C++11 / C++14) and provides some easy-to-use features for developing embedded software. To use `embedded-comstack` a recent C++14 compiler such as gcc/g++ or clang is necessary.

## Motivation

Socket programming in C to commmunicate over Ethernet TCP/IP, UDP or even CAN under Linux is possible, but in my personal opinion limited in reusability, maintainability and as a consequence not very elegant. I often found myself in doing repetitive tasks for programming embedded communication software in C. The same applies to working with threads (pthreads) and real-time schedulers of Linux. We can do better... 

For me it's important to have a library to wrap this not-so-elegant solutions, so I can reuse a well-defined interface for the majority of my applications without rewriting code I've implemented multiple times. So the goal of `embedded-comstack` is to have a general library that is easily applied. This library shall be reusable, maintainable, and easier-to-use.

As an example imagine you want to use a socket to exchange some data between a TCP server and a TCP client. You want to send data from the TCP client. A C-implementation would consist of a lot of initialization steps you have to program. Imagine you want to add a second TCP client sending some data to another TCP server. In the best case, you will create some functions to create sockets and send data. The following code snippet shows only a part of a C-implementation to establish a connection to a TCP server:

```c
struct sockaddr_in server;
unsigned long addr;
memset( &server, 0, sizeof (server));
// ...
memcpy((char *)&server.sin_addr, &addr, sizeof(addr));
server.sin_family = AF_INET;
server.sin_port = htons(5555);
// and so on...
```

The solution could be much better...

In reality, we only want to open a socket and send some data to the TCP server. At this point `embedded-comstack` comes in handy:

```cpp
TcpClient client;
const auto connected = client.connect("127.0.0.1", 5555U);
const auto sent = client.send(&data, data.size());
```

We are now able to create multiple clients or servers easily. In addition, all of the protocols using sockets do have the most implementation part in common. The type `TcpClient` is derived from type `Socket`, which implements sockets in the most general possible way.

## Operating systems supported

The basic software library is supported under the following operating systems:

* Linux 32-bit/64-bit and ROS (all features)
* Windows 32-bit/64-bit (no SocketCAN and real-time threading support)

Please note that some system specific features such as SocketCAN are available under Linux only. The library may be used under Microsoft Windows, but is targeted at Embedded Linux applications.

The following list shows the systems and architectures under which the library has been used and tested successfully:

OS | Device | CPU | Architecture | Compiler
---|--------|-----------|--------------|----------
Ubuntu Linux 14.04 64-Bit | Dell Latitude E7440 | Intel(R) i7-4600U | | g++ 4.9.3 / clang 3.8
Windows 7 64-Bit | Dell Latitude E7440 | Intel(R) i7-4600U | | g++ 4.9.3
Embedded Linux with PREEMPT_RT | Raspberry PI 2 Model B | Broadcom BCM2836 QUAD | ARM Cortex-A8 | arm-g++ 4.9.3
Embedded Linux with PREEMPT_RT | phyFLEX-i.MX6 | Freescale i.MX6 QUAD | ARM Cortex-A9 | arm-g++ 4.9.3

Currently, the following transport layer interfaces are available:

* Ethernet TCP/IP client
* Ethernet TCP/IP server
* SocketCAN (Linux and ROS only!)

# Usage

## Ubuntu Linux and ROS

### Prerequisites

The source code of the basic software library is located in the folder `bsw` as a ROS package. So, if your operating system is Linux you will need Robot Operating System (ROS) to use the library. However, the package is also usable under Microsoft Windows, because ROS packages and the catkin build system are based on CMake. All the catkin and ROS related features are commented out for Windows builds.

The use of `embedded-comstack` in Linux und ROS is recommended. You need at least need the following if you're using Linux:

* Recent C++ (cross-)compiler with C++11/C++14 support (gcc or clang)
* ROS framework (http://wiki.ros.org/ROS/Installation)
* An initialized ROS catkin workspace (http://wiki.ros.org/catkin/Tutorials/create_a_workspace)

### Install

First, get a local work copy of this Git repository by cloning it into the `src` folder of your catkin workspace:

```shell
git clone https://github.com/dtuchsch/embedded-comstack.git
```

Then, build the catkin workspace from the root directory.

```
cd catkin_ws
catkin_make
```

This command builds the basic software library as a library which can be linked against your ROS package that holds the application.

### Using bsw in your applications

All you have to do is to create a new ROS catkin package and add the `bsw` package as a dependency:

```shell
cd catkin_ws
catkin_create_pkg my_app std_msgs roscpp bsw
```

Add the following line to your `CMakeLists.txt` of your application to edit the compiler flags and add C++14 support:

```
set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")
```

This compiler switch is mandatory, otherwise on compile-time it will throw hundreds of compiler errors, only because you forgot to turn on the C++14 standard.

Program your application and add the cpp file as the executable to your `CMakeLists.txt`:

```
add_executable(my_app src/my_app.cpp)
```

You must link against the `bsw` library:

```
target_link_libraries(my_app
   ${catkin_LIBRARIES}
)
```

Linking against the bsw library is mandatory. Otherwise the linker will throw errors because it does not find the method definitions necessary to bake together the executable for your appliation.

### Examples

There is a ROS package that contains some example applications on how to use the basic software library. I will add more examples as time passes by. If you're working on Ubuntu Linux with ROS installed you can try out the examples by copying the ROS package `examples_bsw` into your catkin workspace. After copying, build your catkin workspace and source the setup.bash unless you have done it already.

```
catkin_make
source devel/setup.bash
```

Running an example such as the TCP server/client application is done by `rosrun` and calling the desired ROS node of the ROS package `examples_bsw`:

```shell
rosrun examples_bsw tcp_ping_pong
```

## Windows

# Credits
The abstraction of network communication is adapted by the extremely powerful [SFML Packet](http://www.sfml-dev.org/tutorials/2.3/network-packet.php#problems-that-need-to-be-solved) implementation for Windows and Linux. The implementation is more focused on a lightweight footprint, which is also easily applicable for embedded devices.
