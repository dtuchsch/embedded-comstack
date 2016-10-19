# Packet network communication model

The purpose of the packet model is to provide data network communication in the most general and universal way. It is designed for easy and fast communication in your embedded software project. In particular, it allows a more safe and reliable data transport by using a fixed size data container and compile-time checks when accessing the data packet. 

# Motivation

Communication between various components and platforms plays a central role in today's embedded software development. In almost every embedded software application for example some data from a sensor has to be read or data have to be transmitted from one embedded device to another. For the exchange established protocols and fieldbus systems are used. Some examples are I2C, Ethernet, CAN or SPI.

However, one main challenge occurs when communicating between different platforms which leads to a higher development effort. The so-called endianess (which is also known as the byte order of a platform) may differ between some system components. The platform's byte order describes in which order values are stored in memory. Some platforms may have so-called **Big-Endian** byte order - others have so-called **Little-Endian** byte order. If one platform with the **host-byte-order** Little-Endian receives a bytestream from the network in **network-byte-order**, which corresponds to Big-Endian, and copies the data directly into its memory, the interpretation of data would be incorrect. Copying it directly into memory would result in bytes that are in reverse order. Thus, swapping bytes is necessary to get the Little-Endian representation. As most of the embedded software applications are written in C, this results in byte fiddling between those two byte orders.

The packet communication model solves this problem to support the developer and avoid implementation errors. No more application-specific byte swapping. The serialization and de-serialisation is done by the packet model:

* automatic Big-Endian / Little-Endian conversion between different platforms
* automatic interpretation of the bytestream and conversion in C++ data types (floating points, integers and even user-specific data types)

The advantage is that the developer does not always have to care whether some of the platforms may differ in endianess. This is handled by the packet communication model and ensures the correct serialization and deserialization.

# Specification

All network communications do have at least the following in common: It's always a raw data bytestream transmitted over the physical network. This bytestream must be interpreted first to become an information which can be processed further. To have a general implementation some requirements are specified:

* Independent of the differing platforms in one system the bytestream must always be in network-byte-order (Big-Endian). As a consequence, this means all data that is sent out or received is in network-byte-order.
* The complete serialization and conversion from host-byte-order to network-byte-order and vice versa is done by the packet communication model.
* Each packet has a fixed maximum size.

# Example

1. First, define your frame layout
2. Create a packet
3. Send some data
