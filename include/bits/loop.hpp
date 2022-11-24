#pragma once

#include <array>

#include "cartesian_product.hpp"
#include "indices.hpp"
#include "mdspan.hpp"
#include "rank.hpp"

namespace jada {

namespace detail{

    //nvcc doesnt like template lambdas...
    struct CallProducts{
        template<class B, class E, size_t... Is>
        static constexpr auto call(B begin, E end, std::index_sequence<Is...>){
            return cartesian_product(indices(std::get<Is>(begin), std::get<Is>(end))...);
        }
    };

}

/// @brief Returns a view of multi-dimensional index tuples
/// @param begin index set of begin indices
/// @param end index set of end indices
/// @return A view of index tuples from [begin, end)
template<class B, class E>
static constexpr auto md_indices(B begin, E end) {

    return detail::CallProducts::call(begin, end, std::make_index_sequence<rank(begin)>{});
    /*
    return [&]<Idx... Is>(std::integer_sequence<Idx, Is...>) {
        return cartesian_product(indices(std::get<Is>(begin), std::get<Is>(end))...);
    }
    (std::make_integer_sequence<Idx, Idx(rank(begin))>{});
    */
}

/// @brief Returns all multi-dimensional indices spanned by the extent of the input span
/// @param span the input span to query the extent from
/// @return A view of index tuples from [begin=0, extent(span) )
template<class Span>
static constexpr auto all_indices(Span span) {

    //TODO: make this less insane
    //nvc++ wants integers...
    using Idx = int;

    std::array<Idx, rank(span)> begin{};
    std::array<Idx, rank(span)> end{};

    auto dims = dimensions(span);
    for (size_t i = 0; i < rank(span); ++i){
        end[i] = Idx(dims[i]); 
    }
    return md_indices(begin, end);

    //using Idx = typename decltype(span)::index_type;
    //return md_indices(std::array<Idx, rank(span)>{}, dimensions(span));
}

} // namespace jada