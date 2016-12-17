## CanSocket

Send and receive standard CAN frames and CAN FD frames.

### Objectives

Provide an easy-to-use, reusable and safe SocketCAN interface.

### Standard CAN Frames

Create an object of type `CanSocket` with your CAN interface. The parameter you have to specify is the name of your CAN device.

```c++
#include "CanSocket.h"

CanSocket can{"vcan0"};
```

The example above shows the creation . At construction this will automatically initialize the socket and set up the CAN interface for data transmission.

#### Sending CAN frames

Sending standard standard CAN frames and CAN FD frames is done by calling one template method `send()`. Dependent on its data type given as parameter the method will handle both types. There is one type for the standard CAN frame named `CanStdData` and type `CanFDData` for CAN FD frames.

```c++
CanSocket can{"vcan0"};
CanStdData data{1,2,3};
can.send(0x01U, data, 3U);
CanFDData fd{4,5,6,7,8,9,10,11,12,22};
can.send(0x02U, fd, 10U);
```

> This implementation provides maximum safety. All other types given as parameter will result in a compile-error. Only standard frames and CAN FD types and their statically known sizes are allowed. In addition, this implementation is elegant and easy-to-use, because everything is handled in one method. The user only needs to declare a standard CAN frame or a CAN FD frame.

> Please note: To send CAN FD frames you must call method `can.enable_canfd();`. Otherwise the frame is not sent.

> A standard CAN frame has 8 bytes of user data. A CAN FD frame has 64 bytes of user data.
