/**
 * @file      TcpServer.cpp
 * @author    dtuchscherer <daniel.tuchscherer@gmail.com>
 * @brief     Ethernet TCP/IP Server implementation
 * @details   This is the module implementing the listening for and accepting of
 *            connections from TCP Clients.
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

#include "TcpServer.h"

////////////////////////////////////////////////////////////////////////////////
TcpServer::TcpServer() noexcept : m_connect(), m_data() {}

////////////////////////////////////////////////////////////////////////////////
bool TcpServer::listen(IpAddress ip_address, const std::uint16_t port) noexcept
{
    bool listen_success = false;
    // first build the address
    std::uint32_t ip = ip_address.get_ip_address();
    struct sockaddr_in client;
    ip_address.create_address_struct(ip, port, client);
    const auto handle = m_connect.get_socket();
    // bind the port to the socket
    const int bound = ::bind(handle, (struct sockaddr*)&client, sizeof(client));
    if (bound >= 0)
    {
        // make that socket a listening socket that listens on the port bound.
        const int li = ::listen(handle, 10);
        if (li >= 0)
        {
            listen_success = true;
        }
        else
        {
            m_connect.m_last_error = errno;
            listen_success = false;
        }
    }
    else
    {
        m_connect.m_last_error = errno;
        listen_success = false;
    }

    return listen_success;
}

////////////////////////////////////////////////////////////////////////////////
bool TcpServer::accept() noexcept
{
    bool accepted{false};
    struct sockaddr_in client;
#ifdef _WIN32
    int length = sizeof(client);
#elif defined(__unix__)
    socklen_t length = sizeof(client);
#else
#error "OS not supported."
#endif
    const auto handle = m_connect.get_socket();
    // accept the connection on the socket.
    const int data_socket =
        ::accept(handle, (struct sockaddr*)&client, &length);

    if (data_socket >= 0)
    {
        // we will copy the socket here to avoid interference between
        // the old data socket and a new one.
        // std::cout << "Closing possible previous socket...\n";
        m_data.close_socket();
        // std::cout << "Assign data socket from the accept call...";
        const auto as = m_data.assign(data_socket);

        if (as)
        {
            accepted = true;
        }
        else
        {
            std::cerr << "failed.\n";
            accepted = false;
        }
    }
    else
    {
        std::cerr << "failed.\n";
        m_connect.m_last_error = errno;
        accepted = false;
    }

    return accepted;
}

////////////////////////////////////////////////////////////////////////////////
bool TcpServer::reuse_addr() noexcept
{
    int reuse_addr = 1;
    const auto handle = m_connect.get_socket();
    const auto reuse = setsockopt(handle, SOL_SOCKET, SO_REUSEADDR,
                                  (char*)&reuse_addr, sizeof(reuse_addr));
    return static_cast< bool >(reuse == 0);
}
