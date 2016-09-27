# What is embedded-comstack?
`embedded-comstack` is a tiny cross-platform communication and operating system abstraction library. The basic software library is supported under the following operating systems:

* Linux 32-bit/64-bit and ROS (all features)
* Windows 32-bit/64-bit (no SocketCAN support)

Please note that some system specific features such as SocketCAN are available under Linux only. The library was tested unter the following architectures:

OS | Device | CPU | Architecture | Compiler
---|--------|-----------|--------------|----------
Ubuntu Linux 14.04 64-Bit | PC | Intel i7-4600U | | g++ 4.9.3
Windows 7 64-Bit | PC | Intel i7-4600U | | g++ 4.9.3
Linux | Raspberry PI 2 Model B | Broadcom BCM2836 QUAD | ARM Cortex-A8 | arm-g++ 4.9.3
Linux | phyFLEX-i.MX6 | Freescale i.MX6 QUAD | ARM Cortex-A9 | arm-g++ 4.9.3

Currently, the following transport layer interfaces are available:

* Ethernet TCP/IP client
* Ethernet TCP/IP server
* SocketCAN (Linux and ROS only!)

# Credits:
This abstraction of network communication is adapted by the extremely powerful [SFML Packet](http://www.sfml-dev.org/tutorials/2.3/network-packet.php#problems-that-need-to-be-solved) implementation, but focusing on embedded applications where we do not want dynamic memory allocation.
