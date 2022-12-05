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

    // using Idx = index_type;

    return [&]<size_t... Is>(std::integer_sequence<size_t, Is...>) {
        return cartesian_product(indices(index_type(std::get<Is>(begin)),
                                         index_type(std::get<Is>(end)))...);
    }
    (std::make_integer_sequence<size_t, rank(begin)>{});
}

/// @brief Returns all multi-dimensional indices spanned by the extent of the
/// input span
/// @param span the input span to query the extent from
/// @return A view of index tuples from [begin=0, extent(span) )
template <class Span> static constexpr auto all_indices(Span span) {
    auto end = dimensions(span);
    decltype(end) begin{};
    
    return md_indices(begin, end);
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

} // namespace jada