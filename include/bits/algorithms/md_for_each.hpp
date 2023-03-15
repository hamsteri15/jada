#pragma once

#include <algorithm>
#include <execution>

#include "include/bits/counting_iterator.hpp"
#include "include/bits/loop.hpp"
#include "include/bits/mdspan.hpp"
#include "include/bits/utils.hpp"

namespace jada {

namespace detail {

template <class ExecutionPolicy, class Indices, class UnaryMdIndexFunction>
static constexpr void md_for_each(ExecutionPolicy&&     policy,
                                      Indices               indices,
                                      UnaryMdIndexFunction F) {

    std::for_each_n(policy,
                    counting_iterator(index_type(0)),
                    indices.size(),
                    [=](index_type i) { F(tuple_to_array(indices[i])); });
}

} // namespace detail
} // namespace jada