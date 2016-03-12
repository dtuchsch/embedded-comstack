/**
 * @file      Tcp_Test.cpp
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
#include <iostream>
#include "catch.hpp"
#include "RTTask.h"
#include "TcpServer.h"
#include "TcpClient.h"

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

class ServerTask : public RTTask< ServerTask, 80, 500000 >
{
public:
    ServerTask() noexcept :
    RTTask()
    {

    }

    void pre() noexcept
    {
        m_server.listen(m_client, 4444U);
    }

    void update() noexcept
    {
        m_server.accept();
    }

    void post() noexcept
    {

    }

private:

    TcpServer m_server;
    IpAddress m_client{"localhost"};
};

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
TEST_CASE( "TCP Client connect and disconnect", "[client-connect-send-disconnect]" )
{
    IpAddress local("localhost");
    uint8 send_ascii_zero = 0x30U;
    ServerTask t;

    TcpClient tcp;
    boolean connected = FALSE;

    while ( connected == FALSE )
    {
        connected = tcp.connect(local, 4444U);
    }

    REQUIRE( connected == TRUE );
    REQUIRE( tcp.send(&send_ascii_zero, 1U) == 1 );
    REQUIRE( tcp.disconnect() == TRUE );
}
