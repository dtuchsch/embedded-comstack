# What is embedded-comstack?
`embedded-comstack` is a tiny cross-platform communication and operating system abstraction library. It is programmed in Modern C++. To use it a current C++14 compiler is necessary. 

## Prerequisites

The source code of the basic software library is located in the folder `bsw` as a ROS package. So, if your operating system is Linux you will need Robot Operating System (ROS) to use the library. However, the package is also usable under Microsoft Windows, because ROS packages and the catkin build system are based on CMake. All the catkin and ROS related features are commented out for Windows builds.

* ROS (for Linux only)
* Current C++ compiler with C++14 support

## Operating systems supported

The basic software library is supported under the following operating systems:

* Linux 32-bit/64-bit and ROS (all features)
* Windows 32-bit/64-bit (no SocketCAN support)

Please note that some system specific features such as SocketCAN are available under Linux only. The following list shows the systems and architectures under which the library has been used and tested successfully:

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

# Credits:
This abstraction of network communication is adapted by the extremely powerful [SFML Packet](http://www.sfml-dev.org/tutorials/2.3/network-packet.php#problems-that-need-to-be-solved) implementation, but focusing on embedded applications where we do not want dynamic memory allocation.
