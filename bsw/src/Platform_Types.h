/**
 * @file 	  Platform_Types.h
 * @author 	  dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief	  Platform specific types mapping.
 * @details   long description...
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
# else
#  include <stdint.h>
# endif

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/
# define CPU_TYPE_8         (8)               /**!< Indicating a 8 bit processor */
# define CPU_TYPE_16        (16)              /**!< Indicating a 16 bit processor */
# define CPU_TYPE_32        (32)              /**!< Indicating a 32 bit processor */

# define MSB_FIRST          (0)               /**!< The most significant bit is the first bit of the bit sequence */
# define LSB_FIRST          (1)               /**!< The least significant bit is the first bit of the bit sequence. */

# define HIGH_BYTE_FIRST    (0)               /**!< Within uint16, the high byte is located before the low byte. */
# define LOW_BYTE_FIRST     (1)               /**!< Within uint16, the low byte is located before the high byte. */

# define CPU_TYPE           CPU_TYPE_32       /**!< @req SWS_Platform_00064 This symbol shall be defined as #define having one of the values CPU_TYPE_8, CPU_TYPE_16 or CPU_TYPE_32 according to the platform  */
# define CPU_BIT_ORDER      LSB_FIRST         /**!< @req SWS_Platform_00038 This symbol shall be defined as #define having one of the values MSB_FIRST or LSB_FIRST according to the platform. */
# define CPU_BYTE_ORDER     LOW_BYTE_FIRST    /**!< @req SWS_Platform_00039 This symbol shall be defined as #define having one of the values HIGH_BYTE_FIRST or LOW_BYTE_FIRST according to the platform. */

# ifndef TRUE
#  ifdef __cplusplus
#   define TRUE              true
#  else
#   define TRUE              (1)
#  endif
# endif

# ifndef FALSE
#  ifdef __cplusplus
#   define FALSE            false
#  else
#   define FALSE            (0)
#  endif
# endif

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES											   *
 *******************************************************************************/
# ifdef __cplusplus
#  ifdef __unix__
typedef bool           boolean;
#  endif
# else
typedef unsigned char  boolean;          /**!< @req SWS_Platform_00026 This standard AUTOSAR type shall only be used together with the definitions TRUE and FALSE */
# endif

typedef unsigned char  uint8;            /**!< @req SWS_Platform_00013 This standard AUTOSAR type shall be of 8 bit unsigned. */
typedef signed char    sint8;            /**!< @req SWS_Platform_00016 This standard AUTOSAR type shall be of 8 bit signed. */
typedef unsigned short uint16;           /**!< @req SWS_Platform_00014 This standard AUTOSAR type shall be of 16 bit unsigned. */
typedef signed short   sint16;           /**!< @req SWS_Platform_00017 This standard AUTOSAR type shall be of 16 bit signed. */
typedef unsigned int   uint32;           /**!< @req SWS_Platform_00015 This standard AUTOSAR type shall be 32 bit unsigned. */
typedef signed int     sint32;           /**!< @req SWS_Platform_00018 This standard AUTOSAR type shall be 32 bit signed. */
typedef unsigned long long uint64;       /**!< @req SWS_Platform_00066 This standard AUTOSAR type shall be 64 bit unsigned. */
typedef signed long long    sint64;      /**!< @req SWS_Platform_00067 This standard AUTOSAR type shall be 64 bit signed. */

typedef unsigned char  uint8_least;      /**!< @req SWS_Platform_00020 This optimized AUTOSAR type shall be at least 8 bit unsigned. */
typedef unsigned short uint16_least;     /**!< This optimized type shall be at least 16 bit unsigned. */
typedef unsigned int   uint32_least;     /**!< This optimized type shall be at least 32 bit unsigned. */

typedef float          float32;          /**!< @req SWS_Platform_00041 This standard AUTOSAR type shall be at least 32 bit float. */
typedef double         float64;          /**!< @req SWS_Platform_00042 This standard AUTOSAR type shall be at least 64 bit float. */

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

#endif /* PLATFORM_TYPES_H_ */
