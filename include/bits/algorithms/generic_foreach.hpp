#pragma once

#include <algorithm>
#include <execution>

#include "include/bits/counting_iterator.hpp"
#include "include/bits/loop.hpp"
#include "include/bits/mdspan.hpp"
#include "include/bits/utils.hpp"

namespace jada {

namespace detail {

static constexpr void generic_foreach(auto&& policy, auto indices, auto F) {

    std::for_each_n(policy,
                    counting_iterator(index_type(0)),
                    indices.size(),
                    [=](index_type i) { F(tuple_to_array(indices[i])); });
}

}}