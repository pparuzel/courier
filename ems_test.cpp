#include "ems.hpp"

#include <cmath>
#include <iostream>

struct Vec2 {
    float x;
    float y;

    friend std::ostream& operator<<(std::ostream& out, const Vec2& vec)
    {
        out << '(' << vec.x << ", " << vec.y << ')';
        return out;
    }
};

struct AABBox {
    Vec2 position;
    Vec2 size;
};

template <typename T1, typename T2>
struct CollisionEvent : public std::pair<T1, T2> {
    constexpr CollisionEvent(const T1& box1, const T2& box2)
        : std::pair<T1, T2>{box1, box2}
    {
    }
};

using BoxCollisionEvent = CollisionEvent<AABBox, AABBox>;

struct ExplosionEvent {
    float blast_force;
    Vec2 position;
};

struct WipeoutEvent {
};

template <typename Dispatcher>
void on_aabb_collision(const BoxCollisionEvent& e, Dispatcher& d)
{
    std::cout << "Detected box collision:\n\t first box: " << e.first.position
              << "\n\tsecond box: " << e.second.position << std::endl;

    const auto [aw, ah] = e.first.size;
    const auto [bw, bh] = e.second.size;
    const auto [aposx, aposy] = e.first.position;
    const auto [bposx, bposy] = e.second.position;
    const auto blast_force = std::hypot(aw, ah) * std::hypot(bw, bh);
    d.send(ExplosionEvent{
        .blast_force = blast_force,
        .position = Vec2{(aposx + bposx) / 2, (aposy + bposy) / 2},
    });
    if (blast_force > 10'000) {
        d.send(WipeoutEvent{});
    }
}

void explode(const ExplosionEvent& e)
{
    std::cout << "Boom! Explosion at " << e.position << " with "
              << e.blast_force << " newtons of force." << std::endl;
}

void wipeout(const WipeoutEvent& e)
{
    std::cout << "The whole humanity is being wiped out due to a large blast."
              << std::endl;
}

int main()
{
    using event_registry =
        std::tuple<BoxCollisionEvent, ExplosionEvent, WipeoutEvent>;
    ems::dispatcher<event_registry> dispatcher{};
    auto on_aabb_collision_wrap = [&dispatcher](auto&& e) {
        return on_aabb_collision(e, dispatcher);
    };
    dispatcher.subscribe<BoxCollisionEvent>(on_aabb_collision_wrap);
    dispatcher.subscribe<ExplosionEvent>(&explode);
    AABBox box{.position = {130, 150}, .size = {50, 100}};
    dispatcher.subscribe<WipeoutEvent>(&wipeout);
    dispatcher.send(BoxCollisionEvent{box, box});
}
