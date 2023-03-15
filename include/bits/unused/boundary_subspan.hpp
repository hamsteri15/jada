#pragma once

#include "subspan.hpp"
#include "integer_types.hpp"

namespace jada{

static constexpr auto boundary_indices(auto  dims,
                                       auto dir) {

    static_assert(rank(dims) == rank(dir),
        "Dimension mismatch in boundary_indices.");
    // runtime_assert(valid_direction(direction), "Invalid boundary direction in
    // for_each_boundary_index");


    std::array<index_type, rank(dims)> begin{};
    std::array<index_type, rank(dims)> end{};

    for (size_t i = 0; i < rank(dims); ++i) { end[i] = index_type(dims[i]); }

    for (size_t i = 0; i < rank(dims); ++i) {
        if (dir[i] == 1) { begin[i] = index_type(dims[i]) - 1; }
        if (dir[i] == -1) { end[i] = 1; }
    }

    return std::make_pair(begin, end);

}

/*
//TODO: This doesnt belong here
template <typename T>
static constexpr int signum(T val) {
    return (T(0) < val) - (val < T(0));
}



template <int Dir, size_t N>
static constexpr auto boundary_indices(std::array<size_type, N>  dims) {


    std::array<index_type, N> dir{};
    dir[size_t(Dir)] = signum(Dir);
    return boundary_indices(dims, dir);


}
*/

template<class Span, class Dir>
static constexpr auto make_boundary_subspan(Span span, Dir dir){

    static_assert(rank(span) == rank(dir), "Dimension mismatch in make_boundary_subspan.");

    auto [begin, end] = boundary_indices(dimensions(span), dir);

    return make_subspan(span, begin, end);

}

}
