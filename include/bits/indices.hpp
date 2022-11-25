#pragma once
//#include <range/v3/view/indices.hpp>
#include "integer_types.hpp"
#include <ranges>
namespace jada {

///
/// @brief Wrapper around ranges::indices
///
/// @tparam T models integer
/// @param begin integer type begin index
/// @param end interger type end index
/// @return one-dimensional view of indices [begin, end)
///
template <class T> static constexpr auto indices(T&& begin, T&& end) {
    // TODO: No idea why the int conversion is required here. If it is removed,
    // everything just breaks.
    return std::ranges::views::iota(int(begin), int(end));
}

} // namespace jada