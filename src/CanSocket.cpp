/**
 * @file      Can.cpp
 * @author    dtuchscherer <your.email@hs-heilbronn.de>
 * @brief     short description...
 * @details   long description...
 * @version   1.0
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
#include "Debug.h"
#include "CanSocket.h"

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

/*******************************************************************************
 * PROTOTYPES OF LOCAL FUNCTIONS
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * GLOBAL MODULE VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

/*******************************************************************************
 * FUNCTION DEFINITIONS
 *******************************************************************************/

CanSocket::CanSocket(const char* interface_str) noexcept :
Socket(SocketType::CAN),
m_init(FALSE)
{
    if ( interface_str != NULL_PTR )
    {
        boolean sock_created = create();
        boolean interface_exists = check_interface(interface_str);

        // socket creation successful and interface exists
        if ( interface_exists && sock_created )
        {
            boolean bind_success = bind_if_socket();

            if ( bind_success )
            {
                m_init = TRUE;
            }
            else
            {
                m_init = FALSE;
            }
        }
        else
        {
            m_init = FALSE;
        }
    }
    else
    {
        m_init = FALSE;
    }
}

CanSocket::~CanSocket() noexcept
{
    ::close(get_socket_handle());
}

boolean CanSocket::is_initialized() noexcept
{
    return m_init;
}

sint8 CanSocket::send(const uint16 can_id, const CanDataType& data_ref,
                      const uint8 len) noexcept
{
    sint8 data_sent = 0;

    // First check if the file descriptor for the socket was initialized
    // and the interface is up and running.
    if ( (is_initialized() == TRUE) && (len > 0U) )
    {
        // Structure that is given to the POSIX write function.
        // We have to define CAN-ID, length (DLC) and the data to send.
        struct can_frame frame;

        // clear all to make sure we don't send rubbish.
        std::memset(&frame, 0, sizeof(frame));

        // CAN-ID
        frame.can_id = can_id;

        // length of data is the DLC
        frame.can_dlc = len;

        // check if length is possible for one CAN frame...
        // one CAN frame may only contain 8 data bytes.
        if ( len < 8U )
        {
            frame.can_dlc = len;
        }
        else
        {
            // limit the length to max DLC of CAN
            frame.can_dlc = 8U;
        }

        // copy data into the data field of the frame struct.
        for ( uint8 i = 0U; i < len; ++i )
        {
            // copy byte by byte...
            // this could also be done by memcpy.
            frame.data[i] = data_ref[i];
        }

        // We need to transmit the size of struct can_frame with a POSIX write.
        const auto send_res = write(get_socket_handle(), &frame, m_can_mtu);
        data_sent = static_cast< sint8 >(send_res);

        // Check if the desired length was transmitted over the socket.
        // A maximum of 8 bytes can be transmitted within a CAN message
        // without using IsoTp.
        if ( data_sent <= 0 )
        {
            DEBUG_ERROR("Send failed: %d", errno);
            m_last_error = errno;
            data_sent = -1;
        }
    }
    else
    {
        // the CAN interface is not initialized correctly.
        data_sent = -1;
    }

    return data_sent;
}

sint8 CanSocket::receive(uint16& can_id, CanDataType& data_ref) noexcept
{
    // complete length of the CAN frame
    sint8 can_received = -1;

    if ( is_initialized() == TRUE )
    {
        struct can_frame frame;
        ssize_t nbytes = read(get_socket_handle(), &frame, m_can_mtu);
        can_received = static_cast< sint8 >(nbytes);

        // check if data is on the socket to receive
        if ( can_received > 0 )
        {
            // The id is stored in the var given by ref.
            can_id = frame.can_id;

            // Give the data length code to the return val.
            can_received = static_cast< sint8 >(frame.can_dlc);

            // Transfer the byte stream into the data structure given by ref.
            for ( uint8 i = 0U; i < frame.can_dlc; ++i )
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

sint8 CanSocket::receive(uint16& can_id, CanDataType& data_ref,
                         const uint32 timeout) noexcept
{
    // complete length of the CAN frame
    sint8 can_received = -1;

    if ( is_initialized() == TRUE )
    {
        // we need a file descriptor for polling the socket if there is data to
        // read.
        fd_set recfds;
        struct timeval timeout_struct;

        constexpr uint32 usec_per_sec = 1000000U; // 1s in microseconds

        timeout_struct.tv_sec = timeout / usec_per_sec;
        timeout_struct.tv_usec = timeout % usec_per_sec;

        FD_ZERO(&recfds);
        FD_SET(get_socket_handle(), &recfds);

        // poll if there is something on the socket.
        can_received = select(get_socket_handle() + 1, &recfds, NULL, NULL,
                              &timeout_struct);

        // check the return value of the select. if there is data to read the
        // return value of select will be greater than zero.
        if ( can_received > 0 )
        {
            struct can_frame frame;
            const auto nbytes = read(get_socket_handle(), &frame, m_can_mtu);
            can_received = static_cast< sint8 >(nbytes);

            // check if the read from the socket was successful
            if ( can_received > 0 )
            {
                // The id is stored in the var given by ref.
                can_id = frame.can_id;

                // Set the DLC of the CAN frame as return.
                can_received = (sint8) frame.can_dlc;

                // Transfer the byte stream into the data structure given by ref.
                for ( uint8 i = 0U; i < frame.can_dlc; ++i )
                {
                    // Obviously copying data.
                    data_ref[i] = frame.data[i];
                }
            }
            else if ( can_received == 0 )
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

boolean CanSocket::create() noexcept
{
    bool socket_created = false;
    SocketHandleType& handle = get_socket_handle();
    handle = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    // check here if the socket was opened.
    if ( get_socket_handle() < 0 )
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

boolean CanSocket::check_interface(const char* interface_str) noexcept
{
    boolean exists = FALSE;

    // copying the interface string into the structure.
    std::strncpy(m_ifr.ifr_name, interface_str, IFNAMSIZ - 1);

    // make sure that the string is terminated.
    m_ifr.ifr_name[IFNAMSIZ - 1] = '\0';

    // map the interface string to a given interface of the kernel.
    m_ifr.ifr_ifindex = if_nametoindex(m_ifr.ifr_name);

    // check if the CAN interface exists in the system
    if ( m_ifr.ifr_ifindex != 0 )
    {
        // the CAN interface is known to the system
        exists = TRUE;
    }
    else
    {
        // ... error finding the given interface.
        exists = FALSE;
    }

    return exists;
}

boolean CanSocket::bind_if_socket() noexcept
{
    boolean bind_success = FALSE;

    // Define the address family.
    m_sockaddr.can_family = AF_CAN;

    // Bind the interface number to the socket.
    m_sockaddr.can_ifindex = m_ifr.ifr_ifindex;

    // Bind the socket onto the can interface.
    int bind_res = bind(get_socket_handle(), (struct sockaddr *) &m_sockaddr,
                        sizeof(m_sockaddr));

    // Check if binding the socket to the interface was successful.
    // It's negative if there was an error.
    if ( bind_res < 0 )
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
