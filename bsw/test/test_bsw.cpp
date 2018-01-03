#include "CanSocket.h"
#include "Socket.h"
#include <gtest/gtest.h>

TEST(Sockets, CreateSocket)
{
    struct SocketImpl : public Socket< SocketImpl >
    {
        explicit SocketImpl(bool success) noexcept
            : Socket< SocketImpl >{}, success_{success}
        {
        }
        bool create() noexcept { return success_; }
        bool success_{false};
    };

    {
        SocketImpl socket_impl{true};
        EXPECT_TRUE(socket_impl.is_socket_initialized());
        using namespace std::chrono_literals;
        const bool has_data = socket_impl.wait_for(10ms);
        EXPECT_FALSE(has_data);
        EXPECT_FALSE(socket_impl.set_blocking(true));
        EXPECT_FALSE(socket_impl.set_blocking(false));
        EXPECT_FALSE(socket_impl.set_blocking(false));
        EXPECT_FALSE(socket_impl.set_blocking(true));
    }
}

TEST(Sockets, SocketCanSendAndReceive)
{
    CanSocket can{"vcan0"};
    CanSocket can1{"vcan0"};
    const auto sent = can.send(0x06U, CanFDData{0x00U}, 1U);
    EXPECT_TRUE(can1.set_blocking(false));
    EXPECT_EQ(sent, 72);
    CanIDType can_id{};
    CanFDData data{};

    {
        const auto nbytes = can1.receive(can_id, data);
        EXPECT_EQ(nbytes, 1);
        EXPECT_EQ(can.get_last_error(), 0);
        EXPECT_EQ(can1.get_last_error(), 0);
    }
    {
        const auto nbytes = can1.receive(can_id, data);
        EXPECT_EQ(nbytes, -1);
        EXPECT_EQ(can1.get_last_error(), EAGAIN);
    }

    EXPECT_TRUE(can1.set_blocking(true));
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
