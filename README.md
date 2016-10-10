# What is embedded-comstack?
`embedded-comstack` is a tiny cross-platform / multi-platform communication and operating system library. It is programmed in Modern C++ and compresses some concepts used often when it comes to programming embedded software. A current C++14 compiler is necessary.

## Operating systems supported

The basic software library is supported under the following operating systems:

* Linux 32-bit/64-bit and ROS (all features)
* Windows 32-bit/64-bit (no SocketCAN and real-time threading support)

Please note that some system specific features such as SocketCAN are available under Linux only. The library may be used under Microsoft Windows, but is targeted at Embedded Linux applications.

The following list shows the systems and architectures under which the library has been used and tested successfully:

OS | Device | CPU | Architecture | Compiler
---|--------|-----------|--------------|----------
Ubuntu Linux 14.04 64-Bit | Dell Latitude E7440 | Intel(R) i7-4600U | | g++ 4.9.3
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

* Current C++ (cross-)compiler with C++11/C++14 support
* ROS (http://wiki.ros.org/ROS/Installation)
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
This abstraction of network communication is adapted by the extremely powerful [SFML Packet](http://www.sfml-dev.org/tutorials/2.3/network-packet.php#problems-that-need-to-be-solved) implementation, but focusing on embedded applications where we do not want dynamic memory allocation.
