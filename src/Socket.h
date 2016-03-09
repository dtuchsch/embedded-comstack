/**
 * @file      Socket.h
 * @author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief     Bare Socket Interface
 * @details
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

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

#ifdef __unix__
using SocketHandleType = int;
using SocketErrorType = int;
#endif

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

enum class SocketState : SocketHandleType
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
 * @tparam Derived
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
    m_last_error(0)
    {

    }

    /**
     * @brief Destructor of Socket.
     */
    ~Socket() noexcept
    {

    }

    /**
     * @brief Returns the socket handle for send and receive.
     * @return the socket handle
     */
    SocketHandleType get_socket_handle() const noexcept
    {
        return m_socket;
    }

protected:

    /**
     * @brief The socket handle for the derived classes as reference.
     * @return the socket handle as reference to write and read
     */
    SocketHandleType& get_socket_handle() noexcept
    {
        return m_socket;
    }

    /**
     * @brief A socket will be created.
     * @return true if the socket creation was successful, false if not.
     */
    bool create() noexcept
    {
        // CRTP
        return static_cast< Derived* >(this)->create();
    }

    //! this attribute shows if it's a socket for can, ethernet tcp/ip or udp
    SocketType m_type;

    //! stores the last error in this attribute
    SocketErrorType m_last_error;
private:

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
