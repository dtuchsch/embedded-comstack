/**
 * @file      Socket.h
 * @author    dtuchscherer <daniel.tuchscherer@gmail.com>
 * \brief     Bare Socket Interface
 * @details   This is an interface of an "abstract" socket,
 *            derived classes of the base class Socket will then implement
 *            their own way to create and open a socket.
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

#ifndef SOCKET_H_
#define SOCKET_H_

#ifdef _WIN32
#include <io.h>
#include <winsock2.h>
#elif defined(__unix__)
#include <cerrno>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#else
#error "OS not supported! Please define an operating system."
#endif
#include <iostream>

#ifdef __unix__
using SocketHandleType = int;
using SocketErrorType = int;
#elif defined(_WIN32)
using SocketHandleType = SOCKET;
using SocketErrorType = int;
#undef errno
#define errno WSAGetLastError()
#else
#error "OS not supported!"
#endif

enum class SocketState : SocketHandleType
{
    INVALID
};

/**
 * \brief At init time the socket is not created, thus invalid.
 */
constexpr SocketHandleType get_invalid_alias()
{
    return static_cast< SocketHandleType >(SocketState::INVALID);
}

/**
 * \brief A generic socket implementation
 * \tparam Derived socket class for using CRTP. E.g. it could be a TCP, UDP or
 * CAN socket.
 */
