#include <atomic>
#include <functional>
#include <iostream>
#include <thread>

#include "courier/courier.hpp"

struct JumpEvent {
};

struct InAirEvent {
    float time_before_landing;  // seconds
};

struct RopeIsNearEvent {
};

struct GameOverEvent {
    std::string reason;
};

using events = std::tuple<JumpEvent,        // Player jumped
                          InAirEvent,       // Player is in the air
                          RopeIsNearEvent,  // Rope is near, player has to jump!
                          GameOverEvent>;   // Game needs to stop

auto& get_courier()
{
    static courier::dispatcher<events> dispatcher{};
    return dispatcher;
}

class Player
{
public:
    Player()
    {
        auto&& c = get_courier();
        c.add<JumpEvent, &Player::jump>(*this);
        c.add<InAirEvent>([this](const InAirEvent& e) {
            // Land after some time specified in the event
            const auto milli = static_cast<int>(e.time_before_landing * 1000);
            std::this_thread::sleep_for(std::chrono::milliseconds{milli});
            land();
        });
        c.add<RopeIsNearEvent>([this](auto&&... event) {
            if (not in_air) {
                get_courier().post<GameOverEvent>(
                    "Player has not jumped :(\n> Press ENTER to quit");
            }
        });
        c.add<GameOverEvent>([this](auto&&...) { last_user_input_ = "q"; });
    }

    void handle_input()
    {
        auto&& c = get_courier();
        std::getline(std::cin, last_user_input_);
        if (last_user_input_.empty()) {
            if (not in_air) {
                c.post<JumpEvent>();
            }
        } else if (last_user_input_ == "q" or last_user_input_ == "quit") {
            c.post<GameOverEvent>("Player has quit");
        }
    }

    void jump(const JumpEvent& event)
    {
        std::cout << "<JUMPED>" << std::endl;
        in_air = true;
        std::thread{[] { get_courier().post<InAirEvent>(2.2f); }}.detach();
    }

    void land()
    {
        std::cout << "<LANDED>" << std::endl;
        in_air = false;
    }

private:
    std::atomic<bool> in_air = false;
    std::string last_user_input_{};
};

class SkippingRope
{
public:
    explicit SkippingRope() = default;

    explicit SkippingRope(std::chrono::milliseconds full_cycle_time_ms)
        : full_cycle_time_ms{full_cycle_time_ms}
    {
    }

    void run()
    {
        std::cout << "START! Skip over the skipping rope!" << std::endl;
        std::thread([this]() {
            auto&& c = get_courier();
            auto keep_rolling = true;
            // Stop this thread on game-over
            // Unnecessary here, but suppresses the warning of an endless loop
            c.add<GameOverEvent>([&](auto&&...) { keep_rolling = false; });
            while (keep_rolling) {
                std::this_thread::sleep_for(full_cycle_time_ms / 2);
                std::cout << "JUMP NOW!" << std::endl;
                std::this_thread::sleep_for(full_cycle_time_ms / 2);
                c.post<RopeIsNearEvent>();
            }
        }).detach();
    }

private:
    std::chrono::milliseconds full_cycle_time_ms{2600};
};

class Game
{
public:
    void run()
    {
        std::atomic running = true;
        GameOverEvent game_over{};
        get_courier().add<GameOverEvent>([&](auto&& event) {
            running = false;
            std::cout << "> GAME OVER";
            if (!event.reason.empty()) {
                std::cout << ": " << event.reason;
            }
            std::cout << std::endl;
        });
        rope_.run();
        while (running) {
            player_.handle_input();
        }
    }

private:
    Player player_;
    SkippingRope rope_;
};

int main()
{
    Game game;
    std::cout << "Game of jumping over a skipping rope\n\t";
    std::cout << "To make a regular jump, hit ENTER\n\t";
    std::cout << "or type `q`, `quit` or hit Ctrl-C to exit" << std::endl;
    game.run();
    std::cout << "Bye!" << std::endl;
}
