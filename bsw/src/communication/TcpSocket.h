/**
 * \file      TcpSocket.h
 * \author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * \brief     TCP socket implementation for sending, receiving and polling.
 * \details   TCP socket implementation for sending, receiving and polling.
 * \copyright Copyright (c) 2015, dtuchscherer.
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

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

#ifdef __unix__
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#endif

#include "Socket.h"

/**
 * \brief Concrete class for a Ethernet TCP/IP communication.
 */
class TcpSocket : public Socket< TcpSocket >
{
  public:
    /**
     * \brief Default constructor
     */
    TcpSocket() noexcept;

    /**
     * \brief Default destructor
     */
    ~TcpSocket() noexcept = default;

    /**
     * \brief Send via the TCP/IP socket
     * \param[in] message is the data to send
     * \param[in] len is the length to send
     * \return the number of bytes that have been sent or -1 if there is an
     * error.
     */
    std::int16_t send(const void* message, const std::uint16_t len) noexcept;

    /**
     * \brief Receive via the TCP/IP socket
     * \param[out] is the message container to store the received data
     * \param[in] the length to receive
     * \return how much data has been received. if there is an error the return
     * is smaller than 0.
     */
    std::int16_t receive(void* message, const std::uint16_t len) noexcept;

    /**
     * \brief Create a TCP socket; This method is called by the base class.
     * \return true if the socket is created or false if there was an error when
     * creating the socket.
     */
    bool create() noexcept;

    /**
     * \brief Disable the nagle algorithm and forces to send out data remaining
     * in the buffer immediately.
     * \remark Setting this option to true should be chosen carefully and only
     * if this is really needed (e.g. to get better "real-time behavior" when
     * using TCP/IP).
     * \return true if setting the option as given by the parameter was
     * successful or not.
     */
    bool set_nodelay(const bool option) noexcept;
};

#endif /* TCPSOCKET_H_ */
