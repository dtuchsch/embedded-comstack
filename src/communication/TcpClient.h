/**
 * @file      TcpClient.h
 * @author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief     TCP client class interface
 * @details   For connecting to and disconnecting from TCP servers.
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

#ifndef TCPCLIENT_H_
# define TCPCLIENT_H_

/*******************************************************************************
 * MODULES USED
 *******************************************************************************/
#include "IpAddress.h"
#include "TcpSocket.h"

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/
class TcpClient : public TcpSocket
{
public:
    /**
     * Default constructor calls the base class Socket to create a socket.
     */
    TcpClient() noexcept;

    ~TcpClient() noexcept;

    /**
     * @brief Connect to a TCP/IP server.
     * @param[in] ip_address the IP4 address
     * @param[in] port the port to talk to
     * @return true if connection is established, false if it fails to connect
     * to the server.
     */
    boolean connect(IpAddress& ip_address, uint16 port) noexcept;

    /**
     * @brief explicitly close the socket for disconnection.
     */
    boolean disconnect() noexcept;

private:
};
/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/



#endif /* TCPCLIENT_H_ */
