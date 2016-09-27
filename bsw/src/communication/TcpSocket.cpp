/**
 * @file      TcpSocket.cpp
 * @author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief     Ethernet TCP/IP communication
 * @details
 * @version   1.0
 * @edit      27.09.2016
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
#include "TcpSocket.h"

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
TcpSocket::TcpSocket() noexcept :
		Socket(SocketType::TCP)
{
    // call the base class opening the socket.
}

////////////////////////////////////////////////////////////////////////////////
TcpSocket::~TcpSocket() noexcept
{
    // do not close the socket here,
    // this is done by the base class Socket
}

////////////////////////////////////////////////////////////////////////////////
AR::boolean TcpSocket::create() noexcept
{
    AR::boolean socket_created = FALSE;

    SocketHandleType& handle = get_socket_handle();
    handle = socket(AF_INET, SOCK_STREAM, 0);

    // check here if the socket was opened.
    if ( get_socket_handle() > 0 )
    {
        // ... successfull opened.
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
AR::sint16 TcpSocket::send(const void* message, const AR::uint16&& len) noexcept
{
    const AR::boolean socket_open = is_socket_initialized();
    AR::sint16 data_sent = -1;

    // sending only makes sense if at least the socket is open.
    if ( socket_open == TRUE )
    {        
#ifdef _WIN32
        const char* msg = static_cast< const char* >(message);
#elif defined(__unix__)
        const void* msg = message;
#endif
        const SocketHandleType& handle = get_socket_handle();
#ifdef _WIN32
        data_sent = ::send(handle, msg, len, 0);
#elif defined(__unix__)
        data_sent = ::send(handle, msg, len, MSG_NOSIGNAL);
#endif

        if ( data_sent < 0 )
        {
            m_last_error = errno;
        }
    }
    else
    {
        data_sent = -1;
    }

    return data_sent;
}

////////////////////////////////////////////////////////////////////////////////
AR::sint16 TcpSocket::receive(void* message, const AR::uint16&& len) noexcept
{
    const AR::boolean socket_open = is_socket_initialized();
    AR::sint16 data_received = -1;

    // sending only makes sense if at least the socket is open.
    if ( socket_open == TRUE )
    {
#ifdef _WIN32
        char* msg = static_cast< char* >(message);
#elif defined(__unix__)
        void* msg = message;
#endif        
        const SocketHandleType& handle = get_socket_handle();
        data_received = ::recv(handle, msg, len, 0);

        if ( data_received < 0 )
        {
            m_last_error = errno;
        }
    }
    else
    {
        data_received = -1;
    }

    return data_received;
}

