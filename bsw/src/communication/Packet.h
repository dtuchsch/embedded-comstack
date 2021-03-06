/**
 * \file      Packet.h
 * \author    dtuchscherer <daniel.tuchscherer@gmail.com>
 * \brief     Packet class for network streams
 * \details   This is inspired by SFML Packet, but is also capable for embedded.
 *            Instead of using std::vector as a container we use a fixed size
 *            container std::array.
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

#ifndef PACKET_H_
#define PACKET_H_

#include "Endianess.h" // converting to and from host-byte-order
#include <array>
#include <cstring>

/**
 * \brief Packet class for unified data (network) transport.
 * \tparam Size of the container
 */
template < std::size_t Size > class Packet
{
  public:
    using DataContainer = std::array< uint8, Size >;

    /**
     * \brief Default constructor initializes the write and read position.
     */
    Packet() noexcept : m_write_pos{0U}, m_read_pos{0U}
    {
        static_assert(Size > 0, "Size must be greater than zero!");
    }

    /**
     * \brief Default destructor
     * Nothing to clean up here.
     */
    ~Packet() noexcept = default;

    /**
     * \brief returns the static size of the packet's data to send or receive.
     * \return the static size.
     */
    constexpr std::uint16_t get_size() const noexcept
    {
        return static_cast< std::uint16_t >(m_data.size());
    }

    /**
     * \brief direct link to the data.
     */
    const DataContainer& get_data() const noexcept { return m_data; }

    /**
     * \brief direct link to the data.
     */
    DataContainer& get_data() noexcept { return m_data; }

    /**
     * \brief the complete packet as a constant reference.
     */
    const Packet& get_packet() const noexcept { return *this; }

    Packet& get_packet() noexcept { return *this; }

    /**
     * \brief Clearing the indices for a new storage.
     */
    void clear() noexcept
    {
        m_write_pos = 0U;
        m_read_pos = 0U;
    }

    /**
     * \brief This method is extremly helpful for returning a type T variable
     * from the packet without modifying it from a given position.
     * \tparam T the value type to return
     * \tparam Position the position at what the value begins.
     */
    template < typename T, std::size_t Position > T peek() const noexcept
    {
        static_assert(std::is_arithmetic< T >::value,
                      "Type must be integral or floating point.");
        static_assert(
            Position < Size,
            "The position to read is greater than the actual Packet size.");
        using MyType = T;
        MyType data{static_cast< MyType >(0)};
        const MyType* data_ptr =
            reinterpret_cast< const MyType* >(&m_data[Position]);
        data = from_network< MyType >(*data_ptr);
        return data;
    }

    /**
     * \brief This will store a value of template parameter T at the given
     * position into the packet with network-byte-order.
     */
    template < typename T, std::size_t Position >
    void store(const T& data) noexcept
    {
        static_assert(std::is_arithmetic< T >::value,
                      "Type must be integral or floating point.");
        static_assert(
            Position < Size,
            "The position to write is greater than the actual Packet size.");
        using MyType = T;
        static constexpr auto bytes = sizeof(T);

        // swap to network-byte-order
        MyType network_data = to_network< MyType >(data);
        // we need it byte by byte.
        const std::uint8_t* data_ptr =
            reinterpret_cast< const std::uint8_t* >(&network_data);
        std::memcpy(&m_data[Position], data_ptr, bytes);
    }

    /**
     * \brief Checks if the length of bytes to write is possible.
     * \param[in] bytes_to_write is the number of bytes to store in the Packet.
     * \return true if there is enough space in the container to store the
     * bytes, false if the bytes would exceed the Packet.
     */
    bool is_writable(const std::size_t bytes_to_write) const noexcept
    {
        const auto write_pos_new = m_write_pos + bytes_to_write;
        return (write_pos_new <= m_data.size()) && (bytes_to_write > 0);
    }

    /**
     * \brief Checks if there are bytes left to read from the Packet.
     * \param[in] bytes_to_read the number of bytes to read.
     * \return true if there are bytes left to read or false if the end of
     * the Packet is reached.
     */
    bool is_readable(const std::size_t bytes_to_read) const noexcept
    {
        const auto read_pos_new = m_read_pos + bytes_to_read;
        return (read_pos_new <= m_data.size()) && (bytes_to_read > 0);
    }

    /**
     * \brief Appends data to the container.
     * \tparam T is the type of data to store to determine the number of bytes
     * to write into the container.
     * \param[in] data is the data to store storing to the end of the current
     * write position. This data will be copied, but not modified.
     */
    template < typename T > void append(const T& data) noexcept
    {
        static constexpr uint8 bytes_to_write = sizeof(T);

        if (is_writable(bytes_to_write))
        {
            const std::uint8_t* data_ptr =
                reinterpret_cast< const std::uint8_t* >(&data);
            std::memcpy(&m_data[m_write_pos], data_ptr, bytes_to_write);
            m_write_pos += bytes_to_write;
        }
    }

    /**
     * \brief Appends the container with a char array.
     * \param[in] data the char array.
     */
    void append(const char* data) noexcept
    {
        // determine the length of the char array
        std::uint32_t bytes_to_write =
            static_cast< std::uint32_t >(std::strlen(data));

        // store the length first
        *this << bytes_to_write;

        if (is_writable(bytes_to_write))
        {
            std::memcpy(&m_data[m_write_pos], data, bytes_to_write);
            m_write_pos += bytes_to_write;
        }
    }

    /**
     * \brief Skipping the following bytes incrementing the read position.
     * \param[in] bytes to skip
     */
    bool skip(const std::size_t bytes) noexcept
    {
        bool skipped = false;

        if (is_readable(bytes) == true)
        {
            m_read_pos += bytes;
            skipped = true;
        }

        return skipped;
    }

    /**
     * \brief Extract a bool from this packet to host byte order.
     * \param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * \return the current packet object.
     */
    Packet& operator>>(bool& data) noexcept
    {
        static constexpr uint8 bytes_to_read = sizeof(std::uint8_t);
        std::uint8_t bool_as_num = 0U;
        *this >> bool_as_num;

        if (bool_as_num == 0U)
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
     * \brief Extract an unsigned byte from this packet to host byte order.
     * \param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * \return the packet object
     */
    Packet& operator>>(std::uint8_t& data) noexcept
    {
        static constexpr uint8 bytes_to_read = sizeof(uint8);

        if (is_readable(bytes_to_read))
        {
            data = m_data[m_read_pos];
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * \brief Extract a signed byte from this packet to host byte order.
     * \param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * \return the packet object
     */
    Packet& operator>>(std::int8_t& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(std::int8_t);

        if (is_readable(bytes_to_read))
        {
            data = m_data[m_read_pos];
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * \brief Extract an unsigned word from this packet to host byte order.
     * \param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * \return the packet object
     */
    Packet& operator>>(std::uint16_t& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(std::uint16_t);

        if (is_readable(bytes_to_read))
        {
            const std::uint16_t* data_ptr =
                reinterpret_cast< const std::uint16_t* >(&m_data[m_read_pos]);
            data = from_network< std::uint16_t >(*data_ptr);
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * \brief Extract a singed word from this packet to host byte order.
     * \param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * \return the packet object
     */
    Packet& operator>>(std::int16_t& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(std::int16_t);

        if (is_readable(bytes_to_read))
        {
            const std::int16_t* data_ptr =
                reinterpret_cast< const std::int16_t* >(&m_data[m_read_pos]);
            data = from_network< sint16 >(*data_ptr);
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * \brief Extract an unsigned double word from this
     * packet to host byte order.
     * \param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * \return the packet object
     */
    Packet& operator>>(std::uint32_t& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(std::uint32_t);

        if (is_readable(bytes_to_read))
        {
            const std::uint32_t* data_ptr =
                reinterpret_cast< const std::uint32_t* >(&m_data[m_read_pos]);
            data = from_network< uint32 >(*data_ptr);
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * \brief Extract a signed double word from this packet to host byte order.
     * \param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * \return the packet object
     */
    Packet& operator>>(std::int32_t& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(std::int32_t);

        if (is_readable(bytes_to_read))
        {
            const std::int32_t* data_ptr =
                reinterpret_cast< const std::int32_t* >(&m_data[m_read_pos]);
            data = from_network< std::int32_t >(*data_ptr);
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * \brief Extract an unsigned quad word from this packet to host byte order.
     * \param[out] data is a reference to a variable to store the extracted
     * data from the container in.
     * \return the packet object
     */
    Packet& operator>>(std::uint64_t& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(std::uint64_t);

        if (is_readable(bytes_to_read))
        {
            const std::uint64_t* data_ptr =
                reinterpret_cast< const std::uint64_t* >(&m_data[m_read_pos]);
            data = from_network< std::uint64_t >(*data_ptr);
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * \brief Extract a signed quad word from this packet to host byte order.
     * \param[out] data is a reference to a variable where the data from the
     * packet is stored.
     * \return the packet object.
     */
    Packet& operator>>(std::int64_t& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(std::int64_t);

        if (is_readable(bytes_to_read))
        {
            const std::int64_t* data_ptr =
                reinterpret_cast< const std::int64_t* >(&m_data[m_read_pos]);
            data = from_network< std::int64_t >(*data_ptr);
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * \brief Extract a floating point of 32 bits from this packet
     * to host byte order.
     * \param[out] data the variable where to store the float32 in.
     * \return this object.
     */
    Packet& operator>>(float& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(float);

        if (is_readable(bytes_to_read))
        {
            std::uint32_t f_as_num =
                *(reinterpret_cast< std::uint32_t* >(&m_data[m_read_pos]));
            // first swap the bytes and then convert into a floating point
            f_as_num = from_network< std::uint32_t >(f_as_num);
            std::memcpy(&data, &f_as_num, bytes_to_read);
            // update the read position
            m_read_pos += bytes_to_read;
        }

        return *this;
    }

    /**
     * \brief Extract a floating point from this packet to host byte order.
     * \param[out] data the variable where to store the float64 in.
     */
    Packet& operator>>(double& data) noexcept
    {
        static constexpr auto bytes_to_read = sizeof(double);

        if (is_readable(bytes_to_read))
        {
            std::uint64_t f_as_num =
                *(reinterpret_cast< uint64* >(&m_data[m_read_pos]));
            // first swap the bytes and then convert into a floating point
            f_as_num = from_network< std::uint64_t >(f_as_num);
            std::memcpy(&data, &f_as_num, bytes_to_read);
            m_read_pos += bytes_to_read; // update the read position
        }

        return *this;
    }

    /**
     * \brief Extract a char array from this packet.
     * \param[out] data the array were to store the C-array in.
     */
    Packet& operator>>(char* data) noexcept
    {
        uint32 bytes_to_read = 0U;
        *this >> bytes_to_read;

        if (is_readable(bytes_to_read))
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
     * \brief Store a unsigned int of 8 bytes in this packet in network
     * byte order.
     * \param[in] data the unsigned byte to store in the packet.
     */
    Packet& operator<<(std::uint8_t& data) noexcept
    {
        append< std::uint8_t >(data);
        return *this;
    }

    /**
     * \brief Store a signed int of 8 bytes in this packet in network
     * byte order.
     * \param[in] data is the rhs and the signed byte to store in the
     * packet.
     */
    Packet& operator<<(std::int8_t& data) noexcept
    {
        append< std::int8_t >(data);
        return *this;
    }

    /**
     * \brief Store a bool in this packet in network
     * byte order.
     * \param[in] data is the rhs and the bool to store in the
     * packet.
     * \return the packet object.
     */
    Packet& operator<<(bool& data) noexcept
    {
        // forwards to the uint8 operator
        if (data == true)
        {
            std::uint8_t num_as_bool = 1U;
            *this << num_as_bool;
        }
        else
        {
            std::uint8_t num_as_bool = 0U;
            *this << num_as_bool;
        }

        return *this;
    }

    /**
     * \brief Store a unsigned int of 16 bytes in this packet in network
     * byte order.
     * \param[in] data is the rhs and the unsigned word to store in the
     * packet.
     */
    Packet& operator<<(std::uint16_t& data) noexcept
    {
        std::uint16_t network_data = to_network< std::uint16_t >(data);
        append< std::uint16_t >(network_data);
        return *this;
    }

    /**
     * \brief Store a signed int of 16 bytes in this packet in network
     * byte order.
     * \param[in] data is the rhs and the signed word to store in the
     * packet.
     */
    Packet& operator<<(std::int16_t& data) noexcept
    {
        std::int16_t network_data = to_network< std::int16_t >(data);
        append< std::int16_t >(network_data);
        return *this;
    }

    /**
     * \brief Store a unsigned int of 32 bytes in this packet in network
     * byte order.
     * \param[in] data is the rhs and the signed double word to store in the
     * packet.
     */
    Packet& operator<<(std::uint32_t& data) noexcept
    {
        std::uint32_t network_data = to_network< std::uint32_t >(data);
        append< std::uint32_t >(network_data);
        return *this;
    }

    /**
     * \brief Store a signed int of 32 bytes in this packet in network
     * byte order.
     * \param[in] data the signed double word to store in the packet.
     */
    Packet& operator<<(std::int32_t& data) noexcept
    {
        std::int32_t network_data = to_network< std::int32_t >(data);
        append< std::uint32_t >(network_data);
        return *this;
    }

    /**
     * \brief Store a unsigned int of 64 bytes in this packet in network
     * byte order.
     * \param[in] the unsigned quad word to store in the packet.
     */
    Packet& operator<<(std::uint64_t& data) noexcept
    {
        std::uint64_t network_data = to_network< std::uint64_t >(data);
        append< std::uint64_t >(network_data);
        return *this;
    }

    /**
     * \brief Store a unsigned int of 64 bytes in this packet in network
     * byte order.
     * \param[in] data the signed quad word to store in the packet.
     */
    Packet& operator<<(std::int64_t& data) noexcept
    {
        std::int64_t network_data = to_network< std::int64_t >(data);
        append< std::int64_t >(network_data);
        return *this;
    }

    /**
     * \brief Store a floating point of 32 bytes in this packet in network
     * byte order.
     * \param[in] data the float to store.
     */
    Packet& operator<<(float& data) noexcept
    {
        // first we will convert it to an equivalent byte representation.
        std::uint32_t num_as_float =
            *(reinterpret_cast< std::uint32_t* >(&data));
        std::uint32_t network_data = to_network< std::uint32_t >(num_as_float);
        append< std::uint32_t >(network_data);
        return *this;
    }

    /**
     * \brief Store a floating point of 64 bytes in this packet in network
     * byte order.
     * \param[in] data the double to store
     */
    Packet& operator<<(double& data) noexcept
    {
        // first we will convert it to an equivalent byte representation.
        std::uint64_t num_as_float =
            *(reinterpret_cast< std::uint64_t* >(&data));
        std::uint64_t network_data = to_network< std::uint64_t >(num_as_float);
        append< std::uint64_t >(network_data);
        return *this;
    }

    /**
     * \brief Store a char array in this packet in network
     * byte order.
     * \param[in] data the C-string to store in the packet
     */
    Packet& operator<<(const char* data) noexcept
    {
        append(data);
        return *this;
    }

  protected:
  private:
    //! This is the container that holds the data stored via << operator.
    DataContainer m_data;

    //! Current position where data is appended in the packet.
    std::uint32_t m_write_pos;

    //! current position where data is read from, until this packet is
    //! completely read.
    std::uint32_t m_read_pos;
};

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

#endif /* PACKET_H_ */
