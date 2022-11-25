#pragma once

#include <algorithm>
#include <iterator>

namespace jada {

template <class T> static constexpr void swap(T& a, T& b) {
    T c(std::move(a));
    a = std::move(b);
    b = std::move(c);
}

template <class ForwardIt1, class ForwardIt2>
constexpr void iter_swap(ForwardIt1 a, ForwardIt2 b) {
    jada::swap(*a, *b);
}

template <class ForwardIt, class Compare>
static constexpr ForwardIt
is_sorted_until(ForwardIt first, ForwardIt last, Compare comp) {
    if (first != last) {
        ForwardIt next = first;
        while (++next != last) {
            if (comp(*next, *first)) return next;
            first = next;
        }
    }
    return last;
}

template <class ForwardIt, class T>
static constexpr ForwardIt
upper_bound(ForwardIt first, ForwardIt last, const T& value) {
    ForwardIt                                                 it;
    typename std::iterator_traits<ForwardIt>::difference_type count = 0;
    typename std::iterator_traits<ForwardIt>::difference_type step  = 0;

    count = std::distance(first, last);

    while (count > 0) {
        it   = first;
        step = count / 2;
        std::advance(it, step);

        if (!(value < *it)) {
            first = ++it;
            count -= step + 1;
        } else
            count = step;
    }

    return first;
}

template <class BidirIt>
static constexpr void reverse(BidirIt first, BidirIt last) {
    using iter_cat = typename std::iterator_traits<BidirIt>::iterator_category;

    // Tag dispatch, e.g. calling reverse_impl(first, last, iter_cat()),
    // can be used in C++14 and earlier modes.
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag,
                                    iter_cat>) {
        if (first == last) return;

        for (--last; first < last; (void)++first, --last)
            jada::iter_swap(first, last);
    } else
        while (first != last && first != --last) jada::iter_swap(first++, last);
}

/*
template<class ForwardIt>
constexpr //< since C++20
ForwardIt is_sorted_until(ForwardIt first, ForwardIt last)
{
    return is_sorted_until(first, last, std::less<>());
}
*/

template <class BidirIt>
static constexpr bool next_permutation(BidirIt first, BidirIt last) {

    auto f       = [](auto lhs, auto rhs) { return lhs < rhs; };
    auto r_first = std::make_reverse_iterator(last);
    auto r_last  = std::make_reverse_iterator(first);
    auto left    = jada::is_sorted_until(r_first, r_last, f);
    if (left != r_last) {
        auto right = jada::upper_bound(r_first, left, *left);
        jada::iter_swap(left, right);
    }
    jada::reverse(left.base(), last);
    return left != r_last;
}

} // namespace jada