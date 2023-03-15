#pragma once

#include <algorithm>
#include <execution>

#include "include/bits/algorithms/md_for_each.hpp"

namespace jada {

/// @brief Applies the given function object f to the result of indexing every
/// element in the span (not necessarily in order). The algorithm is executed
/// according to policy.
/// @param policy the execution policy to use. See execution policy for details.
/// @param span the input span.
/// @param f function object, to be applied to the result of span(md_idx).
template <class ExecutionPolicy, class InputSpan, class UnaryFunction>
static constexpr void
for_each(ExecutionPolicy&& policy, InputSpan span, UnaryFunction f) {

    auto F = [=](auto md_idx) { f(span(md_idx)); };
    detail::md_for_each(policy, all_indices(span), F);
}

/// @brief Applies the given function object f to the result of indexing every
/// element in the span (in order).
/// @param span the input span.
/// @param f function object, to be applied to the result of span(md_idx).
template <class InputSpan, class UnaryFunction>
static constexpr void for_each(InputSpan span, UnaryFunction f) {
    for_each(std::execution::seq, span, f);
}

/// @brief Applies the given function object f(md_idx, value) to the result of
/// indexing every element in the span (not necessarily in order).
/// @param policy the execution policy to use. See execution policy for details.
/// @param span the input span.
/// @param f binary function object where the first argument is the current
/// multidimensional index, to be applied to the result of span(md_idx).
template <class ExecutionPolicy, class InputSpan, class BinaryIndexFunction>
static constexpr void for_each_indexed(ExecutionPolicy&&   policy,
                                       InputSpan           span,
                                       BinaryIndexFunction f) {

    auto F = [=](auto md_idx) { f(md_idx, span(md_idx)); };
    detail::md_for_each(policy, all_indices(span), F);
}

/// @brief Applies the given function object f(md_idx, value) to the result of
/// indexing every element in the span (in order).
/// @param span the input span.
/// @param f function object where the first argument is the current
/// multidimensional index, to be applied to the result of span(md_idx).
template <class InputSpan, class BinaryIndexFunction>
static constexpr void for_each_indexed(InputSpan span, BinaryIndexFunction f) {

    for_each_indexed(std::execution::par_unseq, span, f);
}

} // namespace jada
