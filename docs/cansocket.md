## CanSocket

### Standard CAN Frames

Create an object of type `CanSocket`. The parameter you have to specify is the name of your CAN device.

```c++
CanSocket can{"vcan0"};
```

The example above shows the creation . At construction this will automatically initialize the socket and set up the CAN interface for data transmission.

#### Sending CAN Frames

Sending CAN frames is done by calling the method `send()`:

```c++
can.send();
```

A standard CAN frame has 8 bytes of user data. If you pass more than the maximum length, the implementation will limit it.

### CAN FD support

It's also possible to use CAN FD instead of standard CAN by

```c++
CanSocket<CANFD> can{"can0"};
```

At instantiation the socket is created and the interface is configured for CAN FD data transmission.
