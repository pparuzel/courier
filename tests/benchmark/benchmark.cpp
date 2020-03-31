#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include <unordered_map>
#include <utility>

#include "catch.hpp"
#include "courier/courier.hpp"

namespace bench1
{
struct Entity {
    std::unordered_map<double, double*> large_map{};
    Entity* some_pointer = nullptr;
    std::size_t some_value = 0;
    bool some_flag = false;
    /* this padding does not change the structure size */
    // char padding[7];
};

struct SmallEntity {
    bool some_flag = false;
};

struct EventA {
    explicit EventA(int a, int b) : value{a + b} {}
    int value;
};

struct EventB {
    explicit EventB(Entity entity) : entity{std::move(entity)} {}
    Entity entity;
};

struct EventBSmall {
    using Entity = SmallEntity;
    explicit EventBSmall(Entity entity) : entity{entity} {}
    Entity entity;
};

struct EventC {
};

using events = std::tuple<EventA, EventB, EventC>;
using dispatcher = courier::dispatcher<events>;

template <typename Entity>
void post_numerous_events_cpy(dispatcher& disp, Entity& entity, std::size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        const int x = 40;
        const int y = 2;
        disp.post(EventA{x, y});
        disp.post(EventB{entity});
        disp.post(EventC{});
    }
}

template <typename Entity>
void post_numerous_events_fwd(dispatcher& disp, Entity& entity, std::size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        const int x = 40;
        const int y = 2;
        disp.post<EventA>(x, y);
        disp.post<EventB>(entity);
        disp.post<EventC>();
    }
}

}  // namespace bench1

TEST_CASE("Stress test", "[benchmark][intense-benchmark]")
{
    SECTION("Sending large events")
    {
        using namespace bench1;
        dispatcher disp;
        Entity ent;
        auto n_iters = {20, 50, 100, 200, 500};
        SECTION("via r-value reference")
        {
            for (auto n : n_iters) {
                BENCHMARK(std::to_string(n) + 'k')
                {
                    return post_numerous_events_cpy(disp, ent, n * 1000);
                };
            }
        }
        SECTION("via perfect forwarding")
        {
            for (auto n : n_iters) {
                BENCHMARK(std::to_string(n) + 'k')
                {
                    return post_numerous_events_fwd(disp, ent, n * 1000);
                };
            }
        }
    }
}

TEST_CASE("Mild stress test", "[mild-benchmark]")
{
    SECTION("Sending small events")
    {
        using namespace bench1;
        dispatcher disp;
        Entity ent;
        auto n_iters = {20, 50, 100, 200, 500};
        SECTION("via r-value reference")
        {
            for (auto n : n_iters) {
                BENCHMARK(std::to_string(n) + 'k')
                {
                    return post_numerous_events_cpy(disp, ent, n * 1000);
                };
            }
        }
        SECTION("via perfect forwarding")
        {
            for (auto n : n_iters) {
                BENCHMARK(std::to_string(n) + 'k')
                {
                    return post_numerous_events_fwd(disp, ent, n * 1000);
                };
            }
        }
    }
}
