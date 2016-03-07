/**
 * @file      Endianess.h
 * @author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief     short description...
 * @details   long description...
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

#ifndef ENDIANESS_H_
# define ENDIANESS_H_

/*******************************************************************************
 * MODULES USED
 *******************************************************************************/
#ifdef __unix__
#include <endian.h>
#else
#error "Please define BYTE_ORDER of your system architecture."
#endif

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

enum class EndianType
{
    EndianType_little,
    EndianType_big,
    EndianType_network = EndianType_big,
    EndianType_intel = EndianType_little,
    EndianType_motorola = EndianType_big
};

template< typename T, size_t Sz >
T swap_bytes(const T& val) noexcept
{
    return val;
}

template< >
inline uint8 swap_bytes< uint8, 1 >(const uint8& val) noexcept
{
    return val;
}

template< >
inline uint16 swap_bytes< uint16, 2 >(const uint16& val) noexcept
{
    uint16 temp = 0U;
    temp  = ((val >> 8U) & 0x00FFU);
    temp |= ((val << 8U) & 0xFFFFU);
    return temp;
}

template< >
inline sint16 swap_bytes< sint16, 2 >(const sint16& val) noexcept
{
    sint16 temp = 0;
    temp  = ((val >> 8) & 0x00FF);
    temp |= ((val << 8) & 0xFFFF);
    return temp;
}

template< >
inline uint32 swap_bytes< uint32, 4 >(const uint32& val) noexcept
{
    uint32 temp = 0U;
    temp  = ((val >> 24U) & 0x000000FFUL); // byte 3 to 0
    temp |= ((val << 24U) & 0xFF000000UL); // byte 0 to 3
    temp |= ((val >> 8U)  & 0x0000FF00UL); // byte 2 to 1
    temp |= ((val << 8U)  & 0x00FF0000UL); // byte 1 to 2
    return temp;
}

template< >
inline uint64 swap_bytes< uint64, 8 >(const uint64& val) noexcept
{
    uint64 temp = 0U;
    temp  = ((val >> 56U) & 0x00000000000000FFULL); // byte 7 to 0
    temp |= ((val << 56U) & 0xFF00000000000000ULL); // byte 0 to 7
    temp |= ((val >> 40U) & 0x000000000000FF00ULL); // byte 6 to 1
    temp |= ((val << 40U) & 0x00FF000000000000ULL); // byte 1 to 6
    temp |= ((val >> 24U) & 0x0000000000FF0000ULL); // byte 5 to 2
    temp |= ((val << 24U) & 0x0000FF0000000000ULL); // byte 2 to 5
    temp |= ((val >> 8U)  & 0x00000000FF000000ULL); // byte 4 to 3
    temp |= ((val << 8U)  & 0x000000FF00000000ULL); // byte 3 to 4
    return temp;
}

template< EndianType To, EndianType From, typename T >
inline T swap(const T& value) noexcept
{
    static_assert((To != From), "Must be of different types.");
    return swap_bytes< T, sizeof(T) >(value);
}

template< typename ToType, typename FromType >
ToType convert(ToType& to, FromType& from)
{
    static_assert(sizeof(ToType) == sizeof(FromType), "Size of both types must match.");

    memcpy(&to, &from, sizeof(ToType));

    return to;
}

template< typename T >
inline T to_network(const T& value)
{
    T convert = value;
#ifdef __unix__
#if ( BYTE_ORDER == LITTLE_ENDIAN )
    convert = swap_bytes< T, sizeof(T) >(value);
#endif
#endif
    return convert;
}

template< typename T >
inline T from_network(const T& value)
{
    T convert = value;
#ifdef __unix__
#if ( BYTE_ORDER == LITTLE_ENDIAN )
    convert = swap_bytes< T, sizeof(T) >(value);
#endif
#endif
    return convert;
}

#endif /* ENDIANESS_H_ */
