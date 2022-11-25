#pragma once
//#include <range/v3/view/indices.hpp>
#include <ranges>
#include "integer_types.hpp"
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
    //return ranges::views::iota(begin, end);
    //return std::ranges::views::iota(static_cast<index_type>(begin), static_cast<index_type>(end));
    //return std::ranges::views::iota(static_cast<int>(begin), static_cast<int>(end));
    //TODO: fix!
    return std::ranges::views::iota(int(begin), int(end));
    //return std::ranges::iota_view(begin, end);
}

} // namespace jada