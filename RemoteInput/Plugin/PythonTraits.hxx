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
#include <string>
#include <numeric>
#include <locale>
#include <codecvt>

template<class T, class... Ts>
struct is_same_of : std::bool_constant<(std::is_same<typename std::remove_cv<T>::type, typename std::remove_cv<Ts>::type>::value || ...)> { };

template<class T>
struct is_vector
{
    static bool const value = false;
};

template<typename T>
struct is_vector<std::vector<T>>
{
    static bool const value = true;
};

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

template<typename T, typename Predicate>
std::string join(const std::vector<T> &range, const std::string &separator, Predicate converter)
{
    if (range.empty())
    {
        return std::string();
    }

    if (range.size() == 1)
    {
        return converter(range[0]);
    }

    std::string result = "";
    for (std::size_t i = 0; i < range.size() - 1; ++i)
    {
        result += converter(range[i]) + separator;
    }
    return result + converter(range[range.size() - 1]);

//    return std::accumulate(
//            next(begin(range)),
//            end(range),
//            converter(range[0]), // the initial value
//            [&separator, &converter](auto result, const auto &value) {
//                return result + separator + converter(value);
//            }
//    );
}

template<typename... Args>
std::string to_string(const std::tuple<Args...> &ts, const char* separator = "")
{
    auto convert_to_utf8 = [](const std::wstring &value) -> std::string {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(value);
    };

    auto convert_pointer_to_string = [](const auto &value) -> std::string {
        char buffer[(sizeof(void*) * 2) + 1] = {0};
        sprintf(buffer, "%p", value);
        return std::string(buffer);
    };

    auto convert_to_string = [convert_to_utf8, convert_pointer_to_string](auto value) -> std::string {
        if constexpr(is_same_of<std::decay_t<decltype(value)>, char, char*, const char*, std::string>::value)
        {
            return value;
        }
        else if constexpr(is_same_of<std::decay_t<decltype(value)>, wchar_t, std::wstring>::value)
        {
            return convert_to_utf8(value);
        }
        else if constexpr (is_vector<std::decay_t<decltype(value)>>::value)
        {
            using value_type = typename std::decay_t<decltype(value)>::value_type;
            if constexpr(is_same_of<value_type, char, char*, const char*, std::string>::value)
            {
                return "[" + join(value, " ", [](const value_type &value) { return value; }) + "]";
            }
            else if constexpr(is_same_of<value_type, wchar_t, wchar_t*, const wchar_t*, std::wstring>::value)
            {
                return "[" + join(value, " ", [convert_to_utf8](const value_type &value) { return convert_to_utf8(value); }) + "]";
            }
            else if constexpr(std::is_pointer<value_type>::value)
            {
                return "[" + join(value, " ", convert_pointer_to_string) + "]";
            }
            else if constexpr(std::is_union<value_type>::value)
            {
                return "[cannot_convert_type_to_string]";
            }
            else
            {
                return "[" + join(value, " ", [](const value_type &value) { return std::to_string(value); }) + "]";
            }
        }
        else if constexpr(std::is_pointer<decltype(value)>::value)
        {
            return convert_pointer_to_string(value);
        }
        else
        {
            return std::to_string(value);
        }
    };

    return std::apply([separator, convert_to_string](Args... ts) {
        std::string result;
        const char* sep = "";
        ((static_cast<void>(result += sep + convert_to_string(ts)), sep = separator), ...);
        return result;
    }, ts);
}

#endif //REMOTEINPUT_PYTHONTRAITS_HXX
