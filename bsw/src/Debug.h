/**
 * @file 	  Debug.h
 * @author 	  dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief	  Logging interface
 * @details   This is a logging interface for C and C++.
 * @version   2.0
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

#ifndef INCLUDE_DEBUG_H_
# define INCLUDE_DEBUG_H_
# ifdef DEBUG

/*******************************************************************************
 * MODULES USED															       *
 *******************************************************************************/
#  include <stdio.h>
#  include "Std_Types.h"

/*******************************************************************************
 * DEFINITIONS AND MACROS                                                      *
 *******************************************************************************/
#  define PRINT_CMD  printf

#  define DEBUG_INFO(MSG, ...) \
            PRINT_CMD("[INFO] (%s:%d) " MSG "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#  define DEBUG_WARN(MSG, ...) \
            PRINT_CMD("[WARN] (%s:%d) " MSG " \n", __FILE__, __LINE__, ##__VA_ARGS__)

#  define DEBUG_ERROR(MSG, ...) \
            PRINT_CMD("[ERROR] (%s:%d) " MSG "\n", __FILE__, __LINE__, ##__VA_ARGS__)

# define DEBUG_BYTESTREAM(DATA, LEN) \
            for ( uint16 i = 0U; i < LEN; ++i ) { PRINT_CMD("0x%X ", DATA[i]); } \
            PRINT_CMD("\n")

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES											   *
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED VARIABLES												           *
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS                                                          *
 *******************************************************************************/

# else
#  define DEBUG_INFO(M, ...)
#  define DEBUG_WARN(M, ...)
#  define DEBUG_ERROR(M, ...)
#  define DEBUG_BYTESTREAM(DATA, LEN)
# endif /* If debugging is enabled */
#endif /* INCLUDE_DEBUG_H_ */
