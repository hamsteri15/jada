#pragma once

#include "for_each_index.hpp"
#include "loop.hpp"
#include "utils.hpp"

namespace jada {

template <size_t N>
static constexpr bool valid_direction(std::array<index_type, N> direction) {

    size_t sum = 0;
    for (size_t i = 0; i < N; ++i) { sum += std::abs(direction[i]); }
    return sum == 1;
}
template <size_t N>
static constexpr auto boundary_indices(std::array<size_type, N>  dims,
                                       std::array<index_type, N> direction) {
    // runtime_assert(valid_direction(direction), "Invalid boundary direction in
    // for_each_boundary_index");
    std::array<index_type, N> begin{};
    std::array<index_type, N> end{};

    for (size_t i = 0; i < N; ++i) { end[i] = index_type(dims[i]); }

    for (size_t i = 0; i < N; ++i) {
        if (direction[i] == 1) { begin[i] = index_type(dims[i]) - 1; }
        if (direction[i] == -1) { end[i] = 1; }
    }

    return md_indices(begin, end);
}

template <size_t N, class Op>
static constexpr void for_each_boundary_index(
    std::array<index_type, N> direction, std::array<size_type, N> dims, Op op) {

    for_each_index(boundary_indices(dims, direction), op);
}

template <class Span, size_t N, class Op>
static constexpr void
for_each_boundary_index(Span s, std::array<index_type, N> direction, Op op) {

    for_each_boundary_index(dimensions(s), direction, op);
}

} // namespace jada