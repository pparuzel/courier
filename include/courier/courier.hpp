#pragma once

#include <forward_list>
#include <functional>
#include <iostream>
#include <memory>
#include <tuple>

namespace courier::detail
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

template <typename T>
using callback_t = std::function<void(T&)>;

template <typename T>
using callback_ptr = std::unique_ptr<callback_t<T>>;

}  // namespace courier::detail

namespace courier
{
template <typename TEvent>
class poster
{
public:
    constexpr poster() = default;

    template <typename F>
    constexpr auto add_listener(F&& func) noexcept
    {
        return listeners_
            .emplace_front(std::make_unique<detail::callback_t<TEvent>>(
                std::forward<F>(func)))
            .get();
    }

    constexpr void remove_listener(detail::callback_t<TEvent>* ptr) noexcept
    {
        listeners_.remove_if(
            [ptr](const auto& callback) { return ptr == callback.get(); });
    }

    template <typename... Args>
    constexpr void trigger(Args&&... args) const
    {
        for (auto&& listener : std::as_const(listeners_)) {
            (*listener)(std::forward<Args>(args)...);
        }
    }

private:
    std::forward_list<detail::callback_ptr<TEvent>> listeners_;
};

template <typename... E>
class dispatcher_impl
{
public:
    static_assert(
        sizeof...(E) > 0,
        "courier::dispatcher requires event types to be more than zero");

    constexpr dispatcher_impl() = default;

    template <typename TEvent, typename F>
    constexpr decltype(auto) add(F&& func) noexcept
    {
        static_assert(detail::tuple_contains_type_v<poster<TEvent>,
                                                    decltype(event_senders_)>,
                      "Cannot register to an unspecified event");
        return get_poster<TEvent>().template add_listener<F>(
            std::forward<F>(func));
    }

    template <typename TEvent, auto MemberFunc, typename Object>
    constexpr decltype(auto) add(Object&& f) noexcept
    {
        static_assert(detail::tuple_contains_type_v<poster<TEvent>,
                                                    decltype(event_senders_)>,
                      "Cannot register to an unspecified event");
        return get_poster<TEvent>().template add_listener(
            [&](auto&& e) { (f.*MemberFunc)(std::forward<decltype(e)>(e)); });
    }

    template <typename TEvent>
    constexpr void remove(detail::callback_t<TEvent>* ptr) noexcept
    {
        static_assert(detail::tuple_contains_type_v<poster<TEvent>,
                                                    decltype(event_senders_)>,
                      "Cannot remove an unspecified event");
        get_poster<TEvent>().remove_listener(ptr);
    }

    template <typename TEvent>
    constexpr void post(TEvent& event) const
    {
        if constexpr (detail::tuple_contains_type_v<poster<TEvent>,
                                                    decltype(event_senders_)>) {
            get_poster<TEvent>().trigger(event);
        } else {
            std::cerr << "warning: a used type "
#ifndef RTTI_DISABLED
                      << "id '" << typeid(TEvent).name() << "' "
#endif
                      << "is not specified in the dispatcher." << std::endl;
        }
    }

    template <typename TEvent>
    constexpr void post(TEvent&& event) const
    {
        if constexpr (detail::tuple_contains_type_v<poster<TEvent>,
                                                    decltype(event_senders_)>) {
            get_poster<TEvent>().trigger(event);
        } else {
            std::cerr << "warning: a used type "
#ifndef RTTI_DISABLED
                      << "id '" << typeid(TEvent).name() << "' "
#endif
                      << "is not specified in the dispatcher." << std::endl;
        }
    }

    template <typename TEvent, typename... Args>
    constexpr void post(Args&&... args) const
    {
        if constexpr (detail::tuple_contains_type_v<poster<TEvent>,
                                                    decltype(event_senders_)>) {
            TEvent event{std::forward<Args>(args)...};
            get_poster<TEvent>().trigger(event);
        } else {
            std::cerr << "warning: a used type "
#ifndef RTTI_DISABLED
                      << "id '" << typeid(TEvent).name() << "' "
#endif
                      << "is not specified in the dispatcher." << std::endl;
        }
    }

private:
    template <typename TEvent>
    constexpr const auto& get_poster() const noexcept
    {
        return std::get<poster<TEvent>>(event_senders_);
    }

    template <typename TEvent>
    constexpr auto& get_poster() noexcept
    {
        return const_cast<poster<TEvent>&>(
            std::as_const(*this).template get_poster<TEvent>());
    }

private:
    std::tuple<poster<E>...> event_senders_{};
};

template <typename... E>
class dispatcher : public dispatcher_impl<E...>
{
};

template <typename... E>
class dispatcher<std::tuple<E...>> : public dispatcher_impl<E...>
{
};

}  // namespace courier
