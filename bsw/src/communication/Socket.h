/**
 * @file      Socket.h
 * @author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief     Bare Socket Interface
 * @details   This is an interface of an "abstract" socket,
 *            derived classes of the base class Socket will then implement
 *            their own way to create and open a socket.
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

#ifndef SOCKET_H_
# define SOCKET_H_

/*******************************************************************************
 * MODULES USED
 *******************************************************************************/

# ifdef _WIN32
#  include <winsock2.h>
#  include <io.h>
# elif defined(__unix__)
#  include <sys/socket.h>
#  include <fcntl.h>
#  include <unistd.h>
#  include <cerrno>
# else
#  error "OS not defined!"
# endif
#include <iostream>

# include "ComStack_Types.h"

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

# ifdef __unix__
using SocketHandleType = int;
using SocketErrorType = int;
# elif defined (_WIN32)
using SocketHandleType = SOCKET;
using SocketErrorType = int;
#  undef errno
#  define errno WSAGetLastError()
# else
#  error "OS not defined!"
# endif

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

enum class SocketState
    : SocketHandleType
    {
        INVALID
};

constexpr SocketHandleType get_invalid_alias()
{
    return static_cast< SocketHandleType >(SocketState::INVALID);
}

enum class SocketType
{
    CAN,
    TCP,
    UDP
};

/**
 * @brief A Socket implementation
 * @tparam Derived class for CRTP
 */
template< typename Derived >
class Socket
{
public:

    /**
     * @brief Default constructor initializing the socket;
     * @param[in] type defines how the socket will be initialized.
     */
    Socket(SocketType type) noexcept :
			m_type(type),
			m_last_error(0),
			m_socket_init(FALSE),
			m_socket(get_invalid_alias()),
			m_is_blocking(TRUE)
    {
        initialize();
    }

    /**
     * @brief Destructor closes the socket.
     */
    ~Socket() noexcept
    {
        close_socket();
#ifdef _WIN32
        WSACleanup();
#endif
        m_socket_init = FALSE;
    }

    /**
     * @brief Closes the socket explicitly.
     * @return true if a close was successful, false if not.
     */
    AR::boolean close_socket() noexcept
    {
        AR::boolean closed = FALSE;

        if ( is_socket_initialized() == TRUE )
        {
# ifdef __unix__
            // close the socket if one is opened only
        	::shutdown(get_socket_handle(), -2);
            const AR::sint16 cl = ::close(get_socket_handle());
# elif defined (_WIN32)
            const AR::sint16 cl = ::closesocket(get_socket_handle());
# else
#  error "Implementation of Socket::close_socket() failed. OS not defined!"
# endif

            if ( cl == 0 )
            {
                m_socket_init = FALSE;
                closed = TRUE;
            }
            else
            {
                closed = FALSE;
                m_last_error = errno;
            }
        }
        else
        {
        	// is closed already.
        	std::cout << "Socket is closed already.\n";
        	closed = TRUE;
        }

        return closed;
    }

    /**
     * @brief Gets the last error of the socket communication for
     * error handling purposes.
     * @return The last stored error in the socket communication from errno.
     */
    SocketErrorType get_last_error() const noexcept
    {
        return m_last_error;
    }

    /**
     * @brief The socket handle for the derived classes as a reference.
     * @return the socket number as reference for write and read
     */
    const SocketHandleType& get_socket() const noexcept
    {
        return m_socket;
    }

    /**
     * @brief If the interface is initialized.
     * @return true if the socket is open, false if not.
     */
    AR::boolean is_socket_initialized() noexcept
    {
        return m_socket_init;
    }

    /**
     * @brief This method looks for an "event" on the socket by calling
     * select. If the socket is blocking, we can now look for data to receive
     * or if a TCP client wants to connect to the server, before entering
     * a blocking read or write.
     * @return true if there is some kind of response (data or connect)
     * on the socket, false if not.
     */
    AR::boolean poll_activity(const AR::uint16 timeout_us) noexcept
    {
        SocketHandleType nfds;
        AR::boolean response_on_socket = FALSE;

        /* OS specific declarations */
# ifdef _WIN32
        SocketHandleType socket = get_socket_handle();

        /* The first argument of the "select()" system call must be the length
         * of the bitfield. Under Windows the first argument of "select()" should be 0. */
        nfds = static_cast< SocketHandleType >(0);

# elif defined __unix__
        SocketHandleType socket = get_socket_handle();
        nfds = static_cast< SocketHandleType >(socket + 1);
# else
#  error "OS not defined!"
# endif

        struct timeval time_to_wait;
        time_to_wait.tv_sec = static_cast< decltype(time_to_wait.tv_sec) >(0);
        time_to_wait.tv_usec = static_cast< decltype(time_to_wait.tv_sec) >(timeout_us);

        fd_set fd_read;
        FD_ZERO(&fd_read);
        FD_SET(socket, &fd_read);

        const auto select_return = select(nfds, &fd_read, NULL_PTR, NULL_PTR,
                &time_to_wait);

        if ( select_return > 0 )
        {
            /*
             * If the return value of function call select() isn't -1,
             * there is data on the socket to receive.
             * The select returns 0 if the time expired (timeout).
             */
            response_on_socket = TRUE;
        }
        else
        {
            /* The return value of the function select is -1 (SOCKET_ERROR) or a timeout occurred. */
            response_on_socket = FALSE;
        }

        return response_on_socket;
    }

