#pragma once
#include "extents.hpp"
#include "utils.hpp"
#include <iostream>

namespace jada {

namespace stdex = std::experimental;

template <class ElementType, size_t N, class Layout>
using span_base = stdex::mdspan<ElementType, extents<N>, Layout>;

template <class ElementType, size_t N>
using span = span_base<ElementType, N, stdex::layout_right>;

// TODO: rename this function as extents and rename the extents object something
// else
/// @brief Returns the extent of the input span
/// @tparam Span a mdspan type
/// @param span the input span to query the extent of
/// @return extent of the input span
template <class Span> static constexpr auto extent(const Span& span) {
    return span.extents();
}

/// @brief Returns the dimensions of the input span as an std::array
/// @tparam Span a mdspan type
/// @param span the input span to query the dimensions of
/// @return std::array of dimensions spanned by the input span
template <class Span> static constexpr auto dimensions(const Span& span) {
    return extent_to_array(extent(span));
}

/// @brief Makes a multi-dimensional span of the input container
/// @tparam Container a container which has a value_type, size() and data()
/// members
/// @param c the input container
/// @param dims dimensions of the multi-dimensional span
/// @return a multi-dimensional span
template <class Container, class Dims>
static constexpr auto make_span(Container& c, Dims dims) {
    using value_type = typename Container::value_type;
    auto ext         = make_extent(dims);
    runtime_assert(flat_size(ext) == std::size(c),
                   "Dimension mismatch in make_span");
    return span<value_type, rank(ext)>(std::data(c), ext);
}

/// @brief Makes a multi-dimensional span of the input container
/// @tparam Container a container which has a value_type, size() and data()
/// members
/// @param c the input container
/// @param dims dimensions of the multi-dimensional span
/// @return a multi-dimensional span
template <class Container, class Dims>
static constexpr auto make_span(const Container& c, Dims dims) {
    using value_type = const typename Container::value_type;
    auto ext         = make_extent(dims);
    runtime_assert(flat_size(ext) == std::size(c),
                   "Dimension mismatch in make_span");
    return span<value_type, rank(ext)>(std::data(c), ext);
}





} // namespace jada