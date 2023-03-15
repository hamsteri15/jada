#pragma once

#include <array>

#include "subspan.hpp"

namespace jada {

/// @brief Splits the inputs span into three subspans of widths: w_begin,
/// (original - (w_begin + w_end), w_end along the input direction (index) Dir.
/// @tparam Dir the input direction (index) along which to carry out the
/// splitting
/// @param span the input span to split
/// @param w_begin width of the first subspan
/// @param w_end width of the last subspan
/// @return a tuple of three subspans corresponding to [begin, middle, end].
template <size_t Dir>
auto split_to_subspans(auto span, size_t w_begin, size_t w_end) {

    auto b1 = std::array<size_t, rank(span)>{};
    auto e1 = [=]() {
        auto ret = dimensions(span);
        ret[Dir] = w_begin;
        return ret;
    }();

    auto b2 = [=]() {
        std::array<size_t, rank(span)> ret{};
        ret[Dir] = w_begin;
        return ret;
    }();

    auto e2 = [=]() {
        auto ret = dimensions(span);
        ret[Dir] -= w_end;
        return ret;
    }();

    auto b3 = [=]() {
        std::array<size_t, rank(span)> ret{};
        ret[Dir] = dimensions(span)[Dir] - w_end;
        return ret;
    }();
    auto e3 = dimensions(span);

    return std::make_tuple(make_subspan(span, b1, e1),
                           make_subspan(span, b2, e2),
                           make_subspan(span, b3, e3));
}






} // namespace jada
