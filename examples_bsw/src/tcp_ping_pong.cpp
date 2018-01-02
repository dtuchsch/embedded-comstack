// The example sends data between a TCP/IP client and a TCP/IP server.
// The TCP client will send some sample data the TCP server will receive and
// then prints out the received message.
// This example examines the use of the classes TCPClient and TCPServer.
// Please note that the given example is for demonstration purposes only.
// It may contain bugs and unresolved connection errors, but it shows how one
// can use the TCP client and server.

#include "TcpClient.h" // We need the header to create one TCP client object.
#include "TcpServer.h" // We need the header to create the TCP server object.
#include <array>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>

//! this variable is used to manage the thread's lifetime.
static std::atomic< bool > server_running{false};

////////////////////////////////////////////////////////////////////////////////
void server_thread() noexcept
{
    TcpServer server;
    server.reuse_addr();
    // listening on 255.255.255.0 and port 5555. The specified IP accepts
    // all incoming connections.
    const auto listening = server.listen("255.255.255.0", 5555U);
    server_running = true;

    if (listening)
    {
        // Blocking wait for a connection from the client.
        const auto accepted = server.accept();

        if (accepted)
        {
            // print the message layout how the received data is interpreted.
            std::cout << "Byte received | msg counter | user data\n";

            for (;;)
            {
                // check if the main thread changed the server_running variable.
                if (server_running == false)
                {
                    // leave this loop immediately
                    break;
                }

                std::array< std::uint8_t, 4U > data;
                // Receive the messages from the client by using the data
                // socket.
                const std::uint16_t received =
                    server.m_data.receive(&data, data.size());

                if (received > 0U)
                {
                    std::cout << received << " | ";
                    // print on the screen what has been received
                    std::uint16_t ctr = static_cast< std::uint16_t >(data[0]);
                    std::cout << std::hex << ctr << " | ";
                    std::cout << data[1] << data[2] << data[3];
                    std::cout << "\n";
                }
            } // receive loop
        }     // connection from client accepted
    }         // listening socket
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) noexcept
{
    // Create one TCP/IP client that sends data to the server.
    TcpClient client;
    // Create one thread for the TCP server that accepts the connection from the
    // client and then mirrors the data received from the TCP client.
    std::thread st(server_thread);
    // wait until the server thread has started.
    bool temp_running = false;

    // wait until the server thread has been started. we don't want to connect
    // when the server is not listening for connections yet.
    while (temp_running == false)
    {
        temp_running = server_running;
    }

    // Try to connect to the TCP server from the TCP client...
    // Because the server runs on the same machine we connect to localhost and
    // use port 5555.
    const auto connected = client.connect("127.0.0.1", 5555U);
    // a counter for the messages that have been sent and to detect packet loss.
    std::uint8_t ctr{0U};

    if (connected)
    {
        std::cout << "Connection established...\n";

        for (auto i = 0U; i < 10U; ++i)
        {
            // we will send some test data to the server
            std::array< std::uint8_t, 4U > data;
            // a counter for all messages that have been sent
            data[0] = ctr;
            // user data means we need help at client side
            data[1] = 'S';
            data[2] = 'O';
            data[3] = 'S';

            // send out some sample data that is going to be displayed.
            const auto sent = client.send(&data, data.size());

            if (sent < 0)
            {
                std::cerr << "Sending not possible.\n";
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

    // this will break the loop of the server thread and the thread is now
    // joinable.
    server_running = false;
    // exit the server thread.
    st.join();
    return EXIT_SUCCESS;
}
