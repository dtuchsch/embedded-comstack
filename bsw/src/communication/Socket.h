/**
 * \file      Socket.h
 * \author    dtuchscherer <daniel.tuchscherer@gmail.com>
 * \brief     Bare socket interface as a base for concrete socket
 * implementations.
 * \details   This is an interface of an "abstract" socket,
 *            derived classes of the base class Socket will then implement
 *            their own way to create and open a socket.
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
#include <chrono>
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
    INVALID = -1
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
<<<<<<< HEAD
 * \tparam Derived socket type. For instance this could be a TCP, UDP or
=======
 * \tparam Derived socket class for using CRTP. E.g. it could be a TCP, UDP or
>>>>>>> 08470d087e7481166db9570113988589489e4518
 * CAN socket.
 */
template < typename Derived > class Socket
{
  public:
    /**
     * \brief Default constructor initializing the socket
     */
    Socket() noexcept
<<<<<<< HEAD
        : last_error_{0}, socket_{get_invalid_alias()}, socket_init_{false},
          is_blocking_{true}
=======
        : m_last_error{0}, m_socket_init{false}, m_socket{get_invalid_alias()},
          m_is_blocking{true}
>>>>>>> 08470d087e7481166db9570113988589489e4518
    {
        // Create one socket on construction.
        socket_init_ = initialize();
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
        socket_init_ = false;
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
                socket_init_ = false;
                closed = true;
            }
            else
            {
                closed = false;
                last_error_ = errno;
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
    SocketErrorType get_last_error() const noexcept { return last_error_; }

    /**
     * \brief The socket handle for the derived classes as a reference.
     * \return the socket number as reference for write and read
     */
    const SocketHandleType& get_socket() const noexcept { return socket_; }

    /**
     * \brief If the interface is initialized.
     * \return true if the socket is open, false if not.
     */
    bool is_socket_initialized() noexcept { return socket_init_; }

    /**
     * \brief This method looks for an "event" on the socket by calling
     * select. If the socket is blocking, we can now look for data to receive
     * or if a TCP client wants to connect to the server, before entering
     * a blocking read or write.
     * \param[in] deadline Time to wait for pending data on the socket.
     * \return true if there is some kind of response (data or connect)
     * on the socket, false if not.
     */
    template < typename Duration >
    bool wait_for(const Duration&& deadline) noexcept
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
        const std::chrono::seconds sec =
            std::chrono::duration_cast< std::chrono::seconds >(deadline);
        const std::chrono::microseconds usec =
            std::chrono::duration_cast< std::chrono::microseconds >(deadline -
                                                                    sec);
        time_to_wait.tv_sec =
            static_cast< decltype(time_to_wait.tv_sec) >(sec.count());
        time_to_wait.tv_usec =
            static_cast< decltype(time_to_wait.tv_usec) >(usec.count());

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
<<<<<<< HEAD
     * false if it shall be non-blocking on a read on the socket.
     * \return true if the change was successful, false if setting the socket
     * option failed.
=======
     * false if it shall be non-blocking on a read.
     * \return true if it was successful changed, false if not.
>>>>>>> 08470d087e7481166db9570113988589489e4518
     */
    bool set_blocking(const bool blocking)
    {
        bool success = false;

        if (is_socket_initialized() == true)
        {
#ifdef __unix__
            int status = fcntl(socket_, F_GETFL);
#elif defined(_WIN32)
            int status = 1;
#else
#error "OS not supported!"
#endif
            if (status >= 0)
            {
                if (blocking)
                {
#ifdef __unix__
                    status = fcntl(socket_, F_SETFL, status & ~O_NONBLOCK);
#elif defined(_WIN32)
                    u_long nblock = 0;
                    status = ioctlsocket(socket_, FIONBIO, &nblock);
#else
#error "OS not defined!"
#endif
                    success = true;
                }
                else
                {
#ifdef __unix__
                    status = fcntl(socket_, F_SETFL, status | O_NONBLOCK);
#elif defined(_WIN32)
                    u_long nblock = 1;
                    status = ioctlsocket(socket_, FIONBIO, &nblock);
#else
#error "OS not defined!"
#endif
                    success = true;
                }
            }

            if (status >= 0)
            {
                is_blocking_ = blocking;
                success = true;
            }
            else
            {
                last_error_ = errno;
                success = false;
            }
        }

        return success;
    }

    /**
     * \brief Check if this socket is blocking or non-blocking.
     * \return true if the socket is blocking or false if non-blocking.
     */
    bool is_blocking() const noexcept { return is_blocking_; }

    /**
     * \brief Initializes the socket.
     * \return true if initialization was successful, false if there was an
     * error initializing the socket.
     */
    bool initialize() noexcept
    {
#ifdef _WIN32
        const int wsa_start = WSAStartup(MAKEWORD(2, 2), &wsa_data_);
        bool sock_created{false};

        if (wsa_start == 0)
        {
            sock_created = create();
        }
        else
        {
            std::cerr << "WSAStartup failed.\n";
            sock_created = false;
        }
#else
        const bool sock_created = create();
#endif

<<<<<<< HEAD
=======
        if (sock_created)
        {
            m_socket_init = true;
        }
        else
        {
            m_socket_init = false;
        }

>>>>>>> 08470d087e7481166db9570113988589489e4518
        return sock_created;
    }

    /**
     * \brief Assign a new socket handle to this socket object.
     * \param[in] new_handle The socket handle to assign.
     */
    bool assign(const SocketHandleType new_handle) noexcept
    {
        bool created = false;
        socket_ = new_handle;
        socket_init_ = true;
        created = true;
        return created;
    }

    /**
     * \brief Sets the error number for this socket to ease debugging.
     */
    void SetErrorNumber(SocketErrorType value) { last_error_ = value; }

  protected:
    /**
     * \brief The socket handle for the derived classes as a reference.
     * \return the socket number as reference for write/read and setting
     * options.
     */
    SocketHandleType get_socket_handle() const noexcept { return socket_; }

    /**
     * \brief A socket will be opened. Because the parameters for opening a
     * socket heavily relies on the protocol this method calls a method
     * of the concrete implementation (derived).
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

<<<<<<< HEAD
    /// Stores the last error occurred.
    SocketErrorType last_error_;

    /// This stores the socket handle.
    SocketHandleType socket_;

  private:
    /// true if everything is set up and the instance can receive and send data
    bool socket_init_;
=======
  private:
    /// true if everything is set up and the instance can receive and send data
    bool m_socket_init;

    /// This stores the socket handle.
    SocketHandleType m_socket;
>>>>>>> 08470d087e7481166db9570113988589489e4518

    /// Either the socket is in blocking or non-blocking mode.
    bool is_blocking_;

#ifdef _WIN32
    /// For windows sockets the WSAdata is necessary in addition.
    WSADATA wsa_data_;
#endif
};

#endif /* SOCKET_H_ */
