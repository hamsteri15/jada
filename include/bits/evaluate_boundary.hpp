#pragma once

#include "for_each_index.hpp"
#include "index_handle.hpp"
#include <concepts>
#include <array>

namespace jada {




//TODO: add concepts of boundary condition and spatial boundary condition

static constexpr void
evaluate_boundary_condition(auto in, auto op, auto dir) {

    auto new_op = [=](auto idx) {
        const auto stencil = idxhandle_boundary_md_to_oned(in, idx, dir);
        op(stencil);
    };

    for_each_index(boundary_indices(dimensions(in), dir), new_op);
}

template <size_t N, class Span, class Op>
static constexpr void
evaluate_spatial_boundary_condition(Span in, Op op, std::array<index_type, N> dir) {

    auto new_op = [=](auto idx) {
        const auto stencil = idxhandle_boundary_md_to_oned(in, idx, dir);
        op(stencil, idx);
    };

    for_each_index(boundary_indices(dimensions(in), dir), new_op);
}

} // namespace jada