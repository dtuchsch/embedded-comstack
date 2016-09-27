/**
 * @file 	  Platform_Types.h
 * @author 	  dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief	  Platform specific types mapping.
 * @details   The AUTOSAR platform types are platform and compiler dependent.
 *            Because the size of some arithmetic types may differ from platform
 *            to platform this header provides platform independent types of 
 *            fixed size. Since C++11 you may also use fixed width integer types
 *            from the STL directly by including the header <cstdint>.
 * @edit      27.09.2016
 * @copyright Copyright (c) 2015, Daniel Tuchscherer.
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

#ifndef PLATFORM_TYPES_H_
# define PLATFORM_TYPES_H_

/*******************************************************************************
 * MODULES USED
 *******************************************************************************/
# ifdef __cplusplus
#  include <cstdint>
#  include <type_traits>
# else
#  include <stdint.h>
# endif

//! all type-related things will be put into a namespace called AR for AUTOSAR.
//! this avoids overlaps to other type definitions done in third party headers used.
# ifdef __cplusplus
namespace AR
{
# endif
/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/
//! Indicating a 8 bit processor
# define CPU_TYPE_8         (8)

//! Indicating a 16 bit processor
# define CPU_TYPE_16        (16)

//! Indicating a 32 bit processor
# define CPU_TYPE_32        (32)

//! Indicating a 64 bit processor
# define CPU_TYPE_64        (64)

//! The most significant bit is the first bit of the bit sequence.
# define MSB_FIRST          (0)

//! The least significant bit is the first bit of the bit sequence.
# define LSB_FIRST          (1)

//! Within uint16, the high byte is located before the low byte.
# define HIGH_BYTE_FIRST    (0)

//! Within uint16, the low byte is located before the high byte.
# define LOW_BYTE_FIRST     (1)

//! @req SWS_Platform_00064 This symbol shall be defined as #define having one 
//! of the values CPU_TYPE_8, CPU_TYPE_16 or CPU_TYPE_32 according to the platform.
# define CPU_TYPE           CPU_TYPE_32

//! @req SWS_Platform_00038 This symbol shall be defined as #define having one 
//! of the values MSB_FIRST or LSB_FIRST according to the platform.
# define CPU_BIT_ORDER      LSB_FIRST

//! @req SWS_Platform_00039 This symbol shall be defined as #define having one 
//! of the values HIGH_BYTE_FIRST or LOW_BYTE_FIRST according to the platform.
# define CPU_BYTE_ORDER     LOW_BYTE_FIRST

# ifndef TRUE
#  ifdef __cplusplus
#   define TRUE              true
#  else
// in the C programming language there is no type for boolean values.
#   define TRUE              (1)
#  endif
# endif

# ifndef FALSE
#  ifdef __cplusplus
#   define FALSE            false
#  else
// in the C programming language there is no type for boolean values.
#   define FALSE            (0)
#  endif
# endif

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES											   *
 *******************************************************************************/
 
//! @req SWS_Platform_00026 This standard AUTOSAR type shall only be used 
//! together with the definitions TRUE and FALSE.
# ifdef __cplusplus
// in C++ there is a boolean type.
typedef bool                boolean;
# else
// in C there is no bool type. we use a byte to represent true or false with 1 and
// 0 respectively.
typedef std::int8_t         boolean;
# endif

//! @req SWS_Platform_00013 This standard AUTOSAR type shall be of 8 bit unsigned.
typedef std::uint8_t        uint8;

//! @req SWS_Platform_00016 This standard AUTOSAR type shall be of 8 bit signed.
typedef std::int8_t         sint8;

//! @req SWS_Platform_00014 This standard AUTOSAR type shall be of 16 bit unsigned.
typedef std::uint16_t       uint16;

//! @req SWS_Platform_00017 This standard AUTOSAR type shall be of 16 bit signed.
typedef std::int16_t        sint16;

//! @req SWS_Platform_00015 This standard AUTOSAR type shall be 32 bit unsigned.
typedef std::uint32_t       uint32;

//! @req SWS_Platform_00018 This standard AUTOSAR type shall be 32 bit signed.
typedef std::int32_t        sint32;

//! @req SWS_Platform_00066 This standard AUTOSAR type shall be 64 bit unsigned.
typedef std::uint64_t       uint64;

//! @req SWS_Platform_00067 This standard AUTOSAR type shall be 64 bit signed.
typedef std::int64_t        sint64;

//! @req SWS_Platform_00020 This optimized AUTOSAR type shall be at least 8 bit unsigned.
typedef std::uint_least8_t  uint8_least;

//! This optimized type shall be at least 16 bit unsigned.
typedef std::uint_least16_t uint16_least;

//! This optimized type shall be at least 32 bit unsigned.
typedef std::uint_least32_t uint32_least;

//! @req SWS_Platform_00041 This standard AUTOSAR type shall be at least 32 bit float.
typedef float               float32;

//! @req SWS_Platform_00042 This standard AUTOSAR type shall be at least 64 bit float.
typedef double              float64;

//! We will directly check if the size of all types match.
//! Otherwise it will throw a compile-error. if the types do not match and this
//! is not checked it may lead to undefined behavior and some cruel errors.
#ifdef __cplusplus
static_assert(sizeof(uint8) == 1U, "SWS_Platform_00013 This standard AUTOSAR type shall be of 8 bit unsigned.");
static_assert(sizeof(sint8) == 1U, "SWS_Platform_00013 This standard AUTOSAR type shall be of 8 bit unsigned.");
static_assert(sizeof(uint16) == 2U, "SWS_Platform_00013 This standard AUTOSAR type shall be of 8 bit unsigned.");
static_assert(sizeof(sint16) == 2U, "SWS_Platform_00013 This standard AUTOSAR type shall be of 8 bit unsigned.");
static_assert(sizeof(uint32) == 4U, "SWS_Platform_00013 This standard AUTOSAR type shall be of 8 bit unsigned.");
static_assert(sizeof(sint32) == 4U, "SWS_Platform_00013 This standard AUTOSAR type shall be of 8 bit unsigned.");
static_assert(sizeof(uint64) == 8U, "SWS_Platform_00013 This standard AUTOSAR type shall be of 8 bit unsigned.");
static_assert(sizeof(sint64) == 8U, "SWS_Platform_00013 This standard AUTOSAR type shall be of 8 bit unsigned.");
static_assert(sizeof(float32) == 4U, "SWS_Platform_00041 This standard AUTOSAR type shall be at least 32 bit float.");
static_assert(sizeof(float64) == 8U, "SWS_Platform_00042 This standard AUTOSAR type shall be at least 64 bit float.");
static_assert(std::is_unsigned< uint8 >::value == true, "uint8 is not unsigned as it should be!");
static_assert(std::is_signed< sint8 >::value == true, "sint8 is not signed as it should be!");
static_assert(std::is_unsigned< uint16 >::value == true, "uint16 is not unsigned as it should be!");
static_assert(std::is_signed< sint16 >::value == true, "sint16 is not signed as it should be!");
static_assert(std::is_unsigned< uint32 >::value == true, "uint32 is not unsigned as it should be!");
static_assert(std::is_signed< sint32 >::value == true, "sint32 is not signed as it should be!");
static_assert(std::is_unsigned< uint64 >::value == true, "uint64 is not unsigned as it should be!");
static_assert(std::is_signed< sint64 >::value == true, "sint64 is not unsigned as it should be!");
static_assert(std::is_floating_point< float32 >::value == true, "float32 is not a floating point type as specified by AUTOSAR.");
static_assert(std::is_floating_point< float64 >::value == true, "float64 is not a floating point type as specified.");
#endif

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

# ifdef __cplusplus
} /* namespace AR */
# endif

#endif /* PLATFORM_TYPES_H_ */
