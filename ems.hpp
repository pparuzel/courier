#pragma once

#include <functional>
#include <iostream>
#include <tuple>
#include <vector>

namespace ems::detail
{
template <typename T, typename Tuple>
struct tuple_contains_type;

template <typename T, typename... Other>
struct tuple_contains_type<T, std::tuple<Other...>>
    : std::disjunction<std::is_same<T, Other>...> {
};

template <typename T, typename Tuple>
inline constexpr bool tuple_contains_type_v =
    tuple_contains_type<T, Tuple>::value;
}  // namespace ems::detail

namespace ems
{
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
        // TODO: Do mutable lambdas work here despite `const`?
        for (auto&& listener : std::as_const(listeners_)) {
            listener(std::forward<Args>(args)...);
        }
    }

private:
    std::vector<std::function<void(const TEvent&)>> listeners_;
};

template <typename... E>
class dispatcher_impl
{
public:
    static_assert(sizeof...(E) > 0,
                  "ems::dispatcher requires event types to be more than zero");

    constexpr dispatcher_impl() = default;

    template <typename TEvent, typename F>
    constexpr void add(F&& f) noexcept
    {
        static_assert(detail::tuple_contains_type_v<sender<TEvent>,
                                                    decltype(event_senders_)>,
                      "Cannot subscribe to an unregistered event");
        get_sender<TEvent>().template add_listener<F>(std::forward<F>(f));
    }

    template <typename TEvent, auto MemberFunc, typename Object>
    constexpr void add(Object&& f) noexcept
    {
        static_assert(detail::tuple_contains_type_v<sender<TEvent>,
                                                    decltype(event_senders_)>,
                      "Cannot subscribe to an unregistered event");
        get_sender<TEvent>().template add_listener(
            [&](const TEvent& e) { (f.*MemberFunc)(e); });
    }

    template <typename TEvent>
    constexpr void post(const TEvent& event) const
    {
        if constexpr (detail::tuple_contains_type_v<sender<TEvent>,
                                                    decltype(event_senders_)>) {
            get_sender<TEvent>().trigger(event);
        } else {
#ifndef DISABLE_RTTI
            std::cerr << "warning: type id '" << typeid(TEvent).name()
                      << "' is not a registered type." << std::endl;
#endif
        }
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

template <typename... E>
class dispatcher : public dispatcher_impl<E...>
{
};

template <typename... E>
class dispatcher<std::tuple<E...>> : public dispatcher_impl<E...>
{
};

}  // namespace ems
