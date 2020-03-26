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

struct AxisAlignedBoundingBox {
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

using BoxCollisionEvent =
    CollisionEvent<AxisAlignedBoundingBox, AxisAlignedBoundingBox>;

struct ExplosionEvent {
    float blast_force;
    Vec2 position;
};

ems::dispatcher<BoxCollisionEvent, ExplosionEvent> d{};

void on_aabb_collision(const BoxCollisionEvent& e)
{
    std::cout << "Detected box collision:\n\t first box: " << e.first.position
              << "\n\tsecond box: " << e.second.position << std::endl;

    const auto& [a, b] = e;
    const auto [aw, ah] = e.first.size;
    const auto [bw, bh] = e.second.size;
    const auto [aposx, aposy] = e.first.position;
    const auto [bposx, bposy] = e.second.position;
    d.send(ExplosionEvent {
        .blast_force = std::hypot(aw, ah) * std::hypot(bw, bh),
        .position = Vec2{(aposx + bposx) / 2, (aposy + bposy) / 2},
    });
}

void explode(const ExplosionEvent& e)
{
    std::cout << "Boom! Explosion at " << e.position << " with "
              << e.blast_force << " newtons of force." << std::endl;
}

int main()
{
    d.subscribe<BoxCollisionEvent, &on_aabb_collision>();
    d.subscribe<ExplosionEvent, &explode>();
    AxisAlignedBoundingBox box{.position = {130, 150}, .size = {50, 100}};
    d.send(BoxCollisionEvent{box, box});
}
