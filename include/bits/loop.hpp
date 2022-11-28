#pragma once

#include <array>

#include "cartesian_product.hpp"
#include "indices.hpp"
#include "mdspan.hpp"
#include "rank.hpp"

namespace jada {


/// @brief Returns a view of multi-dimensional index tuples
/// @param begin index set of begin indices
/// @param end index set of end indices
/// @return A view of index tuples from [begin, end)
template <class B, class E> static constexpr auto md_indices(B begin, E end) {

    using Idx = size_type;

    return [&]<Idx... Is>(std::integer_sequence<Idx, Is...>) {
        return cartesian_product(indices(std::get<Is>(begin),
    std::get<Is>(end))...);
    }
    (std::make_integer_sequence<Idx, Idx(rank(begin))>{});
    
}

/// @brief Returns all multi-dimensional indices spanned by the extent of the
/// input span
/// @param span the input span to query the extent from
/// @return A view of index tuples from [begin=0, extent(span) )
template <class Span> static constexpr auto all_indices(Span span) {

     return md_indices(std::array<size_type, rank(span)>{}, dimensions(span));
}

/*
template <size_t N>
static constexpr bool valid_direction(std::array<index_type, N> direction) {

    size_t sum = 0;
    for (size_t i = 0; i < N; ++i) { sum += std::abs(direction[i]); }
    return sum == 1;
}
*/

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

} // namespace jada