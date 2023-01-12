#pragma once

#include "cartesian_product.hpp"
#include "utils.hpp"

namespace jada {

template <class integer_t>
static inline std::vector<integer_t> all_factors_of(integer_t n) {

    std::vector<integer_t> ret;

    for (integer_t i = 1; i <= n; ++i) {
        if ((n % i) == 0) { ret.push_back(i); }
    }

    return ret;
}

template <size_t N, class T> static constexpr auto make_tuple(T t) {

    std::array<T, N> arr{};
    for (auto& a : arr) { a = t; }

    auto tuple_maker = [&]<auto... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(arr[Is]...);
    };

    return tuple_maker(std::make_integer_sequence<size_t, N>{});
}

template <size_t N> auto get_candidates(size_t n) {

    auto factors = all_factors_of(n);

    runtime_assert(factors.size() > 0, "Cannot get factors.");

    auto f_view = std::ranges::views::all(factors);

    auto candidate_view =
        std::apply([](auto... v) { return cartesian_product(v...); },
                   make_tuple<N>(f_view));

    auto correct = [=](auto v) {
        return n == std::accumulate(
                        v.begin(), v.end(), size_t(1), std::multiplies{});
    };

    std::vector<std::array<size_t, N>> candidates;

    for (auto tpl : candidate_view) {

        auto candidate = tuple_to_array(tpl);

        if (correct(candidate)) { candidates.push_back(candidate); }
    }
    return candidates;
}

template <size_t N> auto divide_equally(size_t n) {

    auto candidates = get_candidates<N>(n);

    auto sum_abs_diff = [](auto arr) {
        size_t diff = 0;

        for (auto e1 : arr) {
            for (auto e2 : arr) {
                diff += size_t(std::abs(long(e1) - long(e2)));
            }
        }

        return diff;
    };

    auto is_more_square = [=](const auto& lhs, const auto& rhs) {
        return sum_abs_diff(lhs) < sum_abs_diff(rhs);
    };

    return *std::min_element(
        candidates.begin(), candidates.end(), is_more_square);
}

} // namespace jada