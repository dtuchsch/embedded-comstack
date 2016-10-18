# Packet network communication model

The purpose of the packet model is to provide data network communication in the most general and universal way. It is designed for easy and fast communication in your embedded software project. In particular, it allows a more safe and reliable data transport by using a fixed size data container and compile-time checks when accessing the data packet. 

# Motivation

In today's embedded software development communication between various components and platforms plays a central role. In almost every embedded software application some data from a sensor is needed or data has to be transmitted from one embedded device (microcontroller in this case) to another. Established protocols and fieldbus systems are used for exchanging data. Some examples are I2C, Ethernet, CAN or SPI.

However, with communicating between different platforms and device architectures in one system one main challenge occurs. The endianess, which is also known as the byte order, may differ between some system components / platfroms. The platform's byte order describes in which order values are stored in memory. Some platforms may have so-called **Big-Endian** byte order - some may have so-called **Little-Endian**. If one platform with the **host-byte-order** Little-Endian receives a bytestream from the network in **network-byte-order**, which corresponds to Big-Endian, and copies the data directly into its memory the interpretation of data would be incorrect. Copying it directly into memory would result into bytes that are in the wrong order. Thus, reversing bytes is necessary. As most of the embedded software applications are written in C this results in byte fiddling between those two byte orders.

The packet communication model solves this problem to support the developer and avoid implementation errors by doing byte reversing in the background:

* automatic Big-Endian / Little-Endian conversion between different platforms
* automatic interpretation of the bytestream and conversion in C++ data types (floating points, integers and even user-specific data types)

The advantage is that the developer does not always have to care whether some of the platforms may differ in endianess. This is handled by the packet communication model and ensures the correct data interpretation.

# Design

All network communication have at least the following in common: It's always a raw data bytestream traveling over the physical network. This bytestream must be interpreted first to become an information. To have a general implementation some requirements and constraints are specified.

* Independent of the differing platforms in one system the bytestream must always be in network-byte-order (Big-Endian). As a consequence, this means all data that is sent out or received is in network-byte-order.
* The conversion from host-byte-order to network-byte-order and vice versa is done by the packet communication model.
* Each packet has a fixed maximum size.

# Example

1. First, define your frame layout.
