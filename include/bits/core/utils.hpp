#pragma once

#include <array>
#include <stdexcept> //std::runtime_error
#include <string>
#include <tuple>
#include <type_traits>

#ifndef NDEBUG
    #define JADA_DEBUG_BUILD 1
#endif

namespace jada {

// Trait to type check all types of a std::tuple
template <template <typename, typename...> class Trait, typename Tuple>
struct all_of;

template <template <typename, typename...> class Trait, typename... Types>
struct all_of<Trait, std::tuple<Types...>> : std::conjunction<Trait<Types>...> {
};

template <template <typename, typename...> class Trait, typename... Types>
struct all_of<Trait, std::pair<Types...>> : std::conjunction<Trait<Types>...> {
};


#ifdef JADA_DEBUG_BUILD
constexpr void runtime_assert(bool condition, const char* msg) {
    if (!condition) {

        throw std::runtime_error(msg);
    }

}
#else
constexpr void runtime_assert([[maybe_unused]] bool        condition,
                              [[maybe_unused]] const char* msg) {}
#endif


/// @brief Converts all tuple elements to an array of elements of the same size
/// @tparam tuple_t the type of the tuple to convert
/// @param tuple the input tuple to convert
/// @return std::array of the tuple elements
template <typename tuple_t> constexpr auto tuple_to_array(tuple_t&& tuple) {

    constexpr auto get_array = [](auto&&... x) constexpr {
        return std::array{std::forward<decltype(x)>(x)...};
    };
    return std::apply(get_array, std::forward<tuple_t>(tuple));
}

namespace detail{


template<typename... Ts, size_t... Is>
auto tuple_add(const std::tuple<Ts...>& tuple1, const std::tuple<Ts...>& tuple2, std::index_sequence<Is...>) {
    return std::make_tuple((std::get<Is>(tuple1) + std::get<Is>(tuple2))...);
}
}

template<typename... Ts>
auto tuple_add(const std::tuple<Ts...>& tuple1, const std::tuple<Ts...>& tuple2) {
    return detail::tuple_add(tuple1, tuple2, std::index_sequence_for<Ts...>{});
}


} // namespace jada
