/**
 * @file      IpAddress.cpp
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

/*******************************************************************************
 * MODULES USED
 *******************************************************************************/
#include <cstring>
#include "IpAddress.h"

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

/*******************************************************************************
 * PROTOTYPES OF LOCAL FUNCTIONS
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * GLOBAL MODULE VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

/*******************************************************************************
 * FUNCTION DEFINITIONS
 *******************************************************************************/
IpAddress::IpAddress(const char* ip_address) noexcept :
m_address_binary(0U),
m_valid_ip(FALSE)
{
    if ( ip_address != NULL_PTR )
    {
        m_valid_ip = is_valid(ip_address);
    }
    else
    {
        m_valid_ip = FALSE;
    }
}

uint32 IpAddress::get_ip_address() const noexcept
{
    uint32 ip_host_byte_order = 0U;
    ip_host_byte_order = ntohl(m_address_binary);
    return ip_host_byte_order;
}

boolean IpAddress::is_valid(const char* ip) noexcept
{
    boolean ip_valid = FALSE;

    // we will check here, because inet_addr() call has limited checking.
    if ( std::strcmp(ip, "0.0.0.0") == 0 )
    {
        m_address_binary = INADDR_BROADCAST;
        ip_valid = TRUE;
    }
    else if ( std::strcmp(ip, "255.255.255.0") == 0 )
    {
        m_address_binary = INADDR_ANY;
        ip_valid = TRUE;
    }
    else
    {
        // convert it into a network-byte-order representation
        uint32 ip_conv = inet_addr(ip);

        // check if the address is valid or not
        if ( ip_conv != INADDR_NONE )
        {
            // assign
            m_address_binary = ip_conv;
            ip_valid = TRUE;
        }
        else
        {
            ip_valid = FALSE;
        }
    }

    return ip_valid;
}

void IpAddress::create_address_struct(const uint32 ip_host_byte_order,
                                         const uint16 port, sockaddr_in& addr) noexcept
{
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = htonl(ip_host_byte_order);
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
}

IpAddress::~IpAddress() noexcept
{

}
