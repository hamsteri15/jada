#pragma once

#include "subspan.hpp"
#include "utils.hpp"

namespace jada {

/// @brief Creates an index handle which maps multidimensional indices to
/// multidimensional indices centered at the input center center. For example,
/// the returned handle created at indices center={1,2,3} can be indexed with
/// index {0,-1, 0} giving the data from the underlying span at index {1,1,3}.
/// Essentially, this handle just shifts the center of the index to some
/// constant location='center'.

/// @param in the input span from which data is indexed
/// @param center the center of the index_handle
/// @return an index handle centered at 'center'
template <class Span, class Idx>
static constexpr auto idxhandle_md_to_md(Span in, Idx center) {

    static_assert(rank(in) == rank(center), "Rank mismatch in idxhandle_md_to_md.");

    //TODO: make it so that md_idx can be a parameter pack also
    return [=](auto md_idx) {
        auto new_span = make_subspan(in, center);
        return new_span(tuple_to_array(md_idx));
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

    static_assert(rank(in) == rank(center), "Rank mismatch in idxhandle_md_to_oned.");
    constexpr size_t N = rank(in);

    return [=](index_type oned_idx) {
        std::array<index_type, N> mod_idx{};
        mod_idx[Dir] = oned_idx;
        const auto h = idxhandle_md_to_md(in, center);
        return h(mod_idx);
    };
}


template <class Span, class Idx, class Dir>
static constexpr auto idxhandle_boundary_md_to_oned(Span in, Idx center, Dir dir) {

    static_assert(rank(in) == rank(center), "Rank mismatch in idxhandle_boundary_md_to_oned.");
    static_assert(rank(in) == rank(dir), "Rank mismatch in idxhandle_boundary_md_to_oned.");

    constexpr size_t N = rank(in);

    return [=](index_type oned_idx) {

        std::array<index_type, N> mod_idx{};

        for (size_t i = 0; i < N; ++i){
            if (dir[i] == 1){
                mod_idx[i] = oned_idx;
            }
            if (dir[i] == -1){
                mod_idx[i] = -oned_idx;
            }
        }        
        const auto h = idxhandle_md_to_md(in, center);
        return h(mod_idx);
    };
}



} // namespace jada