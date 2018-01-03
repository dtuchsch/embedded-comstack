/**
 * \file      CanSocket.cpp
 * \author    dtuchscherer <daniel.tuchscherer@gmail.com>
 * \brief     SocketCAN implementation in C++
 * \details   These are the methods of class CanSocket to
 *            initialize the CAN interface, send and receive data via Linux
 *            SocketCAN.
 * \version   2.1
 * \copyright Copyright (c) 2018, Daniel Tuchscherer.
 *            All rights reserved.
 *
 *            Redistributions and use in source and binary forms, with
 *            or without modifications, are permitted provided that the
 *            following conditions are met: Redistributions of source code must
 *            retain the above copyright notice, this list of conditions and the
 *            following disclaimer.
 *
 *            Redistributions in binary form must reproduce the above copyright
 *            notice, this list of conditions and the following disclaimer in
 *            the documentation and/or other materials provided with the
 *            distribution.
 *
 *            Neither the name of the Heilbronn University nor the name of its
 *            contributors may be used to endorse or promote products derived
 *            from this software without specific prior written permission.
 *
 *            THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS “AS IS”
 *            AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *            TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *            PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS
 *            OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *            SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *            LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *            USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *            AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *            LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *            ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *            POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef _WIN32
#error "SocketCAN for Linux OS only."
#endif
#include "CanSocket.h"
#include <type_traits> // Checks & decisions at compile-time

////////////////////////////////////////////////////////////////////////////////
std::int8_t CanSocket::receive(CanIDType& can_id, CanFDData& data_ref) noexcept
{
    // Complete length of the CAN frame received
    std::int8_t can_received{-1};

    if (is_can_initialized() == true)
    {
        struct canfd_frame frame;
        const auto socket = get_socket_handle();
        const ssize_t nbytes = read(socket, &frame, can_mtu_);
        can_received = static_cast< std::int8_t >(nbytes);

        // Check if data is on the socket to receive
        if (can_received > 0)
        {
            // The id is stored in the var given by ref.
            can_id = frame.can_id;
            // Give the data length code to the return val.
            can_received = static_cast< std::int8_t >(frame.len);
            // Limit the maximum length
            const auto data_len = std::min(
                static_cast< std::size_t >(frame.len), data_ref.size());
            // Transfer the byte stream into the data structure given by ref.
            for (std::uint8_t i = 0U; i < data_len; ++i)
            {
                data_ref[i] = frame.data[i];
            }
        }
        else
        {
            // Error or timeout.
            can_received = -1;
            last_error_ = errno;
            std::cerr << "Receive failed: " << errno << "\n";
        }
    }

    return can_received;
}

////////////////////////////////////////////////////////////////////////////////
template < typename Duration >
std::int8_t CanSocket::receive(CanIDType& can_id, CanFDData& data_ref,
                               const Duration&& deadline) noexcept
{
    // Complete length of the CAN frame received
    std::int8_t can_received{-1};

    if (is_can_initialized())
    {
        // before we go in a blocking read, we will check if there is
        // activity on the socket.
        bool event = wait_for(deadline);

        // check the return value of the select. if there is data to read the
        // return value of select will be greater than zero.
        if (event > 0)
        {
            struct can_frame frame;
            const auto handle = get_socket_handle();
            const auto nbytes = read(handle, &frame, can_mtu_);
            can_received = static_cast< std::int8_t >(nbytes);

            // check if the read from the socket was successful
            if (can_received > 0)
            {
                // The id is stored in the var given by ref.
                can_id = frame.can_id;
                // Set the DLC of the CAN frame as return.
                can_received = (std::int8_t)frame.can_dlc;

                // Transfer the byte stream into the data structure given by
                // ref.
                for (std::uint8_t i = 0U; i < frame.can_dlc; ++i)
                {
                    // Obviously copying data.
                    data_ref[i] = frame.data[i];
                }
            }
            else if (can_received == 0)
            {
                // Timeout on zero return
                std::cerr << "Timeout reached\n";
                last_error_ = errno;
                can_received = 0;
            }
            else
            {
                std::cerr << "Receive failed: " << errno << "\n";
                last_error_ = errno;
                can_received = -1;
            }
        }
    }

    return can_received;
}

////////////////////////////////////////////////////////////////////////////////
bool CanSocket::create() noexcept
{
    bool socket_created{false};
    socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    // check here if the socket was opened.
    if (get_socket_handle() > 0)
    {
        // ... successfully opened.
        socket_created = true;
    }
    else
    {
        // ... error opening the socket.
        socket_created = false;
    }

    return socket_created;
}

////////////////////////////////////////////////////////////////////////////////
bool CanSocket::enable_canfd() noexcept
{
    bool enabled{false};
    const auto socket = get_socket_handle();
    // retrieve the mtu via ioctrl
    const int info = ioctl(socket, SIOCGIFMTU, &ifr_);

    if (info >= 0)
    {
        // get the mtu of the CAN device configured
        const auto mtu = ifr_.ifr_mtu;

        if (mtu == CANFD_MTU)
        {
            static constexpr int CANFD_FLAG{1};
            const auto option_set =
                setsockopt(socket, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &CANFD_FLAG,
                           sizeof(CANFD_FLAG));

            if (option_set >= 0)
            {
                enabled = true;
            }
            else
            {
                last_error_ = errno;
                enabled = false;
            }
        }
    }

    return enabled;
}

////////////////////////////////////////////////////////////////////////////////
bool CanSocket::bind_if_socket() noexcept
{
    bool bind_success = false;
    // Define the address family.
    sockaddr_.can_family = AF_CAN;
    // Bind the interface number to the socket.
    sockaddr_.can_ifindex = ifr_.ifr_ifindex;
    const auto handle = get_socket_handle();
    // Bind the socket to the CAN interface.
    const int bind_res =
        bind(handle, (struct sockaddr*)&sockaddr_, sizeof(sockaddr_));
    // Check if binding the socket to the interface was successful.
    // It's negative if there was an error.
    if (bind_res < 0)
    {
        // Error binding the socket to the CAN interface.
        // store this as the last error other application layers may access.
        bind_success = false;
        last_error_ = errno;
        std::cerr << "Bind of socket and interface failed.\n";
    }
    else
    {
        // Everything went fine.
        // Return true for other methods of this class.
        bind_success = true;
    }

    return bind_success;
}

////////////////////////////////////////////////////////////////////////////////
bool CanSocket::is_can_initialized() const noexcept { return can_init_; }
