// The example sends data between a TCP/IP client and a TCP/IP server.
// The TCP client will send some sample data the TCP server will receive and 
// then prints out the received message.
// This example examines the use of the classes TCPClient and TCPServer.
// Please note that the given example is for demonstration purposes only. 
// It may contain bugs and unresolved connection errors, but it shows how one 
// can use the TCP client and server.

#include <array>

// this header is included to have access to the EXIT_SUCCESS define.
#include <cstdlib>

// to print out the data that is sent.
#include <iostream>

// to access shared variables without read/write access violations from different
// threads.
#include <mutex>

// this thread library is available since C++11 and works under Windows and Linux.
#include <thread>

// always include Std_Types.h or ComStack_Types.h in case you develop a
// communication relevant application - do not include Platform_Types.h directly !
#include "ComStack_Types.h"

// We need the header to create one TCP client object.
#include "TcpClient.h"

// We need the header to create the TCP server object.
#include "TcpServer.h"

//! this variable is used to manage the thread's lifetime.
static volatile AR::boolean server_running{FALSE};

//! mutex to write shared variables that are used by both threads.
static std::mutex mutex;

////////////////////////////////////////////////////////////////////////////////
void server_thread() noexcept
{
    TcpServer server;
    // listening on 255.255.255.0 and port 5555. The specified IP accepts
    // all incoming connections.
    const auto listening = server.listen("255.255.255.0", 5555U);
    // notify the main thread the server thread has been started successfully.
    // lock the mutex, because the main thread also accesses the variable.
    mutex.lock();
    server_running = TRUE;
    // unlock again otherwise a dead-lock may occur.
    mutex.unlock();
    
    if ( listening )
    {
        // blocking wait for a connection from the client.
        const auto accepted = server.accept();
        
        if ( accepted )
        {
            // print the message layout how the received data is interpreted.
            std::cout << "Byte received | msg counter | user data\n";
            // set the data socket to non-blocking mode. if there is no data it will
            // not wait.
            server.m_data.set_blocking(FALSE);
                
            for ( ; ; )
            {
                mutex.lock();
                
                // check if the main thread changed the server_running variable.
                if ( server_running == FALSE )
                {
                    // leave this loop immediately
                    break;
                }
                
                mutex.unlock();
                
                std::array< AR::uint8, 4U > data;
                // receive the messages from the client by using the data socket.
                const AR::sint16 received = server.m_data.receive(&data, data.size());

                if ( received > 0 )
                {
                    std::cout << received << " | ";
                    // print on the screen what we have received.
                    AR::uint16 ctr = static_cast< AR::uint16 >(data[0]);
                    std::cout << std::hex << ctr << " | ";
                    std::cout << data[1] << data[2] << data[3];
                    std::cout << "\n";
                }
                else
                {
                
                }
            } // receive loop
        } // connection from client accepted
    } // listening socket
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) noexcept
{
    // Create one TCP/IP client that sends data to the server.
    TcpClient client;
    // Create one thread for the TCP server that accepts the connection from the
    // client and then mirrors the data received from the TCP client.
    std::thread st(server_thread);
    // wait until the server thread has started.
    AR::boolean temp_running = FALSE;
    
    // wait until the server thread has been started. we don't want to connect
    // when the server is not listening for connections yet.
    while ( temp_running == FALSE )
    {
        // because this variable is used in two threads we need a mutex to 
        // avoid read/write access from both threads at the same time.
        mutex.lock();
        temp_running = server_running;
        // do not forget to unlock the mutex, otherwise this may end in a dead-lock
        // where both threads are not working.
        mutex.unlock();
    }

    // Try to connect to the TCP server from the TCP client...
    // Because the server runs on the same machine we connect to localhost and
    // use port 5555.
    const auto connected = client.connect("127.0.0.1", 5555U);
    // a counter for the messages that have been sent and to detect packet loss.
    AR::uint8 ctr{0U};
    
    if ( connected )
    {
        std::cout << "Connection established...\n";
        
        for ( auto i = 0U ; i < 10U ; ++i )
        {
            // we will send some test data to the server
            std::array< AR::uint8, 4U > data;
            // a counter for all messages that have been sent
            data[0] = ctr;
            // user data means we need help at client side
            data[1] = 'S';
            data[2] = 'O';
            data[3] = 'S';
            
            // send out some sample data that is going to be displayed.
            const auto sent = client.send(&data, data.size());
            
            if ( sent < 0 )
            {
                break;
            }
            
            // every time a message is sent out the counter is incremented.
            // we can detect message loss at the receiver side.
            ++ctr;
            
            // send messages at a given rate.
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);
        }
    }
    else
    {
        std::cout << "Connection has not been established!\n";
        std::cout << "Error: " << client.get_last_error() << "\n";
    }
    
    // set the variable to false, otherwise the thread will not stop.
    mutex.lock();
    // this will break the loop of the server thread and the thread is now joinable.
    server_running = FALSE;
    // do not forget to unlock
    mutex.unlock();
    // exit the server thread.
    st.join();
    return EXIT_SUCCESS;
}

