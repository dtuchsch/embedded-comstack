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
# include <sys/socket.h>
# include <unistd.h>
# include <cerrno>
# include "ComStack_Types.h"

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

# ifdef __unix__
using SocketHandleType = int;
using SocketErrorType = int;
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
    m_socket(get_invalid_alias()),
    m_last_error(0),
    m_socket_init(FALSE)
    {
        const boolean sock_created = create();

        if ( sock_created == TRUE )
        {
            m_socket_init = TRUE;
        }
    }

    /**
     * @brief Destructor of Socket.
     */
    ~Socket() noexcept
    {
        close_socket();
        m_socket_init = FALSE;
    }

    /**
     * @brief closes the socket
     */
    boolean close_socket()
    {
        boolean closed = FALSE;

        if ( is_socket_initialized() == TRUE )
        {
            // close the socket if one is opened only
            const sint16 cl = ::close(get_socket_handle());

            if ( cl == 0 )
            {
                closed = TRUE;
            }
            else
            {
                closed = FALSE;
                m_last_error = errno;
            }

            return closed;
        }
    }

    /**
     * @brief Gets the last error of the socket communication for
     * error handling purposes.
     * @return The last stored error from errno.
     */
    SocketErrorType get_last_error() const noexcept
    {
        return m_last_error;
    }

    /**
     * @brief Returns the socket handle for send and receive.
     * @return the socket handle
     */
    SocketHandleType get_socket_handle() const noexcept
    {
        return m_socket;
    }

    /**
     * @brief If the interface is initialized.
     * @return if the interface is initialized
     */
    boolean is_socket_initialized() noexcept
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
    boolean poll_activity(const uint16 timeout_us) noexcept
    {
        SocketHandleType nfds;
        boolean response_on_socket = FALSE;

        /* OS specific declarations */
#ifdef _WIN32
        SOCKET sockOS = (SOCKET)sock;

        /* The first argument of the "select()" system call must be the length
         * of the bitfield. Under Windows the first argument of "select()" should be 0. */
        nfds = static_cast< SocketHandleType >(0);

#elif defined __unix__
        SocketHandleType socket = get_socket_handle();
        nfds = static_cast< SocketHandleType >(socket + 1);
#else
#endif

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

protected:

    /**
     * @brief A socket will be opened.
     * @return true if the socket creation was successful, false if not.
     */
    bool create() noexcept
    {
        // CRTP
        return static_cast< Derived* >(this)->create();
    }

    /**
     * @brief The socket handle for the derived classes as reference.
     * @return the socket handle as reference to write and read
     */
    SocketHandleType& get_socket_handle() noexcept
    {
        return m_socket;
    }

    //! this attribute shows if it's a socket for can, ethernet tcp/ip or udp
    SocketType m_type;

    //! stores the last error in this attribute
    SocketErrorType m_last_error;
private:

    //! true if everything is set up and the instance can receive and send data
    boolean m_socket_init;

    //! this stores the socket handle.
    SocketHandleType m_socket;

};

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

#endif /* SOCKET_H_ */
