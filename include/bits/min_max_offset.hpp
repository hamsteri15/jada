#pragma once

#include <array>
#include <iterator>

namespace jada {

struct Tester {

    index_type m_max = 0;
    index_type m_min = 0;

    constexpr auto operator()(auto i) {

        if (i >= m_max) { m_max = index_type(i); }

        if (i < m_min) { m_min = index_type(i); }

        return 1;
    }

    constexpr auto max() const { return m_max; }
    constexpr auto min() const { return m_min; }
};

static constexpr auto min_max_offset(auto op) {
    Tester                         t;
    std::reference_wrapper<Tester> tt(t);
    op(tt);
    return std::make_pair(t.min(), t.max());
}




} // namespace jada
