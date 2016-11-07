/**
 * @file      CanSocket.cpp
 * @author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief     SocketCAN implementation in C++
 * @details   These are the methods of class CanSocket to
 *            initialize the CAN interface, send and receive data via Linux
 *            SocketCAN.
 * @version   2.0
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
# include "Debug.h"
# include "CanSocket.h"

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

////////////////////////////////////////////////////////////////////////////////
CanSocket::CanSocket(const char* interface_str) noexcept :
        Socket(SocketType::CAN),
        m_can_init(FALSE)
{
    AR::boolean sock_created = is_socket_initialized();

    if ( interface_str != NULL_PTR && sock_created == TRUE )
    {
        AR::boolean interface_exists = check_interface(interface_str);

        // socket creation successful and interface exists
        if ( interface_exists == TRUE )
        {
            AR::boolean bind_success = bind_if_socket();

            if ( bind_success == TRUE )
            {
                m_can_init = TRUE;
            }
            else
            {
                m_can_init = FALSE;
            }
        }
        else
        {
            m_can_init = FALSE;
        }
    }
    else
    {
        m_can_init = FALSE;
    }
}

////////////////////////////////////////////////////////////////////////////////
CanSocket::~CanSocket() noexcept
{
    // do not close the socket here, this is done by the base class Socket
}

////////////////////////////////////////////////////////////////////////////////
AR::sint8 CanSocket::send(const AR::uint16 can_id, const CanDataType& data_ref,
                      const AR::uint8 len) noexcept
{
    AR::sint8 data_sent = 0;

    // First check if the file descriptor for the socket was initialized
    // and the interface is up and running.
    if ( (is_can_initialized() == TRUE) && (len > 0U) )
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
        for ( AR::uint8 i = 0U; i < len; ++i )
        {
            // copy byte by byte...
            // this could also be done by memcpy.
            frame.data[i] = data_ref[i];
        }

        const auto handle = get_socket_handle();

        // We need to transmit the size of struct can_frame with a POSIX write.
        const auto send_res = write(handle, &frame, m_can_mtu);
        data_sent = static_cast< AR::sint8 >(send_res);

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

////////////////////////////////////////////////////////////////////////////////
AR::sint8 CanSocket::receive(AR::uint16& can_id, CanDataType& data_ref) noexcept
{
    // complete length of the CAN frame
    AR::sint8 can_received = -1;

    if ( is_can_initialized() == TRUE )
    {
        struct can_frame frame;
        const auto handle = get_socket_handle();
        ssize_t nbytes = read(handle, &frame, m_can_mtu);
        can_received = static_cast< AR::sint8 >(nbytes);

        // check if data is on the socket to receive
        if ( can_received > 0 )
        {
            // The id is stored in the var given by ref.
            can_id = frame.can_id;

            // Give the data length code to the return val.
            can_received = static_cast< AR::sint8 >(frame.can_dlc);

            // Transfer the byte stream into the data structure given by ref.
            for ( AR::uint8 i = 0U; i < frame.can_dlc; ++i )
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
AR::sint8 CanSocket::receive(AR::uint16& can_id, CanDataType& data_ref,
                         const AR::uint16 timeout_us) noexcept
{
    // complete length of the CAN frame
    AR::sint8 can_received = -1;

    if ( is_can_initialized() == TRUE )
    {
        // before we go in a blocking read, we will check if there is
        // activity on the socket.
        AR::boolean event = poll_activity(timeout_us);

        // check the return value of the select. if there is data to read the
        // return value of select will be greater than zero.
        if ( event > 0 )
        {
            struct can_frame frame;
            const auto handle = get_socket_handle();
            const auto nbytes = read(handle, &frame, m_can_mtu);
            can_received = static_cast< AR::sint8 >(nbytes);

            // check if the read from the socket was successful
            if ( can_received > 0 )
            {
                // The id is stored in the var given by ref.
                can_id = frame.can_id;

                // Set the DLC of the CAN frame as return.
                can_received = (AR::sint8) frame.can_dlc;

                // Transfer the byte stream into the data structure given by ref.
                for ( AR::uint8 i = 0U; i < frame.can_dlc; ++i )
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

////////////////////////////////////////////////////////////////////////////////
AR::boolean CanSocket::create() noexcept
{
    AR::boolean socket_created = FALSE;
    SocketHandleType& handle = get_socket_handle();
    handle = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    // check here if the socket was opened.
    if ( handle < 0 )
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
AR::boolean CanSocket::check_interface(const char* interface_str) noexcept
{
    AR::boolean exists = FALSE;

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
    const int bind_res = bind(handle, (struct sockaddr *) &m_sockaddr,
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

////////////////////////////////////////////////////////////////////////////////
AR::boolean CanSocket::is_can_initialized() const noexcept
{
    return m_can_init;
}

#else
# error "SocketCAN for Linux OS only."
#endif /* WIN32 */
