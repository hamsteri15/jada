#pragma once

#include <algorithm>
#include <execution>

#include "counting_iterator.hpp"
#include "loop.hpp"
#include "mdspan.hpp"
#include "utils.hpp"

namespace jada {

template <class ExecutionPolicy, class InputSpan, class UnaryFunction>
static constexpr void
for_each(ExecutionPolicy&& policy, InputSpan span, UnaryFunction f) {

    const auto indices = all_indices(span);

    std::for_each_n(policy,
                    counting_iterator(index_type(0)),
                    indices.size(),
                    [=](index_type i) { f(span(tuple_to_array(indices[i]))); });
}
template <class InputSpan, class UnaryFunction>
static constexpr void for_each(InputSpan span, UnaryFunction f) {
    for_each(std::execution::seq, span, f);
}

template <class ExecutionPolicy, class InputSpan, class UnaryFunction>
static constexpr void
for_each_indexed(ExecutionPolicy&& policy, InputSpan span, UnaryFunction f) {

    const auto indices = all_indices(span);

    std::for_each_n(policy,
                    counting_iterator(index_type(0)),
                    indices.size(),
                    [=](index_type i) { 
                        const auto idx = tuple_to_array(indices[i]);
                        f(idx, span(idx));
                        });
}

template < class InputSpan, class UnaryFunction>
static constexpr void
for_each_indexed(InputSpan span, UnaryFunction f) {

    for_each_indexed(std::execution::par_unseq, span, f);
}

template <class ExecutionPolicy, class InputSpan, class OutputSpan, class UnaryFunction>
static constexpr void
transform(ExecutionPolicy&& policy, InputSpan i_span, OutputSpan o_span, UnaryFunction f) {

    runtime_assert(
        dimensions(i_span) == dimensions(o_span),
        "Dimension mismatch in transform()"
    );

    const auto indices = all_indices(i_span);

    std::for_each_n(policy,
                    counting_iterator(index_type(0)),
                    indices.size(),
                    [=](index_type i) { 
                        const auto idx = tuple_to_array(indices[i]);

                        o_span(idx) = f(i_span(idx));

                        //f(idx, span(idx));
                        });
}


} // namespace jada