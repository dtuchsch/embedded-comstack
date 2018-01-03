/**
 * \file      CanSocket.h
 * \author    dtuchscherer <daniel.tuchscherer@gmail.com>
 * \brief     CAN interface to send and receive CAN frames over SocketCAN.
 * \details   This is a CAN module to send and receive data over CAN bus.
 *            It is designed for Linux systems. The CAN communication
 *            is done via SocketCAN.
 * \copyright Copyright (c) 2018, dtuchscherer.
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
#define CAN_H_
#ifndef _WIN32

#include "Socket.h" // uses sockets under Linux
#include <array>    // rx, tx
#include <cassert>
#include <cstring>         // strcopy for interface name
#include <linux/can.h>     // sockaddr structure, protocols and can_filter
#include <linux/can/raw.h> // filtering
#include <net/if.h>        // interface name
#include <sys/ioctl.h>     // blocking / non-blocking
#include <unistd.h>        // write and read for CAN interface

/**
 * \brief Defining a struct that holds informations about standard CAN frames.
 */
struct CAN_STD
{
    // the standard CAN frame contains a total maximum of 8 bytes of user data.
    static constexpr std::size_t DATA_LEN{8U};
};

/**
 * \brief Defining a struct that holds informations about CAN FD frames.
 */
struct CAN_FD
{
    // A CAN FD frame contains a total maximum of 64 bytes of user data.
    static constexpr std::size_t DATA_LEN{64U};
};

/// Forward type. You may use these types to declare data packets to send and
/// receive.
using CanDataType = std::array< std::uint8_t, CAN_STD::DATA_LEN >;
using CanStdData = CanDataType;
using CanFDData = std::array< std::uint8_t, CAN_FD::DATA_LEN >;
using CanIDType = canid_t;

/**
 * \brief CanSocket is used for sending and receiving standard CAN frames and
 * CAN FD frames.
 */
class CanSocket : public Socket< CanSocket >
{
  public:
    /**
     * \brief Default constructor initializing the socket base class,
     * initializing the CAN.
     * \param[in] ifrname interface name, e.g. "can0", "vcan0"
     */
    template < std::size_t N >
    explicit CanSocket(const char (&interface_str)[N]) noexcept
        : Socket{}, can_init_{false}
    {
        // before we set up the CAN interface, create a socket to send and
        // receive data through.
        bool sock_created = is_socket_initialized();
        assert(sock_created);

        if (sock_created == true)
        {
            // check if the interface is registered
            bool interface_exists = check_interface(interface_str, ifr_);

            // socket creation successful and interface exists
            if (interface_exists == true)
            {
                bool bind_success = bind_if_socket();
                assert(bind_success);
                if (bind_success)
                {
                    // we turn on CAN FD mode automatically as a standard
                    // setting.
                    // This makes it possible to send both standard frames and
                    // CAN FD frames.
                    const bool canfd = enable_canfd();
                    assert(canfd);
                    can_init_ = true;
                }
                else
                {
                    std::cerr << "Binding the interface to the created socket "
                                 "failed.\n";
                    can_init_ = false;
                }
            }
            else
            {
                // the interface specified is not available.
                std::cerr << "CAN interface " << interface_str
                          << " you've specified is not found!\nAre you "
                             "sure you've added the device?\n";
                can_init_ = false;
            }
        }
        else
        {
            std::cerr << "Socket creation failed.\n";
            can_init_ = false;
        }
    }

