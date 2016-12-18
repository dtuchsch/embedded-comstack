/**
 * @file      CanSocket.cpp
 * @author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief     SocketCAN implementation in C++
 * @details   These are the methods of class CanSocket to
 *            initialize the CAN interface, send and receive data via Linux
 *            SocketCAN.
 * @version   2.1
 * @copyright Copyright (c) 2015, dtuchscherer.
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

/*******************************************************************************
 * MODULES USED
 *******************************************************************************/
#ifndef _WIN32
#include "CanSocket.h"
#include "Debug.h"
#include <type_traits> // checks at compile-time

/*******************************************************************************
 * DEFINITIONS AND MACROS
 ******************************************************************************/

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 ******************************************************************************/

/*******************************************************************************
 * PROTOTYPES OF LOCAL FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 * EXPORTED VARIABLES
 ******************************************************************************/

/*******************************************************************************
 * GLOBAL MODULE VARIABLES
 ******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
AR::sint8 CanSocket::receive(CanIDType& can_id, CanFDData& data_ref) noexcept
{
    // complete length of the CAN frame
    AR::sint8 can_received{-1};

    if (is_can_initialized() == TRUE)
    {
        struct canfd_frame frame;
        const auto socket = get_socket_handle();
        ssize_t nbytes = read(socket, &frame, m_can_mtu);
        can_received = static_cast< AR::sint8 >(nbytes);

        // check if data is on the socket to receive
        if (can_received > 0)
        {
            // The id is stored in the var given by ref.
            can_id = frame.can_id;
            // Give the data length code to the return val.
            can_received = static_cast< AR::sint8 >(frame.len);
            // Transfer the byte stream into the data structure given by ref.
            for (AR::uint8 i = 0U; i < frame.len; ++i)
            {
                // Obviously copying data.
                data_ref[i] = frame.data[i];
            }
        }
        else
        {
            // Error or timeout.
            can_received = -1;
            m_last_error = errno;
            DEBUG_ERROR("Receive failed: %d", errno);
        }
    }

    return can_received;
}

////////////////////////////////////////////////////////////////////////////////
AR::sint8 CanSocket::receive(CanIDType& can_id, CanDataType& data_ref,
                             const AR::uint16 timeout_us) noexcept
{
    // complete length of the CAN frame
    AR::sint8 can_received{-1};

    if (is_can_initialized())
    {
        // before we go in a blocking read, we will check if there is
        // activity on the socket.
        AR::boolean event = poll_activity(timeout_us);

        // check the return value of the select. if there is data to read the
        // return value of select will be greater than zero.
        if (event > 0)
        {
            struct can_frame frame;
            const auto handle = get_socket_handle();
            const auto nbytes = read(handle, &frame, m_can_mtu);
            can_received = static_cast< AR::sint8 >(nbytes);

            // check if the read from the socket was successful
            if (can_received > 0)
            {
                // The id is stored in the var given by ref.
                can_id = frame.can_id;

                // Set the DLC of the CAN frame as return.
                can_received = (AR::sint8)frame.can_dlc;

                // Transfer the byte stream into the data structure given by
                // ref.
                for (AR::uint8 i = 0U; i < frame.can_dlc; ++i)
                {
                    // Obviously copying data.
                    data_ref[i] = frame.data[i];
                }
            }
            else if (can_received == 0)
            {
                // timeout on zero return
                DEBUG_INFO("Time out reached");
                m_last_error = errno;
                can_received = 0;
            }
            else
            {
                DEBUG_ERROR("Receive failed: %d", errno);
                m_last_error = errno;
                can_received = -1;
            }
        }
    }

    return can_received;
}

////////////////////////////////////////////////////////////////////////////////
AR::boolean CanSocket::create() noexcept
{
    AR::boolean socket_created = FALSE;
    SocketHandleType& handle = get_socket_handle();
    handle = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    // check here if the socket was opened.
    if (handle < 0)
    {
        // ... error opening the socket.
        socket_created = false;
    }
    else
    {
        // ... successfull opened.
        socket_created = true;
    }

    return socket_created;
}

////////////////////////////////////////////////////////////////////////////////
AR::boolean CanSocket::enable_canfd() noexcept
{
    AR::boolean enabled{false};
    const auto socket = get_socket_handle();
    // retrieve the mtu via ioctrl
    const int info = ioctl(socket, SIOCGIFMTU, &m_ifr);

    if (info >= 0)
    {
        // get the mtu of the CAN device configured
        const auto mtu = m_ifr.ifr_mtu;

        if (mtu == CANFD_MTU)
        {
            static constexpr int CANFD_FLAG{1};
            const auto option_set =
                setsockopt(socket, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &CANFD_FLAG,
                           sizeof(CANFD_FLAG));

            if (option_set)
            {
                enabled = false;
            }
            else
            {
                enabled = true;
            }
        }
    }

    return enabled;
}

////////////////////////////////////////////////////////////////////////////////
AR::boolean CanSocket::bind_if_socket() noexcept
{
    AR::boolean bind_success = FALSE;

    // Define the address family.
    m_sockaddr.can_family = AF_CAN;
    // Bind the interface number to the socket.
    m_sockaddr.can_ifindex = m_ifr.ifr_ifindex;
    const auto handle = get_socket_handle();
    // Bind the socket onto the can interface.
    const int bind_res =
        bind(handle, (struct sockaddr*)&m_sockaddr, sizeof(m_sockaddr));

    // Check if binding the socket to the interface was successful.
    // It's negative if there was an error.
    if (bind_res < 0)
    {
        // Error binding the socket onto the can interface.
        bind_success = FALSE;
        m_last_error = errno;
        DEBUG_ERROR("Bind of socket and interface failed.");
    }
    else
    {
        // Everything went fine.
        // Return true for other methods of this class.
        bind_success = TRUE;
    }

    return bind_success;
}

////////////////////////////////////////////////////////////////////////////////
AR::boolean CanSocket::is_can_initialized() const noexcept
{
    return m_can_init;
}

#else
#error "SocketCAN for Linux OS only."
#endif /* WIN32 */
