#pragma once

#include <array>

#include "include/bits/communication/box.hpp"
#include "include/bits/utils.hpp"

namespace jada {

template <size_t N> struct Block {

private:
    static Box<N> make_outer(const Box<N>&             inner,
                             std::array<index_type, N> begin_halos,
                             std::array<index_type, N> end_halos) {
        Box<N> ret(inner);
        ret.expand(begin_halos, end_halos);
        return ret;
    }

    static Box<N> make_outer(const Box<N>& inner, index_type halos) {
        Box<N> ret(inner);
        ret.expand(halos);
        return ret;
    }

public:
    Box<N> m_inner;
    Box<N> m_outer;

    Block() = default;

    Block(Box<N> inner, index_type halos)
        : m_inner(inner)
        , m_outer(make_outer(inner, halos)) {
        runtime_assert(m_inner.is_valid(), "Invalid box");
        runtime_assert(m_outer.is_valid(), "Invalid box");
    }

    Block(Box<N>                    inner,
          std::array<index_type, N> begin_halos,
          std::array<index_type, N> end_halos)
        : m_inner(inner)
        , m_outer(make_outer(inner, begin_halos, end_halos)) {
        runtime_assert(m_inner.is_valid(), "Invalid box");
        runtime_assert(m_outer.is_valid(), "Invalid box");
    }

    size_t outer_size() const { return m_outer.size(); }
    size_t inner_size() const { return m_inner.size(); }

    Box<N> get_inner_box() const { return m_inner; }
    Box<N> get_outer_box() const { return m_outer; }
};

} // namespace jada
