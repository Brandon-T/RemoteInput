//
//  PythonTraits.hxx
//  RemoteInput
//
//  Created by Brandon on 2020-11-10.
//  Copyright © 2020 XIO. All rights reserved.
//

#ifndef REMOTEINPUT_PYTHONTRAITS_HXX
#define REMOTEINPUT_PYTHONTRAITS_HXX

#include <type_traits>
#include <tuple>

template<class T, class... Ts>
struct is_same_of : std::bool_constant<(std::is_same<typename std::remove_cv<T>::type, typename std::remove_cv<Ts>::type>::value || ...)> { };

template<typename Tuple, std::size_t... Ints>
auto select_tuple(Tuple&& tuple, std::index_sequence<Ints...>)
{
    return std::tuple<std::tuple_element_t<Ints, Tuple>...>(std::get<Ints>(std::forward<Tuple>(tuple))...);
}

template<std::size_t N, typename Seq>
struct offset_sequence;

template<std::size_t N, std::size_t... Ints>
struct offset_sequence<N, std::index_sequence<Ints...>>
{
    using type = std::index_sequence<Ints + N...>;
};

template<std::size_t N, typename Seq>
using offset_sequence_t = typename offset_sequence<N, Seq>::type;

template<std::size_t StartIndex, std::size_t Length>
using offset_index_sequence_t = offset_sequence_t<StartIndex, std::make_index_sequence<Length>>;

template<typename Fn, typename Argument, std::size_t... Ns>
auto tuple_transform_impl(Fn&& fn, Argument&& argument, std::index_sequence<Ns...>)
{
    if constexpr (sizeof...(Ns) == 0)
    {
        return std::tuple<>(); // empty tuple
    }
    else if constexpr (std::is_same_v<decltype(fn(std::get<0>(argument))), void>)
    {
        (fn(std::get<Ns>(argument)),...); // no return value expected
        return;
    }
    else if constexpr (std::is_lvalue_reference_v<decltype(fn(std::get<0>(argument)))>)
    {
        return std::tie(fn(std::get<Ns>(argument))...);
    }
    else if constexpr (std::is_rvalue_reference_v<decltype(fn(std::get<0>(argument)))>)
    {
        return std::forward_as_tuple(fn(std::get<Ns>(argument))...);
    }
    else
    {
        return std::tuple(fn(std::get<Ns>(argument))...);
    }
}

template<typename Fn, typename... Ts>
auto tuple_transform(Fn&& fn, const std::tuple<Ts...>& tuple)
{
    return tuple_transform_impl(std::forward<Fn>(fn), tuple, std::make_index_sequence<sizeof...(Ts)>());
}

template<typename... Args>
std::string to_string(const std::tuple<Args...> &ts, const char* separator = "")
{
    auto to_string = [](auto value) -> std::string {
        if constexpr(is_same_of<decltype(value), char, char*, const char*, std::string>::value)
        {
            return std::string() + value;
        }
        else
        {
            return std::to_string(value);
        }
    };

    return std::apply([separator, to_string](const auto&... ts) {
        std::string result;
        const char* sep = "";
        ((static_cast<void>(result += sep + to_string(ts)), sep = separator), ...);
        return result;
    }, ts);
}

#endif //REMOTEINPUT_PYTHONTRAITS_HXX
