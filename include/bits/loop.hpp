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

} // namespace jada