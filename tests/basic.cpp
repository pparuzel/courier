#define CATCH_CONFIG_MAIN

#include <tuple>
#include <utility>
#include <vector>

#include "catch.hpp"
#include "courier/courier.hpp"

struct Event1 {
    int i;
    int j;
    int k;
};

struct Event2 {
    std::size_t trigger_count = 0;
};

struct Event3 {
    std::size_t trigger_count = 0;
};

struct Event4 : Event2 {
};

void foo(Event2& e)
{
    ++e.trigger_count;
}

///
/// Basic functionality
///

TEST_CASE("Basic functionality", "[basic]")
{
    SECTION("assigning an event to a regular function")
    {
        courier::dispatcher<Event4> dp;
        Event4 event{};
        // Set trigger count to 1 to distinguish from the default 0
        event.trigger_count = 1;
        dp.add<Event4>(foo);
        REQUIRE(event.trigger_count == 1);
        SECTION("but then posting a different event")
        {
            SECTION("which is the base class of the registered event")
            {
                Catch::RedirectedStdErr redirected_std_err{};
                dp.post(static_cast<const Event2&>(event));
                REQUIRE(event.trigger_count == 1);
                CHECK_THAT(redirected_std_err.str(),
                           Catch::Matchers::StartsWith("warning"));
            }
            Catch::RedirectedStdErr redirected_std_err{};
            dp.post(reinterpret_cast<const Event3&>(event));
            REQUIRE(event.trigger_count == 1);
            CHECK_THAT(redirected_std_err.str(),
                       Catch::Matchers::StartsWith("warning"));
        }
        SECTION("and asserting the call happened once")
        {
            dp.post(event);
            REQUIRE(event.trigger_count == 2);
            SECTION("and asserting the call happened twice")
            {
                dp.post(event);
                REQUIRE(event.trigger_count == 3);
            }
        }
    }
    using events = std::tuple<Event1, Event2>;
    courier::dispatcher<events> dispatcher;
    SECTION("assigning an event to a stateful lambda")
    {
        auto value = 4;
        dispatcher.add<Event1>([&, multiplier = 2.f](auto&& event) mutable {
            multiplier *= 2;
            value += event.i * 1000;
            value += event.j * 100;
            value += event.k * 10;
            value *= multiplier;
        });
        REQUIRE(value == 4);
        dispatcher.post<Event1>(1, 2, 3);
        REQUIRE(value == 1234 * 4);
    }
    SECTION("assigning an event to a lambda")
    {
        auto event_triggered = false;
        auto&& cb =
            dispatcher.add<Event1>([&](auto&&...) { event_triggered = true; });
        REQUIRE(!event_triggered);
        dispatcher.post<Event1>(0, 0);
        REQUIRE(event_triggered);

        SECTION("then removing and posting an event")
        {
            event_triggered = false;
            dispatcher.remove(cb);
            REQUIRE(!event_triggered);
            dispatcher.post<Event1>(0, 0);
            REQUIRE(!event_triggered);
        }
    }
    SECTION("assigning two events")
    {
        bool events_triggered[3] = {false, false};
        dispatcher.add<Event1>([&](auto&&...) { events_triggered[0] = true; });
        dispatcher.add<Event2>([&](auto&&...) { events_triggered[1] = true; });
        SECTION("but calling an unspecified third one")
        {
            SECTION("that is inheriting from a specified event")
            {
                Catch::RedirectedStdErr redirected_std_err{};
                dispatcher.post<Event4>();
                REQUIRE(!events_triggered[0]);
                REQUIRE(!events_triggered[1]);
                CHECK_THAT(redirected_std_err.str(),
                           Catch::Matchers::StartsWith("warning"));
            }
            Catch::RedirectedStdErr redirected_std_err{};
            dispatcher.post<Event3>();
            REQUIRE(!events_triggered[0]);
            REQUIRE(!events_triggered[1]);
            CHECK_THAT(redirected_std_err.str(),
                       Catch::Matchers::StartsWith("warning"));
        }
        SECTION("and calling the second event")
        {
            REQUIRE(!events_triggered[0]);
            REQUIRE(!events_triggered[1]);
            dispatcher.post<Event2>();
            REQUIRE(!events_triggered[0]);
            REQUIRE(events_triggered[1]);
        }
        SECTION("and calling both events")
        {
            REQUIRE(!events_triggered[0]);
            REQUIRE(!events_triggered[1]);
            dispatcher.post<Event2>();
            dispatcher.post<Event1>();
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
                dispatcher.post<Event1>(0, 1);
                REQUIRE(events_triggered[0]);
                REQUIRE(events_triggered[1]);
            }
            SECTION("and checking if data is correct")
            {
                using summations = std::pair<std::pair<int, int>, int>;
                std::vector<summations> vs;
                vs.emplace_back(std::make_pair(14, 7), 21);
                vs.emplace_back(std::make_pair(4, 5), 9);
                vs.emplace_back(std::make_pair(2, 0), 2);
                vs.emplace_back(std::make_pair(0, 7), 7);
                vs.emplace_back(std::make_pair(80, 170), 250);
                dispatcher.add<Event1>([](const Event1& e) {
                    // Check summations are correctly passed
                    CHECK(e.i + e.j == e.k);
                });
                for (auto&& [operands, result] : as_const(vs)) {
                    dispatcher.post<Event1>(operands.first, operands.second,
                                            result);
                }
            }
        }
    }
}

///
/// Construction and destruction tests
///

TEST_CASE("Construction tests", "[basic]")
{
    SECTION("contruct dispatcher with a std::vector")
    {
        courier::dispatcher<std::vector<int>> d;
        d.add<std::vector<int>>(
            [](auto&& event) { std::cout << event.size() << '\n'; });
        SECTION("which is list-initialized with 2 elements")
        {
            Catch::RedirectedStdOut redirected_std_out{};
            d.post<std::vector<int>>(10, 2);
            auto out = Catch::trim(redirected_std_out.str());
            CHECK(out == "2");
        }
        SECTION("which is initialized with 10 elements of value 2")
        {
            Catch::RedirectedStdOut redirected_std_out{};
            d.post(std::vector<int>(10, 2));
            auto out = Catch::trim(redirected_std_out.str());
            REQUIRE(out == "10");
        }
    }
}

TEST_CASE("Destruction tests", "[basic]")
{
    SECTION("construct a two-event dispatcher")
    {
        courier::dispatcher<Event1, Event2> d;
        for (int i = 0; i < 10'000; ++i) {
            d.add<Event1>([](auto&&...) {});
            d.add<Event2>([](auto&&...) {});
        }
        d.add<Event2>([](auto&&...) {});
        const auto event1_count = 10'000;
        const auto event2_count = 10'001;
        REQUIRE(d.size() == event1_count + event2_count);
        SECTION("post a lot of events")
        {
            for (int i = 0; i < 100; ++i) {
                d.post<Event1>(i, i + 1, i + 2);
                d.post<Event2>(static_cast<std::size_t>(i + 3));
            }
            REQUIRE(d.size() == event1_count + event2_count);
            SECTION("and remove all Event1")
            {
                d.clear<Event1>();
                REQUIRE(d.size() == event2_count);
            }
            SECTION("and remove all Event2")
            {
                d.clear<Event2>();
                REQUIRE(d.size() == event1_count);
            }
            SECTION("and remove all events")
            {
                d.clear();
                REQUIRE(d.size() == 0);
                REQUIRE(d.empty());
            }
        }
    }
}
