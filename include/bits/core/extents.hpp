#pragma once

#include <iostream>
#include "integer_types.hpp"
#include "mdspan_impl.hpp"
#include "rank.hpp"
#include "utils.hpp"
#include <array>

namespace jada {

namespace stdex = std::experimental;

template <size_type N> using extents = stdex::dextents<size_type, N>;


template <size_type N>
std::ostream& operator<<(std::ostream& os, extents<N> v) {

    os << "{ ";
    for (size_t i = 0; i < N; ++i){
        os << v.extent(i) << " ";
    }
    os << "}";
    return os;
}




/// @brief Converts array-like dimensions to extents
/// @param dims array-like object with spatial dimensions
/// @return extents object with the same dimensions
template <class T> static constexpr auto make_extent(T dims) {
    return extents<rank(dims)>{dims};
}

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
/// @param ext extents to convert
/// @return std::array<A, rank(extents)> of extents
static constexpr auto extent_to_array(auto ext) {
    return detail::MakeArray::make(ext, std::make_index_sequence<rank(ext)>{});
}

/// @brief Overload for std::arrays which simply returns the input
/// @param ext extents to return
/// @return the input extents
template <size_t N, class Int>
static constexpr auto extent_to_array(std::array<Int, N> ext) {
    return ext;
}

/// @brief Computes the total element count spanned by the extents
/// @param ext (array-like) extensions to compute the flat size of
/// @return size_t the flat size
template <class T> static constexpr size_type flat_size(T ext) {

    auto dims = extent_to_array(ext);

    size_type ret(1);
    for (size_t i = 0; i < rank(ext); ++i) { ret *= dims[i]; }
    return ret;
}

/// @brief Adds padding to the input extent.
/// @param extent unpadded extent
/// @param begin_padding amount of padding to add to the begininng
/// @param end_padding amount of padding to add to the end
/// @return a padded extent
template <class T, class P>
static constexpr auto add_padding(T extent, P begin_padding, P end_padding) {

    auto dims = extent_to_array(extent);
    auto beg = extent_to_array(begin_padding);
    auto end = extent_to_array(end_padding);
    for (size_t i = 0; i < rank(extent); ++i) {
        dims[i] += size_type(beg[i] + end[i]);
    }
    return make_extent(dims);
}

/// @brief Adds padding to the input extent
/// @param extent unpadded extent
/// @param padding the amount of padding to add on begin and end of the extent
/// @return a padded extent: padded_extent[i] = extent[i] + 2*padding[i]
template <class T, class P>
static constexpr auto add_padding(T extent, P padding) {
    return add_padding(extent, padding, padding);
}

namespace detail {
// nvcc doesnt like template lamdas...
struct Compare {

    template <class T, class Y, size_t... Is>
    static constexpr auto op(T indices, Y extents, std::index_sequence<Is...>) {
        return std::array<bool, sizeof...(Is)>{
            index_type(std::get<Is>(indices)) <
            index_type(extents.extent(Is))...};
    }
};

} // namespace detail

/// @brief Ensures that the input indices are all smaller than the input dims
/// @param indices set of indices (array/tuple) corresponding to a spatial
/// location
/// @param dims input extensions
/// @return true if all indices in bounds, false otherwise
template <class T, class Y>
static constexpr bool indices_in_bounds(T indices, Y dims) {

    auto extents = make_extent(dims);
    auto arr     = detail::Compare::op<T, decltype(extents)>(
        indices, extents, std::make_index_sequence<rank(extents)>{});
    return std::all_of(
        std::begin(arr), std::end(arr), [](bool b) { return b == true; });
}

} // namespace jada