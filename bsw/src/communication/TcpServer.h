/**
 * @file      TcpServer.h
 * @author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief     Ethernet TCP/IP Server interface
 * @details   This is the TCP/IP Server interface for listening and accepting
 *            connections.
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

#ifndef TCPSERVER_H_
# define TCPSERVER_H_

/*******************************************************************************
 * MODULES USED
 *******************************************************************************/
# include "IpAddress.h"
# include "TcpSocket.h"

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

/**
 * @brief The TcpServer class allows listening for pending connection requests
 * and then accepting incoming connections.
 */
class TcpServer
{
public:

    /**
     * @brief The default instructor will open a socket.
     */
    TcpServer() noexcept;

    /**
     * @brief The default destructor closes the socket, thus closing any
     * opening connection.
     */
    ~TcpServer() noexcept;

    /**
     * @brief Listens for connections.
     * @param[in] ip the IP4 address to listen on for incoming requests.
     * @param[in] port the port to listen on for incoming requests.
     * @return true if listening is possible, false if listening is not
     * possible.
     */
    AR::boolean listen(IpAddress& ip, const AR::uint16 port) noexcept;

    /**
     * @brief Accepts a connection.
     * @return true if a connection is accepted, false if not.
     */
    AR::boolean accept() noexcept;

    //! socket that accepts the connections from TCP clients.
    TcpSocket m_connect;
    
    //! after accepting a connection via this socket data can be
    //! exchanged between TCP client and TCP server.
    TcpSocket m_data;
    
private:
};

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

#endif /* TCPSERVER_H_ */
