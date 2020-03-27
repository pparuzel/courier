#pragma once

#include <functional>
#include <tuple>
#include <vector>

namespace ems
{
/**
 *
 */
template <typename TEvent>
class sender
{
public:
    template <typename F>
    constexpr void add_listener(F&& f) noexcept
    {
        listeners_.emplace_back(std::forward<F>(f));
    }

    template <typename... Args>
    constexpr void trigger(Args&&... args) const
    {
        // TODO: mutable lambdas don't work here cause of `const`
        for (auto&& listener : std::as_const(listeners_)) {
            listener(std::forward<Args>(args)...);
        }
    }

private:
    std::vector<std::function<void(const TEvent&)>> listeners_;
};

template <typename... E>
class dispatcher
{
public:
    static_assert(sizeof...(E) > 0,
                  "ems::dispatcher requires event types to be more than zero");

    constexpr dispatcher() = default;

    template <typename TEvent, typename F>
    constexpr void subscribe(F&& f) noexcept
    {
        get_sender<TEvent>().template add_listener<F>(std::forward<F>(f));
    }

    template <typename TEvent>
    constexpr void send(const TEvent& event) const
    {
        get_sender<TEvent>().trigger(event);
    }

private:
    template <typename TEvent>
    constexpr const auto& get_sender() const noexcept
    {
        return std::get<sender<TEvent>>(event_senders_);
    }

    template <typename TEvent>
    constexpr auto& get_sender() noexcept
    {
        return const_cast<sender<TEvent>&>(
            std::as_const(*this).template get_sender<TEvent>());
    }

private:
    std::tuple<sender<E>...> event_senders_{};
};

}  // namespace ems
