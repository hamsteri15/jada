#pragma once

#include <algorithm>
#include <execution>

#include "include/bits/algorithms/window_transform.hpp"

namespace jada {


template<size_t N, class Span, class Idx>
static constexpr auto idxhandle_md_to_oned_base(Span in, Idx center, const std::array<index_type, N>& dir)
{
    static_assert(rank(in) == rank(center),
                  "Rank mismatch in idxhandle_md_to_oned.");
   
    const auto h = make_subspan(in, center);

    return [=](index_type oned_idx) {
        std::array<index_type, N> mod_idx{};
        for (size_t i = 0; i < N; ++i){
            mod_idx[i] = dir[i] * oned_idx;
        }
        //mod_idx[Dir] = oned_idx;
        return h(mod_idx);
    }; 

}

/// @brief Creates an index handle that maps multidimensional indices to
/// one-dimensional offsets wrt. to the input index 'center' and the direction
/// Dir. For example, if center = {1,2,3}, Dir = 1, the returned index handle
/// 'f' can be indexed with f(1), f(-2), f(3) etc., where f(1) gives data of the
/// underlying span at index {1,3,3} and f(-1) gives data at index {1,1,3}.

/// @tparam Dir the direction along which the offsets are computed
/// @param in the input span from which data is indexed
/// @param center the center of the index_handle
/// @return an index handle centered at 'center'
template <size_t Dir, class Span, class Idx>
static constexpr auto idxhandle_md_to_oned(Span in, Idx center) {

    static_assert(rank(in) == rank(center),
                  "Rank mismatch in idxhandle_md_to_oned.");
    constexpr size_t N = rank(in);

    std::array<index_type, N> dir{};
    dir[Dir] = 1;
    return idxhandle_md_to_oned_base(in, center, dir);

    /*
    static_assert(rank(in) == rank(center),
                  "Rank mismatch in idxhandle_md_to_oned.");
    constexpr size_t N = rank(in);

    // A subspan centered at 'center'
    const auto h = make_subspan(in, center);

    return [=](index_type oned_idx) {
        std::array<index_type, N> mod_idx{};
        mod_idx[Dir] = oned_idx;
        return h(mod_idx);
    };
    */
}

/// @brief Applies the input unary tile function to all elements of the input
/// span and stores the result into the output span. A tile accessor is one
/// dimensional. Executed according to policy (not necessarily in order).
/// @tparam Dir the direction (index) along which the tile is created.
/// @param policy the execution policy to use. See execution policy for details.
/// @param i_span the input span.
/// @param o_span the output span.
/// @param f the unary tile operation. Example: f = [](auto accessor){return
/// accessor(0) + accessor(1);};
template <size_t Dir,
          class ExecutionPolicy,
          class InputSpan,
          class OutputSpan,
          class UnaryTileFunction>
static constexpr void tile_transform(ExecutionPolicy&& policy,
                                     InputSpan         i_span,
                                     OutputSpan        o_span,
                                     UnaryTileFunction f) {

    const auto tile = [](auto idx, auto span) {
        return idxhandle_md_to_oned<Dir>(span, idx);
    };

    detail::window_transform(policy, i_span, o_span, f, tile);
}

/// @brief Applies the input unary tile function to all elements of the input
/// span and stores the result into the output span. A tile accessor is
/// one-dimensional. Executed in order.
/// @tparam Dir the direction (index) along which the tile is created.
/// @param i_span the input span.
/// @param o_span the output span.
/// @param f the unary tile operation. Example: f = [](auto accessor){return
/// accessor(0) + accessor(1);};
template <size_t Dir,
          class InputSpan,
          class OutputSpan,
          class UnaryTileFunction>
static constexpr void
tile_transform(InputSpan i_span, OutputSpan o_span, UnaryTileFunction f) {

    tile_transform<Dir>(std::execution::seq, i_span, o_span, f);
}

} // namespace jada