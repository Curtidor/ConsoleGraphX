#pragma once
#include <type_traits>
#include <limits>
#include <tuple>
#include <utility>
#include <functional>

// NOTE INDEX SUPPORTS ELEMENTS UP TO (size_t / 2)
template <typename T, typename Tuple>
struct IndexInTuple;

// specialization for finding index of T in tuple with T at the first position
template <typename T, typename... Ts>
struct IndexInTuple<T, std::tuple<T, Ts...>> : std::integral_constant<std::size_t, 0> {};

// specialization for finding index when T is not the first element
template <typename T, typename U, typename... Ts>
struct IndexInTuple<T, std::tuple<U, Ts...>> : std::integral_constant<std::size_t, 1 + IndexInTuple<T, std::tuple<Ts...>>::value> {};

// default index for types not in tuple
template <typename T>
struct IndexInTuple<T, std::tuple<>> : std::integral_constant<std::size_t, (std::numeric_limits<std::size_t>::max)() / 2> {};

// check if type T is in a tuple of types
template <typename T, typename Tuple>
struct IsTypeInTuple;

// specialization for std::tuple
template <typename T, typename... Ts>
struct IsTypeInTuple<T, std::tuple<Ts...>>
    : std::disjunction<std::is_same<T, Ts>...> {};

template <std::size_t N, typename... Ts>
decltype(auto) GetTupleElement(std::tuple<Ts...>& tpl) 
{
    return std::get<N>(tpl); 
}

// helper to detect if a type is std::function
template <typename T>
struct is_std_function : std::false_type {};

template <typename Ret, typename... Args>
struct is_std_function<std::function<Ret(Args...)>> : std::true_type {};

// helper to detect if a type is a callable raw function pointer
template <typename T>
struct is_function_pointer : std::false_type {};

template <typename Ret, typename... Args>
struct is_function_pointer<Ret(*)(Args...)> : std::true_type {};

// helper to check if the type is either a raw pointer or std::function
template <typename T>
struct is_valid_event_type : std::integral_constant<bool,
    is_function_pointer<T>::value || is_std_function<T>::value> {};

