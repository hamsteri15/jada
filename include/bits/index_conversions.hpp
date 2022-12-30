#pragma once

#include "rank.hpp"
#include <array>
#include <cstddef>
namespace jada {

enum class StorageOrder {
    RowMajor, // C-style
    ColMajor  // FORTRAN-style
};

///
///@brief Get the shift between consequtive indices in direction I based on the
/// storage order
///
///@tparam I the component (direction) to get the shift for
///@tparam storage the storage order
///@param dimension the maximum extent of the multidimensional array
///@return constexpr idx_t the shift in direction I
///
template <size_t I, StorageOrder storage>
static constexpr size_t get_shift(auto dim) {

    static_assert(I < rank(dim), "Shift index out of bounds");

    if constexpr (storage == StorageOrder::RowMajor) {

        return std::accumulate(std::begin(dim) + I + 1,
                               std::end(dim),
                               size_t(1),
                               std::multiplies<size_t>{});
    }

    return std::accumulate(std::begin(dim),
                           std::begin(dim) + I,
                           size_t(1),
                           std::multiplies<size_t>{});
}

///
///@brief Get the shifts between consequtive indices in all N directions of a
/// multidimensional (flattened) array
///
///@tparam storage storage order
///@param dimension the maximum extent of the multidimensional array
///@return constexpr std::array<idx_t, N> the shifts in all N directions
///
template <StorageOrder storage> static constexpr auto get_shifts(auto dim) {
    return [&]<auto... Is>(std::index_sequence<Is...>) {
        return std::array<size_type, rank(dim)>{get_shift<Is, storage>(dim)...};
    }
    (std::make_index_sequence<rank(dim)>{});
}

///
///@brief Given an array of multidimensional indices ([k,j,i] for example)
/// computes the flat index based on the storage order and dimensions
///
///@tparam storage storage order
///@param idx array of indices to flatten
///@param dimension the maximum extent of the multidimensional array
///@return constexpr idx_t the flat index
///
template <StorageOrder storage>
static constexpr index_type flatten(auto idx, auto dim) {

    runtime_assert(indices_in_bounds(idx, dim), "Index out of bounds");

    const auto mult = get_shifts<storage>(dim);

    return std::inner_product(
        std::begin(idx), std::end(idx), std::begin(mult), index_type(0));
}

///
///@brief Given dimensions and an index returns the multidimensional index based
/// on the storage order
///
///@tparam N number of dimensions
///@tparam storage storage order
///@param idx index to unflatten
///@param dims maximum extent of the multidimensional indices
///@return constexpr std::array<idx_t, N> array of multidimensional indices
///
template <StorageOrder storage>
static constexpr auto unflatten(index_type idx, auto dims) {

    runtime_assert(std::accumulate(std::begin(dims),
                                   std::end(dims),
                                   index_type(1),
                                   std::multiplies<index_type>{}) > idx,
                   "Index out of bounds"

    );

    std::array<index_type, rank(dims)> md_idx;
    auto                               mult = get_shifts<storage>(dims);

    // TODO: this is not nice, try to make some sense at some point
    if constexpr (storage == StorageOrder::RowMajor) {

        for (size_t i = 0; i < rank(dims); ++i) {
            md_idx[i] = idx / index_type(mult[i]);
            idx -= md_idx[i] * index_type(mult[i]);
        }
    }

    else {

        for (size_t i = rank(dims) - 1; int(i) >= 0; --i) {
            md_idx[i] = idx / index_type(mult[i]);
            idx -= md_idx[i] * index_type(mult[i]);
        }
    }

    return md_idx;
}

} // namespace jada