#include "dynamic_ems.hpp"

#include <iostream>

struct Box {
    std::string name{"<unnamed>"};
};

template <typename T1, typename T2>
struct CollisionEvent : public dynamic_ems::event, public std::pair<T1, T2> {
    explicit CollisionEvent(const T1& obj1, const T2& obj2)
        : std::pair<T1, T2>{obj1, obj2}
    {
    }
};
using box_collision_event_t = CollisionEvent<Box, Box>;

struct NoEvent : public dynamic_ems::event {
};
// struct Explosion {
//     void explode(const box_collision_event_t& event) {
//         void(this);
//         std::cout << "Boom! Destroyed " << event.first.name << " and "
//                   << event.second.name << std::endl;
//     }
// };

void explode(const box_collision_event_t& event)
{
    std::cout << "Boom! Destroyed " << event.first.name << " and "
              << event.second.name << std::endl;
}

void handle_noevent(const NoEvent&)
{
    std::cout << "No event happened" << std::endl;
}

int main()
{
    dynamic_ems::registry r;
    r.subscribe<box_collision_event_t, &explode>();
    r.subscribe<NoEvent, &handle_noevent>();
    Box box1{"box1"}, box2{"box2"};
    r.send(CollisionEvent{box1, box2});
    r.send(NoEvent{});
}
