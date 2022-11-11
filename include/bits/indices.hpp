#pragma once
#include <range/v3/view/indices.hpp>

namespace jada {


///
/// @brief Wrapper around ranges::indices
///
/// @tparam T models integer
/// @param begin integer type begin index
/// @param end interger type end index
/// @return one-dimensional view of indices [begin, end)
///
template<class T>
static constexpr auto indices(T&& begin, T&& end) {
    return ranges::views::indices(begin, end);
}

} // namespace jada