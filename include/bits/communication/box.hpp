#pragma once

#include "include/bits/extents.hpp"
#include "include/bits/integer_types.hpp"
#include "include/bits/utils.hpp"

namespace jada {

template <size_t N> struct Box {

public:
    std::array<index_type, N> m_begin{}; // inclusive begin
    std::array<index_type, N> m_end{};   // non-inclusive end

public:
    Box() = default;

    Box(std::array<index_type, N> begin, std::array<index_type, N> end)
        : m_begin(begin)
        , m_end(end) {

        runtime_assert(this->is_valid(), "Invalid box");
    }

    bool operator==(const Box<N>& rhs) const = default;
    bool operator!=(const Box<N>& rhs) const = default;

    auto get_extent() const {
        auto diff = [this]() {
            std::array<index_type, N> ret{};
            for (size_t i = 0; i < N; ++i) { ret[i] = m_end[i] - m_begin[i]; }
            return ret;
        }();

        return make_extent(diff);
    }

    auto clone() const { return Box<N>{*this}; }

    size_t size() const { return flat_size(this->get_extent()); }

    void translate(std::array<index_type, N> shift) {

        for (size_t i = 0; i < N; ++i) {
            m_begin[i] += shift[i];
            m_end[i] += shift[i];
        }

        runtime_assert(this->is_valid(), "Invalid box");
    }

    ///
    ///@brief Expand the box width the input thicknesses
    ///
    ///@param begin_thickness the thickness to expand the begin coordinates
    ///@param end_thickness the thickness to expand the end coordinates
    ///
    void expand(std::array<index_type, N> begin_thickness,
                std::array<index_type, N> end_thickness) {

        for (size_t i = 0; i < N; ++i) {
            m_begin[i] -= begin_thickness[i];
            m_end[i] += end_thickness[i];
        }
        runtime_assert(this->is_valid(), "Invalid box");
    }

    ///
    ///@brief Expand the box in all directions
    ///
    ///@param thickness the width of the added extent
    ///
    void expand(index_type thickness) {

        std::array<index_type, N> t2{};
        for (auto& e : t2) { e = thickness; }
        this->expand(t2, t2);
        runtime_assert(this->is_valid(), "Invalid box");
    }

    bool is_valid() const {
        for (size_t i = 0; i < N; ++i) {
            if (m_begin[i] > m_end[i]) { return false; }
        }
        return true;
    }

    /// @brief Checks if an input idx is inisde this box
    /// @param idx the index to query
    /// @return true if index is inside this box, false otherwise
    bool contains(std::array<index_type, N> idx) const {

        auto valid = [](auto min, auto val, auto max) {
            return (min <= val) && (val < max);
        };
        for (size_t i = 0; i < N; ++i) {

            if (!valid(m_begin[i], idx[i], m_end[i])) { return false; }
        }
        return true;
    }

    /// @brief Checks if other box is fully inside this box
    /// @param other the box to query
    /// @return true if other fits fully inside this box, false otherwise
    bool contains(const Box<N>& other) const {
        return this->contains(other.m_begin) && this->contains(other.m_end);
    }

    template <size_t L>
    friend std::ostream& operator<<(std::ostream& os, const Box<L>& box);
};

/// @brief Serialization of a box to an input stream
/// @param os the stream to serialize the box into
/// @param v the box to serialize
/// @return the input stream with the box put into it
template <size_t L>
std::ostream& operator<<(std::ostream& os, const Box<L>& v) {

    os << "Box { ";
    for (auto e : v.m_begin) { os << e << " "; }
    os << "}, {";

    for (auto e : v.m_end) { os << e << " "; }

    os << "}";
    return os;
}

///
///@brief Returns true if lhs and rhs have common area.
///
///@param lhs left hand side input box
///@param rhs right hand side input box
///@return true if the two input boxes share area
///@return false otherwise
///
template <size_t N> bool have_overlap(const Box<N>& lhs, const Box<N>& rhs) {

    bool first  = lhs.contains(rhs.m_begin) || lhs.contains(rhs.m_end);
    bool second = rhs.contains(lhs.m_begin) || rhs.contains(lhs.m_end);

    return first || second;
}

///
///@brief Merges boxes lhs and rhs forming a new box. Assumes that the merger is
/// also a Cartesian box without rotations.
///@param lhs left hand side input box
///@param rhs right hand side input box
///@return Box<N> The result box of merging two Cartesian boxes
///
template <size_t N> Box<N> merge(const Box<N>& lhs, const Box<N>& rhs) {

    std::array<index_type, N> begin{};
    std::array<index_type, N> end{};
    for (size_t i = 0; i < N; ++i) {
        begin[i] = std::min(lhs.m_begin[i], rhs.m_begin[i]);
        end[i]   = std::max(lhs.m_end[i], rhs.m_end[i]);
    }
    return Box<N>(begin, end);
}

/// @brief Returns the intersection of boxes lhs and rhs. Assumes that the
/// intersection is also a Cartesian box, i.e. no rotations are allowed.
/// @param lhs left hand side input box
/// @param rhs right hand side input box
/// @return the intersection of lhs and rhs
template <size_t N> Box<N> intersection(const Box<N>& lhs, const Box<N>& rhs) {

    // Box from 0 to 0 if no overlap found
    if (!have_overlap(lhs, rhs)) { return Box<N>{}; }

    std::array<index_type, N> begin{};
    std::array<index_type, N> end{};

    for (size_t i = 0; i < N; ++i) {
        begin[i] = std::max(lhs.m_begin[i], rhs.m_begin[i]);
        end[i]   = std::min(lhs.m_end[i], rhs.m_end[i]);
    }

    return Box<N>(begin, end);
}

/// @brief Returns a vector from the beginning of box 'lhs' to beginning of box
/// 'rhs'
/// @param lhs Box marking the beginning of the distance vector
/// @param rhs Box marking the end of the distance vector
/// @return a vector from lhs to rhs
template <size_t N>
std::array<index_type, N> distance(const Box<N>& lhs, const Box<N>& rhs) {

    std::array<index_type, N> ret{};
    for (size_t i = 0; i < N; ++i) { ret[i] = rhs.m_begin[i] - lhs.m_begin[i]; }
    return ret;
}

} // namespace jada