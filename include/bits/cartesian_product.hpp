#pragma once

#include "cartesian_product_impl.hpp"

namespace jada {

/// @brief Wrapper around ranges::cartesian_product
/// @tparam ...Rngs Pack of range types
/// @param ...rngs Pack of range values
/// @return A view of size = size(rng1) * size(rng2)... containing tuples of [(rng1[0], rng2[0]...),
/// ...]
template <class... Rngs> constexpr auto cartesian_product(Rngs&&... rngs) {
    return tl::views::cartesian_product(rngs...);
}

} // namespace jada