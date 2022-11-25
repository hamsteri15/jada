#pragma once

#include "for_each_index.hpp"
#include "utils.hpp"

namespace jada{

template<size_t N>
static constexpr bool valid_direction(std::array<index_type, N> direction){

    size_t sum = 0;
    for (size_t i = 0; i < N; ++i){
        sum += std::abs(direction[i]);
    }
    return sum == 1;

}

/*
template<size_t N, class Op>
static constexpr void for_each_boundary_index(std::array<index_type, N> direction, std::array<size_type, N> dims, Op op){

    runtime_assert(valid_direction(direction), "Invalid boundary direction in for_each_boundary_index");

    std::array<index_type, N> begin{};
    std::array<index_type, N> end{};

    for (size_t i = 0; i < N; ++i){
        
        if (std::abs(direction[i]) == )
        
        //begin[i] = 
        //end[i] = 
    }

    for_each_index(begin, end, op);

}
*/

}