#pragma once
#include <type_traits>
#include <limits>
#include <tuple>
#include <utility>

template<typename... Types>
using TupleOfTypes = std::tuple<Types...>;

template <typename T>
struct CompileTimeFalse : std::false_type { };

template <typename T>
constexpr bool CompileTimeFalseV = CompileTimeFalse<T>::value;

template <typename T>
void TriggerTypeNameInError(T&&)
{
	static_assert(CompileTimeFalseV<T>,
		"Compilation failed because you wanted to know the type; see below:");
}

template <typename T>
void PrintSingleTypeSTAT()
{
	TriggerTypeNameInError<T>(std::forward<T>(T()));
}

template <typename TupleT, std::size_t... Indexes>
void ForEachTypeInTupleSTAT(std::index_sequence<Indexes...>)
{
	(PrintSingleTypeSTAT<std::tuple_element_t<Indexes, TupleT>>(), ...);
}

template <typename... Args>
void PrintTypesInPackSTAT()
{
	using TupleOfArgs = TupleOfTypes<Args...>;
	ForEachTypeInTupleSTAT<TupleOfArgs>(std::make_index_sequence<std::tuple_size_v<TupleOfArgs>>{});
}

template <typename T>
struct StripPointer
{
    using type = T;
};

template <typename T>
struct StripPointer<T*>
{
    using type = T;
};

template <typename T>
struct StripPointer<T**>
{
	using type = T*;
};

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
