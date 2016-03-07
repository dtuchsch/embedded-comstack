/**
 * @file 	  Std_Types.h
 * @author 	  dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief	  Standard Types interface
 * @details   According to Specification of Standard Types AUTOSAR 4.0
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

#ifndef STD_TYPES_H
# define STD_TYPES_H

/*******************************************************************************
 * MODULES USED
 *******************************************************************************/

/* Std_Types.h shall include Platform_Types.h */
# include "Platform_Types.h"

/* Std_Types.h shall include Compiler.h */
# include "Compiler.h"

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

/*
 * Every BSW Service API called by application software
 * via RTE shall return a Std_ReturnType, return value.
 */
# ifndef STATUSTYPEDEFINED
#  define STATUSTYPEDEFINED
#  define E_OK (0x00U)

typedef unsigned char StatusType;
# endif

# define E_NOT_OK (0x01U)

# define STD_HIGH (0x01) /** Physical state 5V or 3.3V */
# define STD_LOW (0x00)  /** Physical state 0V */

# define STD_ACTIVE (0x01) /** Logical state active */
# define STD_IDLE (0x00) /** Logical state idle */

/*
 * Compiler switches shall be compared with defined values.
 * Simple checks if a compiler switch is defined shall not be used.
 * #if ( EEP_DEV_ERROR_DETECT == STD_ON ) ..
 */
# define STD_ON (0x01)
# define STD_OFF (0x00)

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

/**
 * This type can be used as standard API return type which is shared
 * between the RTE and the BSW modules.
 */
typedef uint8 Std_ReturnType;

/**
 * @brief This type shall be used to request the version of a
 * BSW module using the <Module name>_GetVersionInfo() function.
 */
typedef struct
{
    uint16 vendorID;
    uint16 moduleID;
    uint8 sw_major_version;
    uint8 sw_minor_version;
    uint8 sw_patch_version;
} Std_VersionInfoType;

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

# ifdef __cplusplus
#  define STD_VERSION_CHECK(MODULE,\
            SW_MAJOR_VERSION,\
            SW_MINOR_VERSION,\
            SW_PATCH_VERSION)\
static_assert(MODULE ## _SW_MAJOR_VERSION  == SW_MAJOR_VERSION,\
"Module major version is not compatible with interface");\
static_assert(MODULE ## _SW_MINOR_VERSION == SW_MINOR_VERSION,\
"Module minor version is not compatible with interface");\
static_assert(MODULE ## _SW_PATCH_VERSION == SW_PATCH_VERSION,\
"Module patch version is not compatible with interface")

# else

# endif

#endif /* STD_TYPES_H */
