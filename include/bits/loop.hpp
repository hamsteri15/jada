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
/// @brief Returns a view of multi-dimensional index tuples
/// @param end index set of end indices
/// @return A view of index tuples from [0, end)
template <class E> static constexpr auto md_indices(E end) {

    E begin{};
    return md_indices(begin, end);

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






} // namespace jada