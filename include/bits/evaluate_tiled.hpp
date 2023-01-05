#pragma once

#include "loop.hpp"
#include "mdspan.hpp"
#include "subspan.hpp"
#include "for_each_index.hpp"
#include "index_handle.hpp"

#include "algorithms.hpp"

namespace jada {


/// @brief TODO: remove this function
/// Evaluates the input tiled stencil operation 'op' on all input 'indices' of the input span
/// 'in' and stores the result to the output span 'out'.
/// @tparam Dir the direction along which the tiled stencil is formed
/// @param in input span
/// @param out output span
/// @param op the tiled stencil operation
/// @param indices the indices in which to evaluate the operation in
template <size_t Dir, class Span1, class Span2, class Op, class Indices>
void evaluate(Span1 in, Span2 out, Op op, Indices indices) {
    
    


    auto new_op = [=] (auto idx){
        const auto stencil = idxhandle_md_to_oned<Dir>(in, idx);
        out(tuple_to_array(idx)) = op(stencil);
    };

    for_each_index(indices, new_op);
    

    //tile_transform<Dir>(std::execution::par, in, out, op);


}




} // namespace jada