    /**
     * @brief Set the socket into blocking or non-blocking mode.
     * @return true if it was successful changed, false if not.
     */
    AR::boolean set_blocking(const AR::boolean blocking)
    {
        AR::boolean success = FALSE;

        if ( is_socket_initialized() == TRUE )
        {
# ifdef __unix__
            int status = fcntl(m_socket, F_GETFL);
# elif defined (_WIN32)
            int status = 1;
# else
#  error "OS not defined!"
# endif
            if ( status >= 0 )
            {
                if ( blocking == TRUE )
                {
# ifdef __unix__
                    status = fcntl(m_socket, F_SETFL, status & ~O_NONBLOCK);
# elif defined (_WIN32)
                    u_long nblock = 0;
                    status = ioctlsocket(m_socket, FIONBIO, &nblock);
# else
#  error "OS not defined!"
# endif
                    success = TRUE;
                }
                else
                {
# ifdef __unix__
                    status = fcntl(m_socket, F_SETFL, status | O_NONBLOCK);
# elif defined (_WIN32)
                    u_long nblock = 1;
                    status = ioctlsocket(m_socket, FIONBIO, &nblock);
# else
#  error "OS not defined!"
# endif 
                    success = TRUE;
                }
            }

            if ( status >= 0 )
            {
                m_is_blocking = blocking;
                success = TRUE;
            }
            else
            {
                m_last_error = errno;
                success = FALSE;
            }
        }

        return success;
    }

    /**
     * @brief Check if this socket is blocking or non-blocking.
     * @return true if the socket is blocking or false if non-blocking.
     */
    AR::boolean is_blocking() const noexcept
    {
        return m_is_blocking;
    }

    /**
     * @brief initializes the socket.
     * @return true if initialization was successful, false if there was an error
     * initializing the socket.
     */
    AR::boolean initialize() noexcept
    {
# ifdef _WIN32
        const int wsa_start = WSAStartup(MAKEWORD(2, 2), &m_wsa_data);
        AR::boolean sock_created{FALSE};

        if ( wsa_start == 0 )
        {
            sock_created = create();
        }
        else
        {
            sock_created = FALSE;
        }
# else
        const AR::boolean sock_created = create();
# endif

        if ( sock_created == TRUE )
        {
            m_socket_init = TRUE;
        }
        else
        {
            m_socket_init = FALSE;
        }

        return sock_created;
    }


    /**
     * @brief Assign a new socket.
     */
    AR::boolean assign(const SocketHandleType& new_handle) noexcept
    {
        AR::boolean created = FALSE;
		m_socket = new_handle;
		m_socket_init = TRUE;
		created = TRUE;
        return created;
    }

    //! stores the last error in this attribute
    SocketErrorType m_last_error;

protected:

    /**
     * @brief The socket handle for the derived classes as a reference.
     * @return the socket number as reference for write and read
     */
    SocketHandleType& get_socket_handle() noexcept
    {
        return m_socket;
    }

    /**
     * @brief A socket will be opened. Because the parameters for opening a
     * socket strongly relies on the protocol this method calls a method
     * of the class derived.
     * @return true if the socket creation was successful, false if not.
     */
    AR::boolean create() noexcept
    {
        AR::boolean created = FALSE;

        // only if the current socket is closed
        if ( is_socket_initialized() == FALSE )
        {
            // CRTP
            created = static_cast< Derived* >(this)->create();
        }
        else
        {
            created = FALSE;
        }

        return created;
    }

    //! this attribute shows if it's a socket for can, ethernet tcp/ip or udp
    SocketType m_type;

private:

    //! true if everything is set up and the instance can receive and send data
    AR::boolean m_socket_init;

    //! this stores the socket handle.
    SocketHandleType m_socket;

    //! Either the socket is in blocking or non-blocking mode.
    AR::boolean m_is_blocking;

# ifdef _WIN32
    //! for windows sockets the WSAdata is necessary
    WSADATA m_wsa_data;
# endif
};

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

#endif /* SOCKET_H_ */
