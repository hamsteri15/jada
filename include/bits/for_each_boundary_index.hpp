#pragma once

#include "for_each_index.hpp"
#include "loop.hpp"
#include "utils.hpp"

namespace jada {


template <size_t N, class Op>
static constexpr void for_each_boundary_index(
    std::array<index_type, N> direction, std::array<size_type, N> dims, Op op) {

    for_each_index(boundary_indices(dims, direction), op);
}


} // namespace jada