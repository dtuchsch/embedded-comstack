/**
 * @file      Packet.h
 * @author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief     Packet class for network streams
 * @details   This is inspired by SFML Packet, but is also capable for embedded.
 *            Instead of using std::vector as a container we use a fixed size
 *            container std::array.
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

#ifndef PACKET_H_
# define PACKET_H_

/*******************************************************************************
 * MODULES USED
 *******************************************************************************/

#include <array>
#include <cstring>

#include "Std_Types.h"
#include "Endianess.h"

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

/**
 * @brief Packet class for unified data (network) transport.
 * @tparam Size of the container
 */
template< std::size_t Size >
class Packet
{
public:

    using DataContainer = std::array< uint8, Size >;

    /**
     * @brief Default constructor initializes the write and read position.
     */
    Packet() noexcept :
    m_write_pos(0U),
    m_read_pos(0U)
    {
        static_assert(Size > 0, "Size must be greater than zero!");
    }

    /**
     * @brief Default destructor
     */
    ~Packet() noexcept
    {

    }

    /**
     * @brief returns the static size of the packet's data to send or receive.
     * @return the static size.
     */
    constexpr uint16 get_size() const noexcept
    {
        return static_cast< uint16 >(m_data.size());
    }

    const DataContainer& get_data() const noexcept
    {
        return m_data;
    }

    void clear() noexcept
    {
        m_write_pos = 0U;
        m_read_pos = 0U;
    }

    /**
     * @brief Checks if the length of bytes to write is possible.
     * @param[in] bytes_to_write is the number of bytes to store in the Packet.
     * @return true if there is enough space in the container to store the
     * bytes, false if the bytes would exceed the Packet.
     */
    bool is_writable(const std::size_t bytes_to_write) const noexcept
    {
        const auto write_pos_new = m_write_pos + bytes_to_write;
        return (write_pos_new <= m_data.size()) && (bytes_to_write > 0);
    }

    /**
     * @brief Checks if there are bytes left to read from the Packet.
     * @param[in] bytes_to_read the number of bytes to read.
     * @return true if there are bytes left to read or false if the end of
     * the Packet is reached.
     */
    bool is_readable(const std::size_t bytes_to_read) const noexcept
    {
        const auto read_pos_new = m_read_pos + bytes_to_read;
        return (read_pos_new <= m_data.size()) && (bytes_to_read > 0);
    }

    /**
     * @brief Appends data to the container.
     * @tparam T is the type of data to store to determine the number of bytes
     * to write into the container.
     * @param[in] data is the data to store storing to the end of the current
     * write position. This data will be copied, but not modified.
     */
    template< typename T >
    void append(const T& data) noexcept
    {
        static constexpr uint8 bytes_to_write = sizeof(T);

        if ( is_writable(bytes_to_write) )
        {
            const uint8* data_ptr = reinterpret_cast< const uint8* >(&data);
            std::memcpy(&m_data[m_write_pos], data_ptr, bytes_to_write);
            m_write_pos += bytes_to_write;
        }
    }

    /**
     * @brief Appends the container with a char array.
     * @param[in] data the char array.
     */
    void append(const char* data) noexcept
    {
        // determine the length of the char array
        uint32 bytes_to_write = static_cast< uint32 >(std::strlen(data));

        // store the length first
        *this << bytes_to_write;

        if ( is_writable(bytes_to_write) )
        {
            std::memcpy(&m_data[m_write_pos], data, bytes_to_write);
            m_write_pos += bytes_to_write;
        }
    }

    /**
     * @brief Extract a boolean from this packet to host byte order.
     * @param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * @return the current packet object.
     */
    Packet& operator >>(bool& data) noexcept
    {
        static constexpr uint8 bytes_to_read = sizeof(uint8);

        uint8 bool_as_num = 0U;
        *this >> bool_as_num;

        if ( bool_as_num == 0U )
        {
            data = false;
        }
        else
        {
            data = true;
        }

        return *this;
    }

