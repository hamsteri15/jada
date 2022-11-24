#pragma once

#include <algorithm>
#include <execution>

#include "counting_iterator.hpp"

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

   
    std::for_each_n(
        std::execution::par,
        counting_iterator(0),
        indices.size(),
        [=](int i){
            op(indices[i]);
        }

    );
    
}

} // namespace jada