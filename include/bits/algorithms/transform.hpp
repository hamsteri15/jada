#pragma once

#include <algorithm>
#include <execution>

#include "include/bits/algorithms/md_for_each.hpp"

namespace jada {

/// @brief applies the given function to a range spanned by multiple dimensions
/// and stores the result in another multidimensional range of same extent,
/// keeping the original elements order and beginning at the zeroth element of
/// i_span. Executed according to policy (not necessarily in order).
/// @param policy the execution policy to use. See execution policy for details.
/// @param i_span the input span.
/// @param o_span the output span.
/// @param f the unary function object which should return a type corresponding
/// to the value_type of o_span.
template <class ExecutionPolicy,
          class InputSpan,
          class OutputSpan,
          class UnaryFunction>
static constexpr void transform(ExecutionPolicy&& policy,
                                InputSpan         i_span,
                                OutputSpan        o_span,
                                UnaryFunction     f) {

    runtime_assert(dimensions(i_span) == dimensions(o_span),
                   "Dimension mismatch in transform()");


    auto F = [=](auto md_idx) { o_span(md_idx) = f(i_span(md_idx)); };

    detail::md_for_each(policy, all_indices(i_span), F);
}

/// @brief applies the given function to a range spanned by multiple dimensions
/// and stores the result in another multidimensional range of same extent,
/// keeping the original elements order and beginning at the zeroth element of
/// i_span. Executed in order.
/// @param i_span the input span.
/// @param o_span the output span.
/// @param f the unary function object which should return a type corresponding
/// to the value_type of o_span.
template <class InputSpan, class OutputSpan, class UnaryFunction>
static constexpr void
transform(InputSpan i_span, OutputSpan o_span, UnaryFunction f) {

    transform(std::execution::seq, i_span, o_span, f);
}

/// @brief applies the given function f(md_idx, value) to a range spanned by
/// multiple dimensions and stores the result in another multidimensional range
/// of same extent, keeping the original elements order and beginning at the
/// zeroth element of i_span. Executed according to policy (not necessarily in
/// order).
/// @param policy the execution policy to use. See execution policy for details.
/// @param i_span the input span.
/// @param o_span the output span.
/// @param f the binary function object which should return a type corresponding
/// to the value_type of o_span and take a current multidimensional index as the
/// first argument.
template <class ExecutionPolicy,
          class InputSpan,
          class OutputSpan,
          class BinaryIndexFunction>
static constexpr void transform_indexed(ExecutionPolicy&&   policy,
                                        InputSpan           i_span,
                                        OutputSpan          o_span,
                                        BinaryIndexFunction f) {

    runtime_assert(dimensions(i_span) == dimensions(o_span),
                   "Dimension mismatch in transform_indexed()");

    auto F = [=](auto md_idx) { o_span(md_idx) = f(md_idx, i_span(md_idx)); };

    detail::md_for_each(policy, all_indices(i_span), F);
}

/// @brief applies the given function f(md_idx, value) to a range spanned by
/// multiple dimensions and stores the result in another multidimensional range
/// of same extent, keeping the original elements order and beginning at the
/// zeroth element of i_span. Executed in order.
/// @param i_span the input span.
/// @param o_span the output span.
/// @param f the binary function object which should return a type corresponding
/// to the value_type of o_span and take a current multidimensional index as the
/// first argument.
template <class InputSpan, class OutputSpan, class BinaryIndexFunction>
static constexpr void
transform_indexed(InputSpan i_span, OutputSpan o_span, BinaryIndexFunction f) {

    transform_indexed(std::execution::seq, i_span, o_span, f);
}

} // namespace jada