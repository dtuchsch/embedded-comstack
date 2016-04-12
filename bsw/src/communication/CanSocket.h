/**
 * @file      CanSocket.h
 * @author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief     CAN interface to send and receive CAN frames over SocketCAN.
 * @details   This is a CAN module to send and receive data over CAN bus.
 *            It is designed for Linux systems. The CAN communication
 *            is done via SocketCAN.
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

#ifndef CAN_H_
# define CAN_H_
# ifndef _WIN32
/*******************************************************************************
 * MODULES USED
 *******************************************************************************/

/* write and read */
#  include <unistd.h>

/* interface name */
#  include <net/if.h>

/* blocking / non-blocking */
#  include <sys/ioctl.h>

/* sockaddr structure, protocols and can_filter */
#  include <linux/can.h>

/* filtering */
#  include <linux/can/raw.h>

#  include <array>        // rx, tx
#  include <cstring>

#  include "Socket.h"

/* @req CAN388 */
#  include "ComStack_Types.h"

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

//! Forward type
using CanDataType = std::array< AR::uint8, 8U >;

/**
 * @brief
 */
class CanSocket: public Socket< CanSocket >
{
public:

    /**
     * @brief Default constructor initializing the socket base class,
     * initializing the CAN.
     * @param[in] ifrname interface name, e.g. "can0", "vcan0"
     */
    CanSocket(const char* interface_str) noexcept;

    /**
     * @brief Destructor closes the file descriptor of the socket.
     */
    ~CanSocket() noexcept;

    /**
     * @brief Transmits a message on CAN bus.
     * @param[in] can_id CAN identifier to transmit the message.
     * @param[in] data_ref This is an array of bytes that contains
     * the packet and is being transmitted on CAN bus with the given CAN ID.
     * @param[in] len Length in bytes to send.
     * One CAN packet may contain 8 bytes at a maximum.
     * @return the length transmitted. if everything works fine this must be
     * the number of bytes equally to CAN_MTU = 16.
     * @remarks If you want to send more than 8 bytes as a message
     * you need a transport layer like CanTp / ISO-TP.
     */
    AR::sint8 send(const AR::uint16 can_id, const CanDataType& data_ref,
               const AR::uint8 len) noexcept;

    /**
     * @brief Receives a CAN message from the socket and
     * writes the data into an array (blocking read).
     * @param[out] can_id CAN identifier of the message received.
     * @param[out] data_ref Array to store the received packet data to.
     * @return Greater than zero if data was received.
     * This returns -1 if there was an error or timeout.
     */
    AR::sint8 receive(AR::uint16& can_id, CanDataType& data_ref) noexcept;

    /**
     * @brief Receives a CAN message from the socket and writes the data into
     * an array (non-blocking read / timeout / polling possible).
     * @param[out] can_id   CAN identifier of the message received.
     * @param[out] data_ref Array to store the received packet data to.
     * @param[in] timeout timeout of the read function in micro seconds
     * @return Greater than zero if data was received.
     * If there is a timeout it returns zero.
     * If there was an error, -1 is transmitted.
     */
    AR::sint8 receive(AR::uint16& can_id, CanDataType& data_ref,
                  const AR::uint16 timeout_us) noexcept;

    /**
     * @brief Create a CAN socket
     * @return true if the socket is opened or false if there was an error.
     */
    AR::boolean create() noexcept;

    /**
     *
     */
    AR::boolean is_can_initialized() const noexcept;

protected:

private:

    /**
     * @brief checking if the interface exists in the system.
     * @return true if the interface is existend in the system, if the system
     * does not know the given interface it will return false.
     */
    AR::boolean check_interface(const char* interface_str) noexcept;

    /**
     * @brief if the interface exists we will bind the interace to the socket.
     * @return true if binding the interface to the socket was succesful -
     * otherwise false
     */
    AR::boolean bind_if_socket() noexcept;

    //! Holds the index of the interface in a struct if
    //! the interface exists.
    struct ifreq m_ifr;

    //! Holds the address family CAN and
    //! the interface index to bind the socket to.
    struct sockaddr_can m_sockaddr;

    //! Whether the socket creation, binding and interface is ok or not.
    AR::boolean m_can_init;

    // determine the size of the struct on compile-time
    // this is always the same
    static constexpr auto m_can_mtu = sizeof(struct can_frame);
};

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/
# endif /* WIN32 */
#endif /* CAN_H_ */
