#pragma once

#include "for_each_index.hpp"
#include "index_handle.hpp"
#include <array>

namespace jada {

template <size_t N, class Span, class Op>
static constexpr void
evaluate_boundary(Span in, Op op, std::array<index_type, N> dir) {

    auto new_op = [=](auto idx) {
        const auto stencil = idxhandle_boundary_md_to_oned(in, idx, dir);
        op(stencil);
    };

    for_each_index(boundary_indices(dimensions(in), dir), new_op);
}

} // namespace jada