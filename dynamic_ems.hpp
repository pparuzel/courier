#pragma once

#include <string>
#include <vector>

namespace dynamic_ems
{
struct event {
    virtual ~event() = default;
};
// event::event() = default;

struct callback {
    std::string dynamic_type;
    void (*func)(const event&);
};

class registry
{
public:
    template <typename TEvent, void (*Func)(const TEvent&)>
    void subscribe()
    {
        auto wrap = [](const event& event) {
            Func(static_cast<const TEvent&>(event));
        };
        callbacks_.push_back({typeid(TEvent).name(), wrap});
        // reinterpret_cast<void (*)(const event&)>(Func)
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

private:
    std::vector<callback> callbacks_;
};

}  // namespace dynamic_ems
