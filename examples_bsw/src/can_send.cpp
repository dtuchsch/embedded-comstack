/// \brief This example will send out standard and CAN FD frames.

#include "CanSocket.h"
#include <cassert>
#include <chrono>
#include <thread>

int main() noexcept
{
    CanSocket vcan0{"vcan0"};

    for (;;)
    {
        vcan0.send(8U, CanFDData{0x00U, 0x01U}, 2U);
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }

    return 0;
}
