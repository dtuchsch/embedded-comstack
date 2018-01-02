/**
 * @file      TcpClient.cpp
 * @author    dtuchscherer <daniel.tuchscherer@gmail.com>
 * @brief     TCP client implementation
 * @details   For connecting to and disconnecting from TCP servers.
 * @version   1.0
 * @copyright Copyright (c) 2018, dtuchscherer.
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

#include "TcpClient.h"

////////////////////////////////////////////////////////////////////////////////
TcpClient::TcpClient() noexcept : TcpSocket() {}

////////////////////////////////////////////////////////////////////////////////
TcpClient::~TcpClient() noexcept {}

////////////////////////////////////////////////////////////////////////////////
bool TcpClient::connect(IpAddress ip_address, const std::uint16_t port) noexcept
{
    // return value
    bool connected_r = false;
    bool socket_created = is_socket_initialized();

    // look if the socket is there
    if (socket_created == true)
    {
        // first build the address
        std::uint32_t ip = ip_address.get_ip_address();
        struct sockaddr_in server;
        ip_address.create_address_struct(ip, port, server);

        // then establish the connection to the server
        const SocketHandleType& socket_handle = get_socket_handle();
        const auto connected = ::connect(
            socket_handle, (struct sockaddr*)&server, sizeof(struct sockaddr));

        if (connected >= 0)
        {
            connected_r = true;
        }
        else
        {
            connected_r = false;
            m_last_error = errno;
        }
    }
    else
    {
        connected_r = false;
    }

    return connected_r;
}

////////////////////////////////////////////////////////////////////////////////
bool TcpClient::disconnect() noexcept { return close_socket(); }
