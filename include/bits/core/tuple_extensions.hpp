#pragma once

#include "rank.hpp"
#include "utils.hpp"

namespace jada {

namespace detail {

template <size_t... Is>
static constexpr auto
elementwise_add(auto tuple1, auto& tuple2, std::index_sequence<Is...>) {
    return std::make_tuple((std::get<Is>(tuple1) + std::get<Is>(tuple2))...);
}
} // namespace detail

///
///@brief Elementwise addition of two generic types which have std::get implemented.
///
///@param lhs Left-hand side operand.
///@param rhs Right-hand side operand.
///@return A tuple of elementwise added values.
///
static constexpr auto elementwise_add(auto lhs, auto rhs) {
    static_assert(rank(lhs) == rank(rhs), "Rank mismatch in elemenwise_add");
    constexpr size_t N = rank(lhs);
    return detail::elementwise_add(lhs, rhs, std::make_index_sequence<N>{});
}

} // namespace jada