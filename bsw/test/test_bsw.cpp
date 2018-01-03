#include "Socket.h"
#include <gtest/gtest.h>

TEST(Sockets, CreateSocket)
{
    struct SocketImpl : public Socket< SocketImpl >
    {
        SocketImpl(bool success) noexcept : success_{success} {}
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

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
