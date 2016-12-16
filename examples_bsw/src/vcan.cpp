////////////////////////////////////////////////////////////////////////////////
// This example shows the usage of the CanSocket class. This example is based on
// the game classic pong. Two threads are exchanging data over virtual CAN which
// mimics a little ping pong "game".
////////////////////////////////////////////////////////////////////////////////

#include "CanSocket.h"
#include <chrono>
#include <mutex>
#include <random>
#include <thread>

////////////////////////////////////////////////////////////////////////////////
enum class GameStatus
{
    READY,
    PLAY,
    GAME_OVER
};

////////////////////////////////////////////////////////////////////////////////
enum class Players
{
    PLAYER1,
    PLAYER2
};

////////////////////////////////////////////////////////////////////////////////
enum class Events
{
    SERVE,
    BALL_HIT,
    BALL_MISS,
    NONE
};

////////////////////////////////////////////////////////////////////////////////
void ball_flies() noexcept
{
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(500ms);
}

////////////////////////////////////////////////////////////////////////////////
std::pair< Events, GameStatus > player_act(Events event) noexcept
{
    if (event == Events::BALL_HIT)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        // give "true" 1/4 of the time
        // give "false" 3/4 of the time
        std::bernoulli_distribution d(0.9);
        const auto hit = d(gen);

        if (hit)
        {
            return std::make_pair(Events::BALL_HIT, GameStatus::PLAY);
        }
        else
        {
            return std::make_pair(Events::BALL_MISS, GameStatus::GAME_OVER);
        }
    }
    else if (event == Events::SERVE)
    {
        return std::make_pair(Events::BALL_HIT, GameStatus::PLAY);
    }
    else
    {
        return std::make_pair(Events::NONE, GameStatus::GAME_OVER);
    }
}

////////////////////////////////////////////////////////////////////////////////
void player2() noexcept
{
    CanSocket can_player2("vcan0");
    can_player2.enable_canfd();
    GameStatus status{GameStatus::READY};

    for (;;)
    {
        CanIDType id{0};
        CanFDData data_fd;
        const auto received = can_player2.receive(id, data_fd);
        const auto action = player_act(static_cast< Events >(data_fd[0]));

        if (action.first == Events::BALL_HIT)
        {
            ball_flies();
            CanFDData data{static_cast< std::uint8_t >(action.first),
                           static_cast< std::uint8_t >(action.second), 6};
            can_player2.send(static_cast< int >(Players::PLAYER2), data, 10);
        }
        else
        {
            CanFDData data{static_cast< std::uint8_t >(action.first),
                           static_cast< std::uint8_t >(action.second), 6};
            can_player2.send(static_cast< int >(Players::PLAYER2), data, 10);
            std::cout << "Player 1 wins!\n";
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
int main() noexcept
{
    std::thread p2(player2);
    CanSocket can_player1("vcan0");
    can_player1.enable_canfd();
    GameStatus status{GameStatus::READY};
    const auto action = player_act(Events::SERVE);
    CanStdData data{static_cast< std::uint8_t >(action.first),
                    static_cast< std::uint8_t >(action.second), 3};
    can_player1.send(static_cast< int >(Players::PLAYER1), data, 9);

    for (;;)
    {
        CanIDType id{0};
        CanFDData data_fd;
        const auto received = can_player1.receive(id, data_fd);

        if (action.first == Events::BALL_HIT)
        {
            ball_flies();
            CanStdData data{static_cast< std::uint8_t >(action.first),
                            static_cast< std::uint8_t >(action.second), 3};
            can_player1.send(static_cast< int >(Players::PLAYER1), data, 9);
        }
        else
        {
            CanStdData data{static_cast< std::uint8_t >(action.first),
                            static_cast< std::uint8_t >(action.second), 3};
            can_player1.send(static_cast< int >(Players::PLAYER1), data, 9);
            std::cout << "Player 2 wins!\n";
            break;
        }
    }

    p2.join();
    return 0;
}