    /**
     * \brief Transmits a message on CAN bus.
     * \tparam CANData Deduced type: Whether this is a standard CAN frame or a
     * CAN FD frame.
     * \param[in] can_id CAN identifier to transmit the message.
     * \param[in] data This is an array of bytes that contains
     * data and is being transmitted on CAN bus with the given CAN ID and DLC.
     * \param[in] len Length in bytes to send. This is then written to the DLC
     * field.
     * One standard CAN frame may contain 8 bytes as a maximum.
     * \return the length transmitted. If send was successfull this must be
     * the number of bytes equally to CAN_MTU = 16 for standard frames and
     * CANFD_MTU = 72 for CAN FD frames.
     * \remarks If you want to send more than 8 byte as a complete
     * message, you need a transport layer such as CANTp / ISO-TP.
     */
    template < typename CANData >
    std::int8_t send(const CanIDType can_id, const CANData& data,
                     const std::uint8_t len) noexcept
    {
        // initialize with zero value. uninitialized stack variables are a
        // common error source.
        std::int8_t data_sent{0};
        // must be the correct array size. minimizes static analysis efforts by
        // checking this at compile-time
        static_assert(std::is_same< CanStdData, CANData >::value ||
                          std::is_same< CanFDData, CANData >::value,
                      "Must be a standard CAN frame or CAN FD frame.");

        // Select the can frame type: may be a standard CAN frame or CAN FD
        // frame
        typedef typename std::conditional<
            std::is_same< CanStdData, CANData >::value, struct can_frame,
            struct canfd_frame >::type SelectedFrame;

        // First check if the file descriptor for the socket was initialized
        // and the interface is up and running.
        if (is_can_initialized() && (len > 0U))
        {
            // Structure that is given to the POSIX write function.
            // We have to define CAN-ID, length (DLC) and copy the data to send.
            struct canfd_frame frame;
            // clear all to make sure we don't send rubbish.
            std::memset(&frame, 0, sizeof(SelectedFrame));
            // set the CAN ID of this message to send
            frame.can_id = can_id;
            // check if length is possible for one CAN frame...
            // limit the length to max DLC of standard CAN or CAN FD
            frame.len = std::min(static_cast< decltype(data.size()) >(len),
                                 data.size());

            // copy data into the data field of the frame struct.
            for (std::uint8_t i = 0U; i < len; ++i)
            {
                frame.data[i] = data[i];
            }

            const auto socket = get_socket_handle();
            // We need to transmit the size of struct can_frame with a POSIX
            // write.
            const auto send_res = write(socket, &frame, sizeof(SelectedFrame));
            data_sent = static_cast< std::int8_t >(send_res);

            // Check if the desired length was transmitted over the socket.
            // A maximum of 8 bytes can be transmitted within a CAN message
            // without using IsoTp.
            if (data_sent <= 0)
            {
                std::cerr << "Sending CAN frame failed with error number: "
                          << errno << "\n";
                // store the transport layer error number, other layers may
                // access to do an advanced and application specific error
                // handling.
                last_error_ = errno;
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

    /**
     * \brief Receives a CAN message from the socket and
     * writes the data into an array (blocking read).
     * \param[out] can_id CAN identifier of the message received.
     * \param[out] data_ref Array to store the received packet data to.
     * \return Greater than zero if data was received.
     * This returns -1 if there was an error or timeout.
     */
    std::int8_t receive(CanIDType& can_id, CanFDData& data_ref) noexcept;

    /**
     * \brief Receives a CAN message from the socket and writes the data into
     * an array (non-blocking read / timeout / polling possible).
     * \param[out] can_id   CAN identifier of the message received.
     * \param[out] data_ref Array to store the received packet data to.
     * \param[in] timeout timeout of the read function in micro seconds
     * \return Greater than zero if data was received.
     * If there is a timeout it returns zero.
     * If there was an error, -1 is transmitted.
     */
    template < typename Duration >
    std::int8_t receive(CanIDType& can_id, CanDataType& data_ref,
                        const Duration&& deadline) noexcept;

    /**
     * \brief Create a CAN socket / file descriptor to send and receive.
     * \return true if the socket is opened or false if there was an error.
     */
    bool create() noexcept;

    /**
     * \brief Checks if the CAN is initialized properly.
     */
    bool is_can_initialized() const noexcept;

    /**
     * \brief Switch to CAN FD mode. Configure the interface to send and receive
     * CAN FD frames.
     * \return true if enabling CAN FD was possible, false if not.
     */
    bool enable_canfd() noexcept;

  private:
    /**
     * \brief Check if the interface exists and is known to the OS.
     * \param[in] interface_str the interface name as string.
     * \return true if the interface is known. If the system
     * does not know the given interface it will return false.
     */
    template < std::size_t N >
    static bool check_interface(const char (&interface_str)[N],
                                struct ifreq& can_if) noexcept
    {
        bool exists{false};
        // copying the interface string into the structure.
        std::strncpy(can_if.ifr_name, interface_str, IFNAMSIZ - 1U);
        // make sure the string is terminated properly.
        can_if.ifr_name[IFNAMSIZ - 1U] = '\0';
        // map the interface string to a given interface of the system.
        can_if.ifr_ifindex = if_nametoindex(can_if.ifr_name);

        // check if the CAN interface is known to the system
        if (can_if.ifr_ifindex != 0)
        {
            // the CAN interface is known to the system
            exists = true;
        }
        else
        {
            // ... error finding the given interface. The interface is not known
            // to the system.
            exists = false;
        }

        return exists;
    }

    /**
     * \brief If the interface exists this will bind the interface to the
     * socket.
     * \return true if binding the interface to the socket was succesful -
     * otherwise false
     */
    bool bind_if_socket() noexcept;

    /// Holds the index of the interface in a struct if
    /// the interface exists. Works as a handle for configuration.
    struct ifreq ifr_;

    /// Holds the address family CAN and
    /// the interface index to bind the socket to.
    struct sockaddr_can sockaddr_;

    /// Whether the socket creation, binding and interface is ok, configured or
    /// not.
    bool can_init_;

    // determine the MTU with the help of the struct on compile-time
    static constexpr auto can_mtu_ = sizeof(struct canfd_frame);
};

#endif // WIN32 detection
#endif // CAN_H
