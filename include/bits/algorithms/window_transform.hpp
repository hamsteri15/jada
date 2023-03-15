#pragma once

#include <algorithm>
#include <execution>

#include "include/bits/counting_iterator.hpp"
#include "include/bits/loop.hpp"
#include "include/bits/mdspan.hpp"
#include "include/bits/utils.hpp"

namespace jada {


namespace detail {

template <class ExecutionPolicy,
          class InputSpan,
          class OutputSpan,
          class UnaryIndexFunction,
          class WindowMaker>
static constexpr void window_transform(ExecutionPolicy&&  policy,
                                       InputSpan          i_span,
                                       OutputSpan         o_span,
                                       UnaryIndexFunction f,
                                       WindowMaker        f2) {

    runtime_assert(dimensions(i_span) == dimensions(o_span),
                   "Dimension mismatch in shifted_transform()");

    const auto indices = all_indices(i_span);

    std::for_each_n(policy,
                    counting_iterator(index_type(0)),
                    indices.size(),
                    [=](index_type i) {
                        const auto idx = tuple_to_array(indices[i]);
                        o_span(idx)    = f(f2(idx, i_span));
                    });
}

} // namespace detail

/// @brief applies the input unary window function to all elements of the input
/// span and stores the result into the output span. A window accessor has the
/// same dimensions as the input spans. Executed according to policy (not
/// necessarily in order).
/// @param policy the execution policy to use. See execution policy for details.
/// @param i_span the input span.
/// @param o_span the output span.
/// @param f the unary window operation. Example: f = [](auto accessor){return
/// accessor(1,0) + accessor(-1,0);};
template <class ExecutionPolicy,
          class InputSpan,
          class OutputSpan,
          class UnaryWindowFunction>
static constexpr void window_transform(ExecutionPolicy&&   policy,
                                       InputSpan           i_span,
                                       OutputSpan          o_span,
                                       UnaryWindowFunction f) {

    const auto window = [](auto idx, auto span) {
        return make_subspan(span, idx);
    };

    detail::window_transform(policy, i_span, o_span, f, window);
}

/// @brief applies the input unary window function to all elements of the input
/// span and stores the result into the output span. A window accessor has the
/// same dimensions as the input spans. Executed in order.
/// @param i_span the input span.
/// @param o_span the output span.
/// @param f the unary window operation. Example: f = [](auto accessor){return
/// accessor(1,0) + accessor(-1,0);}
template <class InputSpan, class OutputSpan, class UnaryWindowFunction>
static constexpr void
window_transform(InputSpan i_span, OutputSpan o_span, UnaryWindowFunction f) {

    window_transform(std::execution::seq, i_span, o_span, f);
}

}