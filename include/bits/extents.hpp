#pragma once

#include "rank.hpp"
#include "utils.hpp"
#include <array>
#include <experimental/mdspan>

namespace jada {

namespace stdex = std::experimental;

template <size_t N> using extents = stdex::dextents<size_t, N>;

/// @brief Converts array-like dimensions to extents
/// @param dims array-like object with spatial dimensions
/// @return extents object with the same dimensions
template<class T>
static constexpr auto make_extent(T dims) { return extents<rank(dims)>{dims}; }

/// @brief Converts extents to std::array
/// @param ext extensions to convert
/// @return std::array<size_t, Rank> array with the extents
template<class T>
static constexpr auto extent_to_array(T ext) {

    using Idx = typename decltype(ext)::index_type;

    auto f = [=]<size_t... Is>(std::index_sequence<Is...>) {
        return std::array<Idx, sizeof...(Is)>{ext.extent(Is)...};
    };
    return f(std::make_index_sequence<rank(ext)>{});
}

/// @brief Computes the total element count spanned by the extents
/// @param ext extensions to compute the flat size of
/// @return size_t the flat size
template<class T>
static constexpr size_t flat_size(T ext) {

    size_t ret(1);
    for (size_t i = 0; i < rank(ext); ++i) { ret *= ext.extent(i); }
    return ret;
}

/// @brief Ensures that the input indices are all smaller than the input dims
/// @param indices set of indices (array/tuple) corresponding to a spatial location
/// @param dims input extensions
/// @return true if all indices in bounds, false otherwise
template<class T, class Y>
static constexpr bool indices_in_bounds(T indices, Y dims) {

    auto extents = make_extent(dims);

    auto f = [=]<size_t... Is>(std::index_sequence<Is...>) {
        return std::array<bool, sizeof...(Is)>{std::get<Is>(indices) < extents.extent(Is)...};
    };

    auto arr = f(std::make_index_sequence<rank(extents)>{});

    return std::all_of(std::begin(arr), std::end(arr), [](bool b) { return b == true; });
}

} // namespace jada