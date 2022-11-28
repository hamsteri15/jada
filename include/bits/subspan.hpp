#pragma once

#include "mdspan.hpp"
#include "rank.hpp"
#include <array>

namespace jada {

namespace detail {

// nvcc doesnt like template lamdas...
struct TupleMaker2 {

    template <class Span, class B, class E, size_t... Is>
    static constexpr auto
    make(Span span, B begin, E end, std::index_sequence<Is...>) {
        return std::make_tuple(
            span, std::make_pair(std::get<Is>(begin), std::get<Is>(end))...);
    }
};



} // namespace detail

/// @brief Creates a subspan from the input span based on the index sets begin
/// and end
/// @param span the input span to create the subspan from
/// @param begin set of indices describing the beginning of the subpan
/// @param end set of indices describing the end of the subspan
/// @return a subspan form [begin, end)
template <class Span, class B, class E>
static constexpr auto make_subspan(Span span, B begin, E end) {

    static_assert(rank(span) == rank(begin),
                  "Dimension mismatch in make_subspan");
    static_assert(rank(begin) == rank(end),
                  "Dimension mismatch in make_subspan");

    auto tpl = detail::TupleMaker2::make(
        span, begin, end, std::make_index_sequence<rank(span)>{});
    auto callable = [](auto... params) { return stdex::submdspan(params...); };

    return std::apply(callable, tpl);
}

/// @brief Creates a subspan centered at 'center'
/// @param span the input span to create the subspan from 
/// @param center center of the new subspan
/// @return a subspan centered at 'center'
template <class Span, class C>
static constexpr auto make_subspan(Span span, C center) {

    return make_subspan(span, center, center);
}


} // namespace jada