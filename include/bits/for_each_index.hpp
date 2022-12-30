#pragma once

#include <algorithm>
#include <execution>

#include "index_conversions.hpp"
#include "counting_iterator.hpp"
#include "loop.hpp"

namespace jada {

template <class Indices, class Op>
static constexpr void for_each_index(Indices indices, Op op) {

    /*
    std::for_each(
        std::execution::par,
        std::begin(indices),
        std::end(indices),
        [=](auto tpl) { op(tpl); });
    */
    std::for_each_n(std::execution::par,
                    counting_iterator(index_type(0)),
                    indices.size(),
                    [=](index_type i) { op(indices[i]); });
    
}

template <class B, class E, class Op>
static constexpr void for_each_index(B begin, E end, Op op) {

    for_each_index(md_indices(begin, end), op);
}

template<class Span, class Op>
static constexpr void assign_for_each_index(Span s, Op op) {

    auto new_op = [=](auto idx){
        s(tuple_to_array(idx)) = op(idx);
    };

    for_each_index(all_indices(s), new_op);
}




} // namespace jada