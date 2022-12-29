#pragma once

#include "jada.hpp"
#include <array>
#include <iomanip>
#include <iostream>

using namespace jada;

enum Dir { x = 1, y = 0 };

struct Grid {

    Grid(
         size_t nx,
         size_t ny,
         size_t padding_x,
         size_t padding_y) {
        std::get<Dir::x>(m_L)           = 1.0;
        std::get<Dir::y>(m_L)           = 1.0;
        std::get<Dir::x>(m_point_count) = nx;
        std::get<Dir::y>(m_point_count) = ny;
        std::get<Dir::x>(m_padding)     = padding_x;
        std::get<Dir::y>(m_padding)     = padding_y;
    }

    auto coord(auto idx_tpl) const {
        std::array<double, 2> ret{};

        index_type i = std::get<Dir::x>(idx_tpl);
        index_type j = std::get<Dir::y>(idx_tpl);
        double     x = 0.5 * delta(Dir::x) + i * delta(Dir::x);
        double     y = 0.5 * delta(Dir::y) + j * delta(Dir::y);

        ret[Dir::x] = x;
        ret[Dir::y] = y;
        return ret;
    }

    auto L(Dir dir) const { return m_L[dir]; }

    auto delta(Dir dir) const { return m_L[dir] / double(m_point_count[dir]); }

    auto extent() const { return m_point_count; }

    auto padding() const { return m_padding; }

    auto padded_extent() const {
        std::array<size_t, 2> ret(m_point_count);
        for (size_t i = 0; i < ret.size(); ++i) { ret[i] += m_padding[i] * 2; }
        return ret;
    }

    size_t padded_size() const {
        size_t size = 1;
        auto   ext  = this->padded_extent();
        for (size_t i = 0; i < 2; ++i) { size *= ext[i]; }
        return size;
    }

    size_t size() const {
        size_t ret = 1;
        auto   ext = this->extent();
        for (size_t i = 0; i < 2; ++i) { ret *= ext[i]; }
        return ret;
    }

    double kappa() const { return m_kappa; }

private:
    std::array<double, 2> m_L;
    std::array<size_t, 2> m_point_count;
    std::array<size_t, 2> m_padding;
    double                m_kappa = 1.0;
};

template <class field> auto full_span(const field& f, Grid grid) {
    return make_span(f, grid.padded_extent());
}

template <class field> auto full_span(field& f, Grid grid) {
    return make_span(f, grid.padded_extent());
}

template <class field> auto internal_span(const field& f, Grid grid) {
    auto            fspan = full_span(f, grid);
    auto            begin = grid.padding();
    decltype(begin) end(begin);
    for (size_t i = 0; i < end.size(); ++i) { end[i] += grid.extent()[i]; }
    return make_subspan(fspan, begin, end);
}

template <class field> auto internal_span(field& f, Grid grid) {
    auto            fspan = full_span(f, grid);
    auto            begin = grid.padding();
    decltype(begin) end(begin);
    for (size_t i = 0; i < end.size(); ++i) { end[i] += grid.extent()[i]; }
    return make_subspan(fspan, begin, end);
}

template <class Span> void print(Span span) {

    if constexpr (rank(span) == 1) {

        for (size_t i = 0; i < span.extent(0); ++i) {
            std::cout << span(i) << " ";
        }
        std::cout << std::endl;
    } else if constexpr (rank(span) == 2) {
        for (size_t i = 0; i < span.extent(0); ++i) {
            for (size_t j = 0; j < span.extent(1); ++j) {
                std::cout << std::setprecision(2) << std::setw(4) << span(i, j)
                          << " ";
            }
            std::cout << std::endl;
        }
    } else {
        throw std::logic_error("Only ranks 1 and 2 spans can be printed");
    }
}

auto get_direction(Dir dir) {

    std::array<index_type, 2> pos{};
    std::array<index_type, 2> neg{};

    pos[dir] = 1;
    neg[dir] = -1;
    return std::make_pair(neg, pos);
}