    /**
     * @brief Extract an unsigned byte from this packet to host byte order.
     * @param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * @return the packet object
     */
    Packet& operator >>(uint8& data) noexcept
    {
        static constexpr uint8 bytes_to_read = sizeof(uint8);

        if ( is_readable(bytes_to_read) )
        {
            data = m_data[m_read_pos];
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * @brief Extract a signed byte from this packet to host byte order.
     * @param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * @return the packet object
     */
    Packet& operator >>(sint8& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(sint8);

        if ( is_readable(bytes_to_read) )
        {
            data = m_data[m_read_pos];
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * @brief Extract an unsigned word from this packet to host byte order.
     * @param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * @return the packet object
     */
    Packet& operator >>(uint16& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(uint16);

        if ( is_readable(bytes_to_read) )
        {
            const uint16* data_ptr = reinterpret_cast< const uint16* >(&m_data[m_read_pos]);
            data = from_network< uint16 >(*data_ptr);
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * @brief Extract a singed word from this packet to host byte order.
     * @param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * @return the packet object
     */
    Packet& operator >>(sint16& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(sint16);

        if ( is_readable(bytes_to_read) )
        {
            sint16* data_ptr = reinterpret_cast< sint16* >(&m_data[m_read_pos]);
            data = from_network< sint16 >(*data_ptr);
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * @brief Extract an unsigned double word from this
     * packet to host byte order.
     * @param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * @return the packet object
     */
    Packet& operator >>(uint32& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(uint32);

        if ( is_readable(bytes_to_read) )
        {
            uint32* data_ptr = reinterpret_cast< uint32* >(&m_data[m_read_pos]);
            data = from_network< uint32 >(*data_ptr);
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * @brief Extract a signed double word from this packet to host byte order.
     * @param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * @return the packet object
     */
    Packet& operator >>(sint32& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(sint32);

        if ( is_readable(bytes_to_read) )
        {
            sint32* data_ptr = reinterpret_cast< sint32* >(&m_data[m_read_pos]);
            data = from_network< sint32 >(*data_ptr);
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * @brief Extract an unsigned quad word from this packet to host byte order.
     * @param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * @return the packet object
     */
    Packet& operator >>(uint64& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(uint64);

        if ( is_readable(bytes_to_read) )
        {
            uint64* data_ptr = reinterpret_cast< uint64* >(&m_data[m_read_pos]);
            data = from_network< uint64 >(*data_ptr);
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * @brief Extract a signed quad word from this packet to host byte order.
     * @param[out] data is a reference to a variable where the data from the
     * packet is stored.
     * @return the packet object.
     */
    Packet& operator >>(sint64& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(sint64);

        if ( is_readable(bytes_to_read) )
        {
            sint64* data_ptr = reinterpret_cast< sint64* >(&m_data[m_read_pos]);
            data = from_network< sint64 >(*data_ptr);
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * @brief Extract a floating point of 32 bits from this packet
     * to host byte order.
     * @param[out] data the variable where to store the float32 in.
     * @return this object.
     */
    Packet& operator >>(float32& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(float32);

        if ( is_readable(bytes_to_read) )
        {
            uint32 f_as_num = *(reinterpret_cast< uint32* >(&m_data[m_read_pos]));
            f_as_num = from_network< uint32 >(f_as_num);
            std::memcpy(&data, &f_as_num, bytes_to_read);

            // update the read position
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * @brief Extract a floating point from this packet to host byte order.
     * @param[out] data the variable where to store the float64 in.
     */
    Packet& operator >>(float64& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(float64);

        if ( is_readable(bytes_to_read) )
        {
            uint64 f_as_num = *(reinterpret_cast< uint64* >(&m_data[m_read_pos]));
            f_as_num = from_network< uint64 >(f_as_num);
            std::memcpy(&data, &f_as_num, bytes_to_read);
            m_read_pos += bytes_to_read;    // update the read position
        }

        return *this;
    }

    /**
     * @brief Extract a char array from this packet.
     * @param[out] data the array were to store the C-array in.
     */
    Packet& operator >>(char* data) noexcept
    {
        uint32 bytes_to_read = 0U;
        *this >> bytes_to_read;

        if ( is_readable(bytes_to_read) )
        {
            std::memcpy(data, &m_data[m_read_pos], bytes_to_read);

            // we need to add the char terminator
            data[bytes_to_read] = '\0';

            // update reading position
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * @brief Store a unsigned int of 8 bytes in this packet in network
     * byte order.
     * @param[in] data the unsigned byte to store in the packet.
     */
    Packet& operator <<(uint8& data) noexcept
    {
        append< uint8 >(data);
        return *this;
    }

    /**
     * @brief Store a signed int of 8 bytes in this packet in network
     * byte order.
     * @param[in] data is the rhs and the signed byte to store in the
     * packet.
     */
    Packet& operator <<(sint8& data) noexcept
    {
        append< sint8 >(data);
        return *this;
    }

    /**
     * @brief Store a boolean in this packet in network
     * byte order.
     * @param[in] data is the rhs and the boolean to store in the
     * packet.
     * @return the packet object.
     */
    Packet& operator <<(bool& data) noexcept
    {
        // forwards to the uint8 operator
        if ( data == true )
        {
            uint8 num_as_bool = 1U;
            *this << num_as_bool;
        }
        else
        {
            uint8 num_as_bool = 0U;
            *this << num_as_bool;
        }

        return *this;
    }

    /**
     * @brief Store a unsigned int of 16 bytes in this packet in network
     * byte order.
     * @param[in] data is the rhs and the unsigned word to store in the
     * packet.
     */
    Packet& operator <<(uint16& data) noexcept
    {
        uint16 network_data = to_network< uint16 >(data);
        append< uint16 >(network_data);
        return *this;
    }

    /**
     * @brief Store a signed int of 16 bytes in this packet in network
     * byte order.
     * @param[in] data is the rhs and the signed word to store in the
     * packet.
     */
    Packet& operator <<(sint16& data) noexcept
    {
        sint16 network_data = to_network< sint16 >(data);
        append< sint16 >(network_data);
        return *this;
    }

    /**
     * @brief Store a unsigned int of 32 bytes in this packet in network
     * byte order.
     * @param[in] data is the rhs and the signed double word to store in the
     * packet.
     */
    Packet& operator <<(uint32& data) noexcept
    {
        uint32 network_data = to_network< uint32 >(data);
        append< uint32 >(network_data);
        return *this;
    }

    /**
     * @brief Store a signed int of 32 bytes in this packet in network
     * byte order.
     * @param[in] data the signed double word to store in the packet.
     */
    Packet& operator <<(sint32& data) noexcept
    {
        sint32 network_data = to_network< sint32 >(data);
        append< uint32 >(network_data);
        return *this;
    }

    /**
     * @brief Store a unsigned int of 64 bytes in this packet in network
     * byte order.
     * @param[in] the unsigned quad word to store in the packet.
     */
    Packet& operator <<(uint64& data) noexcept
    {
        uint64 network_data = to_network< uint64 >(data);
        append< uint64 >(network_data);
        return *this;
    }

    /**
     * @brief Store a unsigned int of 64 bytes in this packet in network
     * byte order.
     * @param[in] data the signed quad word to store in the packet.
     */
    Packet& operator <<(sint64& data) noexcept
    {
        sint64 network_data = to_network< sint64 >(data);
        append< sint64 >(network_data);
        return *this;
    }

    /**
     * @brief Store a floating point of 32 bytes in this packet in network
     * byte order.
     * @param[in] data the float to store.
     */
    Packet& operator <<(float32& data) noexcept
    {
        // first we will convert it to an equivalent byte representation.
        uint32 num_as_float = *(reinterpret_cast< uint32* >(&data));
        uint32 network_data = to_network< uint32 >(num_as_float);
        append< uint32 >(network_data);
        return *this;
    }

    /**
     * @brief Store a floating point of 64 bytes in this packet in network
     * byte order.
     * @param[in] data the double to store
     */
    Packet& operator <<(float64& data) noexcept
    {
        // first we will convert it to an equivalent byte representation.
        uint64 num_as_float = *(reinterpret_cast< uint64* >(&data));
        uint64 network_data = to_network< uint64 >(num_as_float);
        append< uint64 >(network_data);
        return *this;
    }

    /**
     * @brief Store a char array in this packet in network
     * byte order.
     * @param[in] data the C-string to store in the packet
     */
    Packet& operator <<(const char* data) noexcept
    {
        append(data);
        return *this;
    }

private:

    //! This is the container that holds the data stored via << operator.
    DataContainer m_data;

    //! Current position where data is appended in the packet.
    uint32 m_write_pos;

    //! current position where data is read from, until this packet is completely
    //! read.
    uint32 m_read_pos;
};

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

#endif /* PACKET_H_ */
