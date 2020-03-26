#include <functional>
#include <iostream>
#include <vector>

struct Event {
    virtual ~Event() = default;
};
// Event::~Event() = default;

struct Callback {
    std::string dynamic_type;
    void (*func)(const Event&);
};

struct Registry {
    template <typename TEvent, void (*Func)(const TEvent&)>
    void subscribe()
    {
        auto wrap = [](const Event& event) {
            Func(static_cast<const TEvent&>(event));
        };
        callbacks_.push_back({typeid(TEvent).name(), wrap});
        // reinterpret_cast<void (*)(const Event&)>(Func)
    }

    template <typename TEvent>
    void send(TEvent&& event)
    {
        for (auto&& cb : callbacks_) {
            if (cb.dynamic_type == typeid(event).name()) {
                cb.func(event);
            }
        }
    }

    std::vector<Callback> callbacks_;
};

struct Box {
    std::string name{"<unnamed>"};
};

template <typename T1, typename T2>
struct CollisionEvent : public Event, public std::pair<T1, T2> {
    explicit CollisionEvent(const T1& obj1, const T2& obj2)
        : std::pair<T1, T2>{obj1, obj2}
    {
    }
};
using box_collision_event_t = CollisionEvent<Box, Box>;

struct NoEvent : public Event {
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
    Registry r;
    r.subscribe<box_collision_event_t, &explode>();
    r.subscribe<NoEvent, &handle_noevent>();
    Box box1{"box1"}, box2{"box2"};
    r.send(CollisionEvent{box1, box2});
    r.send(NoEvent{});
}
