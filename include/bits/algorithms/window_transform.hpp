#pragma once

#include <algorithm>
#include <execution>

#include "include/bits/algorithms/md_for_each.hpp"

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

    auto func = [=](auto md_idx) { o_span(md_idx) = f(f2(md_idx, i_span)); };

    md_for_each(policy, all_indices(i_span), func);
}

} // namespace detail

/// @brief Applies the input unary window function to all elements of the input
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

/// @brief Applies the input unary window function to all elements of the input
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

} // namespace jada