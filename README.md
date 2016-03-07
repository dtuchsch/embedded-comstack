# What is embedded-comstack?
embedded-comstack is a small communication stack written in modern C++. 
As it says it shall be suitable for embedded devices like microcontrollers.

# Network Packets?
In many projects i found myself encode, then send, again receive and decode bytestreams the old C-way.
This includes casting to bytes, words, floats and swapping these bytes between different architectures in specific functions. 

We can do better by abstracting this communication mechanism independent of big and little endian architectures. The result is a uniform Packet class that is generic, tested and may be used in a big amount of projects where we have to deal with exchanging data via a network or a fieldbus.

Some credits:
This abstraction of network communication is adapted by the extremely powerful [SFML Packet](http://www.sfml-dev.org/tutorials/2.3/network-packet.php#problems-that-need-to-be-solved) implementation, but focusing on embedded applications where we do not want dynamic memory allocation.

## How to use Packet

```c++
    Packet< 2 > packet;
    sint16 number = -23000;
    packet << number;
```
