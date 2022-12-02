#pragma once

#include "rank.hpp"
#include "utils.hpp"
#include "integer_types.hpp"
#include "mdspan_impl.hpp"
#include <array>

namespace jada {

namespace stdex = std::experimental;

template <size_t N> using extents = stdex::dextents<size_type, N>;

/// @brief Converts array-like dimensions to extents
/// @param dims array-like object with spatial dimensions
/// @return extents object with the same dimensions
template <class T> static constexpr auto make_extent(T dims) { return extents<rank(dims)>{dims}; }

namespace detail {

// nvcc doesnt like template lamdas...
struct MakeArray {

    template <class T, size_t... Is>
    static constexpr auto make(T extents, std::index_sequence<Is...>) {
        return std::array{extents.extent(Is)...};
    }
};

} // namespace detail

/// @brief Converts extents to std::array
/// @param ext extensions to convert
/// @return std::array<size_t, Rank> array with the extents
template <class T> static constexpr auto extent_to_array(T ext) {
    return detail::MakeArray::make(ext, std::make_index_sequence<rank(ext)>{});
}

/// @brief Computes the total element count spanned by the extents
/// @param ext extensions to compute the flat size of
/// @return size_t the flat size
template <class T> static constexpr size_type flat_size(T ext) {

    size_type ret(1);
    for (size_t i = 0; i < rank(ext); ++i) { ret *= ext.extent(i); }
    return ret;
}

namespace detail {
// nvcc doesnt like template lamdas...
struct Compare {

    template <class T, class Y, size_t... Is>
    static constexpr auto op(T indices, Y extents, std::index_sequence<Is...>) {
        return std::array<bool, sizeof...(Is)>{std::get<Is>(indices) < extents.extent(Is)...};
    }
};

} // namespace detail

/// @brief Ensures that the input indices are all smaller than the input dims
/// @param indices set of indices (array/tuple) corresponding to a spatial location
/// @param dims input extensions
/// @return true if all indices in bounds, false otherwise
template <class T, class Y> static constexpr bool indices_in_bounds(T indices, Y dims) {

    auto extents = make_extent(dims);
    auto arr     = detail::Compare::op<T, decltype(extents)>(
        indices, extents, std::make_index_sequence<rank(extents)>{});
    return std::all_of(std::begin(arr), std::end(arr), [](bool b) { return b == true; });
}

} // namespace jada