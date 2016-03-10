/**
 * @file      IpAddress.h
 * @author    dtuchscherer <your.email@hs-heilbronn.de>
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

#ifndef IPADDRESS_H_
# define IPADDRESS_H_

/*******************************************************************************
 * MODULES USED
 *******************************************************************************/
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Socket.h"
#include "ComStack_Types.h"

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

/**
 * @brief IpAddress class interface for interpreting ip addresses as string.
 */
class IpAddress
{
public:
    /**
     * @brief Construct the address from a C-style string
     * @param[in] ip_address the IP4 address as C-style string e.g. "192.168.3.11"
     */
    IpAddress(const char* ip_address) noexcept;

    /**
     * Default destructor
     */
    ~IpAddress() noexcept;

    /**
     * @brief get the ip address for socket communication.
     * @return the ip address in host-byte-order
     */
    uint32 get_ip_address() const noexcept;

    /**
     * @brief
     * @param[in] ip_host_byte_order the IP4 address in host-byte-order
     * @param[in] port the port to connect to
     * @param[out] addr the structure to store the ip info to.
     */
    void create_address_struct(const uint32 ip_host_byte_order,
                                  const uint16 port, sockaddr_in& addr)
                                          noexcept;

private:

    /**
     * @brief Try to build a byte representation out of the address given as
     * string.
     * @param[in] ip the IP4 address as C-style string
     * @return true if the ip address is valid, false if not.
     */
    boolean is_valid(const char* ip) noexcept;

    //! the address in network-byte-order
    uint32 m_address_binary;

    //! if the configuration of the given ip was successful or not.
    boolean m_valid_ip;
};

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

#endif /* IPADDRESS_H_ */
