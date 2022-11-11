#pragma once

#include "loop.hpp"
#include "mdspan.hpp"
#include "subspan.hpp"
#include "tiled_stencil.hpp"

namespace jada {

/*
#ifdef __CUDACC__
    namespace algos = thrust;
else
    namespace algos = std;
#endif
*/
namespace algos = std;


/// @brief Evaluates the input tiled stencil operation 'op' on all input 'indices' of the input span
/// 'in' and stores the result to the output span 'out'.
/// @tparam Dir the direction along which the tiled stencil is formed
/// @param in input span
/// @param out output span
/// @param op the tiled stencil operation
/// @param indices the indices in which to evaluate the operation in
template <size_t Dir, class Span1, class Span2, class Op, class Indices> 
void evaluate(Span1 in, Span2 out, Op op, Indices indices) {

    algos::for_each(algos::begin(indices), algos::end(indices), [=](auto idx) {
        auto stencil = make_tiled_subspan<Dir>(in, idx);
        // TODO: get rid of the tuple conversion on the LHS
        out(tuple_to_array(idx)) = op(stencil);
    });
}

/// @brief Evaluates the input 'op' on all possible indices based on the padding requirement of the
/// input operation. If the padding requirement of the operation is 2 and the spans are
/// one-dimensional, the operation is evaluated at indices [2, size(in) - 2]. The operations are
/// evaluated on the input span 'in' and results are stored in the same indices of the output span
/// 'out'.
/// @tparam Dir the direction along which the tiled stencil is formed
/// @param in input span
/// @param out output span
/// @param op the tiled stencil operation
template <size_t Dir, class Span1, class Span2, class Op>
 void evaluate_tiled(Span1 in, Span2 out, Op op) {
    static_assert(rank(in) == rank(out), "Rank mismatch in evaluate tiled.");

    runtime_assert(extent(in) == extent(out), "Dimension mismatch");

    auto          end = dimensions(in);
    decltype(end) begin{};

    std::get<Dir>(begin) += op.padding;
    std::get<Dir>(end) -= op.padding;

    evaluate<Dir>(in, out, op, md_indices(begin, end));
}

} // namespace jada
