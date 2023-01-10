#pragma once

#include <algorithm>
#include <execution>

#include "counting_iterator.hpp"
#include "index_conversions.hpp"
#include "loop.hpp"
#include "mdspan.hpp"
#include "utils.hpp"

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

    const auto indices = all_indices(span);

    std::for_each_n(policy,
                    counting_iterator(index_type(0)),
                    indices.size(),
                    [=](index_type i) { f(span(tuple_to_array(indices[i]))); });
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

    const auto indices = all_indices(span);

    std::for_each_n(policy,
                    counting_iterator(index_type(0)),
                    indices.size(),
                    [=](index_type i) {
                        const auto idx = tuple_to_array(indices[i]);
                        f(idx, span(idx));
                    });
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

    const auto indices = all_indices(i_span);

    std::for_each_n(policy,
                    counting_iterator(index_type(0)),
                    indices.size(),
                    [=](index_type i) {
                        const auto idx = tuple_to_array(indices[i]);
                        o_span(idx)    = f(i_span(idx));
                    });
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

    const auto indices = all_indices(i_span);

    std::for_each_n(policy,
                    counting_iterator(index_type(0)),
                    indices.size(),
                    [=](index_type i) {
                        const auto idx = tuple_to_array(indices[i]);
                        o_span(idx)    = f(idx, i_span(idx));
                    });
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

/// @brief applies the input unary tile function to all elements of the input
/// span and stores the result into the output span. A tile accessor is one
/// dimensional. Executed according to policy (not necessarily in order).
/// @tparam Dir the direction (index) along which the tile is created.
/// @param policy the execution policy to use. See execution policy for details.
/// @param i_span the input span.
/// @param o_span the output span.
/// @param f the unary tile operation. Example: f = [](auto accessor){return
/// accessor(0) + accessor(1);};
template <size_t Dir,
          class ExecutionPolicy,
          class InputSpan,
          class OutputSpan,
          class UnaryTileFunction>
static constexpr void tile_transform(ExecutionPolicy&& policy,
                                     InputSpan         i_span,
                                     OutputSpan        o_span,
                                     UnaryTileFunction f) {

    const auto tile = [](auto idx, auto span) {
        return idxhandle_md_to_oned<Dir>(span, idx);
    };

    detail::window_transform(policy, i_span, o_span, f, tile);
}

/// @brief applies the input unary tile function to all elements of the input
/// span and stores the result into the output span. A tile accessor is
/// one-dimensional. Executed in order.
/// @tparam Dir the direction (index) along which the tile is created.
/// @param i_span the input span.
/// @param o_span the output span.
/// @param f the unary tile operation. Example: f = [](auto accessor){return
/// accessor(0) + accessor(1);};
template <size_t Dir,
          class InputSpan,
          class OutputSpan,
          class UnaryTileFunction>
static constexpr void
tile_transform(InputSpan i_span, OutputSpan o_span, UnaryTileFunction f) {

    tile_transform<Dir>(std::execution::seq, i_span, o_span, f);
}

} // namespace jada