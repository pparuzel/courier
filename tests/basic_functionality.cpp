#define CATCH_CONFIG_MAIN

#include <tuple>
#include <utility>
#include <vector>

#include "catch.hpp"
#include "ebus/ebus.hpp"

struct Event1 {
    int i;
    int j;
    int k;
};

struct Event2 {
};

struct Event3 {
};

struct Event4 : Event2 {
};

TEST_CASE("Basic functionality", "[basic]")
{
    using events = std::tuple<Event1, Event2>;
    ebus::dispatcher<events> dispatcher;

    SECTION("assigning an event to a lambda")
    {
        auto event_triggered = false;
        auto&& cb =
            dispatcher.add<Event1>([&](auto&&...) { event_triggered = true; });
        REQUIRE(!event_triggered);
        dispatcher.post(Event1{0, 0});
        REQUIRE(event_triggered);

        SECTION("then removing and posting an event")
        {
            event_triggered = false;
            dispatcher.remove(cb);
            REQUIRE(!event_triggered);
            dispatcher.post(Event1{0, 0});
            REQUIRE(!event_triggered);
        }
    }
    SECTION("assigning two events")
    {
        bool events_triggered[3] = {false, false};
        dispatcher.add<Event1>([&](auto&&...) { events_triggered[0] = true; });
        dispatcher.add<Event2>([&](auto&&...) { events_triggered[1] = true; });
        SECTION("but calling a third unspecified")
        {
            SECTION("that is inheriting from a specified event")
            {
                Catch::RedirectedStdErr redirected_std_err{};
                dispatcher.post(Event4{});
                REQUIRE(!events_triggered[0]);
                REQUIRE(!events_triggered[1]);
                CHECK_THAT(redirected_std_err.str(),
                           Catch::Matchers::StartsWith("warning"));
            }
            Catch::RedirectedStdErr redirected_std_err{};
            dispatcher.post(Event3{});
            REQUIRE(!events_triggered[0]);
            REQUIRE(!events_triggered[1]);
            CHECK_THAT(redirected_std_err.str(),
                       Catch::Matchers::StartsWith("warning"));
        }
        SECTION("and calling the second event")
        {
            REQUIRE(!events_triggered[0]);
            REQUIRE(!events_triggered[1]);
            dispatcher.post(Event2{});
            REQUIRE(!events_triggered[0]);
            REQUIRE(events_triggered[1]);
        }
        SECTION("and calling both events")
        {
            REQUIRE(!events_triggered[0]);
            REQUIRE(!events_triggered[1]);
            dispatcher.post(Event2{});
            dispatcher.post(Event1{});
            REQUIRE(events_triggered[0]);
            REQUIRE(events_triggered[1]);
        }
        SECTION("but adding a registered event with a different callback")
        {
            events_triggered[0] = false;
            events_triggered[1] = false;
            SECTION("and making sure both are called")
            {
                dispatcher.add<Event1>(
                    [&](auto&&...) { events_triggered[1] = true; });
                REQUIRE(!events_triggered[0]);
                REQUIRE(!events_triggered[1]);
                dispatcher.post(Event1{0, 1});
                REQUIRE(events_triggered[0]);
                REQUIRE(events_triggered[1]);
            }
            SECTION("and checking if data is correct")
            {
                using summations = std::pair<std::pair<int, int>, int>;
                std::vector<summations> vs = {
                    {{14, 7}, 21}, {{4, 5}, 9},      {{2, 0}, 2},
                    {{0, 7}, 7},   {{80, 170}, 250},
                };
                dispatcher.add<Event1>(
                    [](const Event1& e) { CHECK(e.i + e.j == e.k); });
                for (auto&& [operands, result] : as_const(vs)) {
                    dispatcher.post(
                        Event1{operands.first, operands.second, result});
                }
            }
        }
    }
}