template < typename Derived > class Socket
{
  public:
    /**
     * \brief Default constructor initializing the socket
     */
    Socket() noexcept
        : m_last_error{0}, m_socket_init{false}, m_socket{get_invalid_alias()},
          m_is_blocking{true}
    {
        // create one socket.
        initialize();
    }

    /**
     * \brief Destructor closes the socket.
     * Clean up / close the socket in this particular case.
     */
    ~Socket() noexcept
    {
        close_socket();
#ifdef _WIN32
        // the WSA clean up is valid under windows only.
        WSACleanup();
#endif
        m_socket_init = false;
    }

    /**
     * \brief Closes the socket explicitly.
     * \return true if a close was successful, false if the socket was not
     * initialized, but close_socket() was called.
     */
    bool close_socket() noexcept
    {
        bool closed = false;

        if (is_socket_initialized() == true)
        {
#ifdef __unix__
            // close the socket if one is opened only
            ::shutdown(get_socket_handle(), -2);
            const std::int16_t cl = ::close(get_socket_handle());
#elif defined(_WIN32)
            const std::int16_t cl = ::closesocket(get_socket_handle());
#else
#error "Implementation of Socket::close_socket() failed. OS not supported!"
#endif

            if (cl == 0)
            {
                m_socket_init = false;
                closed = true;
            }
            else
            {
                closed = false;
                m_last_error = errno;
            }
        }
        else
        {
            // is closed already.
            std::cerr << "Socket is closed already.\n";
            closed = true;
        }

        return closed;
    }

    /**
     * \brief Gets the last error of the socket communication for
     * error handling purposes.
     * \return The last stored error in the socket communication from errno.
     */
    SocketErrorType get_last_error() const noexcept { return m_last_error; }

    /**
     * \brief The socket handle for the derived classes as a reference.
     * \return the socket number as reference for write and read
     */
    const SocketHandleType& get_socket() const noexcept { return m_socket; }

    /**
     * \brief If the interface is initialized.
     * \return true if the socket is open, false if not.
     */
    bool is_socket_initialized() noexcept { return m_socket_init; }

    /**
     * \brief This method looks for an "event" on the socket by calling
     * select. If the socket is blocking, we can now look for data to receive
     * or if a TCP client wants to connect to the server, before entering
     * a blocking read or write.
     * \return true if there is some kind of response (data or connect)
     * on the socket, false if not.
     */
    bool poll_activity(const std::uint16_t timeout_us) noexcept
    {
        SocketHandleType nfds;
        bool response_on_socket = false;

        /* OS specific declarations */
#ifdef _WIN32
        SocketHandleType socket = get_socket_handle();

        /* The first argument of the "select()" system call must be the length
         * of the bitfield. Under Windows the first argument of "select()"
         * should be 0. */
        nfds = static_cast< SocketHandleType >(0);

#elif defined __unix__
        SocketHandleType socket = get_socket_handle();
        nfds = static_cast< SocketHandleType >(socket + 1);
#else
#error "OS not supported!"
#endif

        struct timeval time_to_wait;
        time_to_wait.tv_sec = static_cast< decltype(time_to_wait.tv_sec) >(0);
        time_to_wait.tv_usec =
            static_cast< decltype(time_to_wait.tv_sec) >(timeout_us);

        fd_set fd_read;
        FD_ZERO(&fd_read);
        FD_SET(socket, &fd_read);

        const auto select_return =
            select(nfds, &fd_read, nullptr, nullptr, &time_to_wait);

        if (select_return > 0)
        {
            /*
             * If the return value of function call select() isn't -1,
             * there is data on the socket to receive.
             * The select returns 0 if the time expired (timeout).
             */
            response_on_socket = true;
        }
        else
        {
            /* The return value of the function select is -1 (SOCKET_ERROR) or a
             * timeout occurred. */
            response_on_socket = false;
        }

        return response_on_socket;
    }

    /**
     * \brief Set the socket into blocking or non-blocking mode.
     * \param[in] blocking true if this socket shall be blocking on a receive;
     * false if it shall be non-blocking on a read.
     * \return true if it was successful changed, false if not.
     */
    bool set_blocking(const bool blocking)
    {
        bool success = false;

        if (is_socket_initialized() == true)
        {
#ifdef __unix__
            int status = fcntl(m_socket, F_GETFL);
#elif defined(_WIN32)
            int status = 1;
#else
#error "OS not supported!"
#endif
            if (status >= 0)
            {
                if (blocking == true)
                {
#ifdef __unix__
                    status = fcntl(m_socket, F_SETFL, status & ~O_NONBLOCK);
#elif defined(_WIN32)
                    u_long nblock = 0;
                    status = ioctlsocket(m_socket, FIONBIO, &nblock);
#else
#error "OS not defined!"
#endif
                    success = true;
                }
                else
                {
#ifdef __unix__
                    status = fcntl(m_socket, F_SETFL, status | O_NONBLOCK);
#elif defined(_WIN32)
                    u_long nblock = 1;
                    status = ioctlsocket(m_socket, FIONBIO, &nblock);
#else
#error "OS not defined!"
#endif
                    success = true;
                }
            }

            if (status >= 0)
            {
                m_is_blocking = blocking;
                success = true;
            }
            else
            {
                m_last_error = errno;
                success = false;
            }
        }

        return success;
    }

    /**
     * \brief Check if this socket is blocking or non-blocking.
     * \return true if the socket is blocking or false if non-blocking.
     */
    bool is_blocking() const noexcept { return m_is_blocking; }

    /**
     * \brief initializes the socket.
     * \return true if initialization was successful, false if there was an
     * error initializing the socket.
     */
    bool initialize() noexcept
    {
#ifdef _WIN32
        const int wsa_start = WSAStartup(MAKEWORD(2, 2), &m_wsa_data);
        bool sock_created{false};

        if (wsa_start == 0)
        {
            sock_created = create();
        }
        else
        {
            sock_created = false;
        }
#else
        const bool sock_created = create();
#endif

        if (sock_created)
        {
            m_socket_init = true;
        }
        else
        {
            m_socket_init = false;
        }

        return sock_created;
    }

    /**
     * \brief Assign a new socket.
     */
    bool assign(const SocketHandleType& new_handle) noexcept
    {
        bool created = false;
        m_socket = new_handle;
        m_socket_init = true;
        created = true;
        return created;
    }

    /// stores the last error occurred.
    SocketErrorType m_last_error;

  protected:
    /**
     * \brief The socket handle for the derived classes as a reference.
     * \return the socket number as reference for write and read
     */
    SocketHandleType& get_socket_handle() noexcept { return m_socket; }

    /**
     * \brief A socket will be opened. Because the parameters for opening a
     * socket strongly relies on the protocol this method calls a method
     * of the class derived.
     * \return true if the socket creation was successful, false if not.
     */
    bool create() noexcept
    {
        bool created{false};

        // only if the current socket is closed
        if (is_socket_initialized() == false)
        {
            // CRTP
            created = static_cast< Derived* >(this)->create();
        }

        return created;
    }

  private:
    /// true if everything is set up and the instance can receive and send data
    bool m_socket_init;

    /// This stores the socket handle.
    SocketHandleType m_socket;

    /// Either the socket is in blocking or non-blocking mode.
    bool m_is_blocking;

#ifdef _WIN32
    /// for windows sockets the WSAdata is necessary
    WSADATA m_wsa_data;
#endif
};

#endif /* SOCKET_H_ */
