#pragma once

#include "include/bits/core/extents.hpp"
#include "include/bits/core/integer_types.hpp"
#include "include/bits/core/utils.hpp"

namespace jada {

template <size_t N> struct Box {

public:
    static constexpr size_t dimension_count = N;

    std::array<index_type, N> begin{}; // inclusive begin
    std::array<index_type, N> end{};   // non-inclusive end

public:
    Box() = default;

    Box(std::array<index_type, N> begin_, std::array<index_type, N> end_)
        : begin(begin_)
        , end(end_) {

        // runtime_assert(this->is_valid(), "Invalid box");
    }

    bool operator==(const Box<N>& rhs) const = default;
    bool operator!=(const Box<N>& rhs) const = default;

    constexpr auto get_extent() const {
        auto diff = [this]() {
            std::array<size_type, N> ret{};
            for (size_t i = 0; i < N; ++i) {
                ret[i] = size_type(end[i] - begin[i]);
            }
            return ret;
        }();

        return make_extent(diff);
    }

    auto clone() const { return Box<N>{*this}; }

    size_t size() const { return flat_size(this->get_extent()); }

    bool is_valid() const {
        for (size_t i = 0; i < N; ++i) {
            if (begin[i] > end[i]) { return false; }
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

            if (!valid(begin[i], idx[i], end[i])) { return false; }
        }
        return true;
    }

    /// @brief Checks if other box is fully inside this box
    /// @param other the box to query
    /// @return true if other fits fully inside this box, false otherwise
    bool contains(const Box<N>& other) const {
        return this->contains(other.begin) && this->contains(other.end);
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
    for (auto e : v.begin) { os << e << " "; }
    os << "}, {";

    for (auto e : v.end) { os << e << " "; }

    os << "}";
    return os;
}

/// @brief Computes the input box b
/// @param b the box to compute the volume for
/// @return the volume of the box
template <size_t N> index_type volume(const Box<N>& b) {

    index_type vol = 1;
    for (size_t i = 0; i < N; ++i) { vol *= (b.end[i] - b.begin[i]); }
    return vol;
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
        begin[i] = std::min(lhs.begin[i], rhs.begin[i]);
        end[i]   = std::max(lhs.end[i], rhs.end[i]);
    }
    return Box<N>(begin, end);
}

/// @brief Returns the intersection of boxes lhs and rhs. Assumes that the
/// intersection is also a Cartesian box, i.e. no rotations are allowed. If the
/// input boxes do not intersect, a box with zero area is returned.
/// @param lhs left hand side input box
/// @param rhs right hand side input box
/// @return the intersection of lhs and rhs
template <size_t N> Box<N> intersection(const Box<N>& lhs, const Box<N>& rhs) {

    std::array<index_type, N> begin{};
    std::array<index_type, N> end{};

    for (size_t i = 0; i < N; ++i) {
        begin[i] = std::max(lhs.begin[i], rhs.begin[i]);
        end[i]   = std::min(lhs.end[i], rhs.end[i]);
    }

    Box<N> ret(begin, end);
    if (!ret.is_valid()) { return Box<N>{}; }
    return ret;
}

/// @brief Returns a vector from the beginning of box 'lhs' to beginning of box
/// 'rhs'
/// @param lhs Box marking the beginning of the distance vector
/// @param rhs Box marking the end of the distance vector
/// @return a vector from lhs to rhs
template <size_t N>
std::array<index_type, N> distance(const Box<N>& lhs, const Box<N>& rhs) {

    std::array<index_type, N> ret{};
    for (size_t i = 0; i < N; ++i) { ret[i] = rhs.begin[i] - lhs.begin[i]; }
    return ret;
}

///
///@brief Returns true if lhs and rhs have common volme.
///
///@param lhs left hand side input box
///@param rhs right hand side input box
///@return true if the two input boxes share volume
///@return false otherwise
///
template <size_t N> bool have_overlap(const Box<N>& lhs, const Box<N>& rhs) {
    return volume(intersection(lhs, rhs)) > 0;
}

///
/// @brief Translates the input box by the input amount wrt. current position of
/// the box.
/// @param box the box to translate
/// @param amount the amount to translate
/// @return translated box
///
template <size_t N>
Box<N> translate(const Box<N>& box, std::array<index_type, N> amount) {

    auto ret(box);

    for (size_t i = 0; i < N; ++i) {
        ret.begin[i] += amount[i];
        ret.end[i] += amount[i];
    }

    runtime_assert(ret.is_valid(), "Invalid box");

    return ret;
}

///
///@brief Expand the box with the input thicknesses
///
///@param box the box to expand
///@param begin_thickness the thickness to expand the begin coordinates
///@param end_thickness the thickness to expand the end coordinates
///@return Box<N> the expanded box
///
template <size_t N>
Box<N> expand(const Box<N>&             box,
              std::array<index_type, N> begin_thickness,
              std::array<index_type, N> end_thickness) {

    auto ret(box);
    for (size_t i = 0; i < N; ++i) {
        ret.begin[i] -= begin_thickness[i];
        ret.end[i] += end_thickness[i];
    }

    runtime_assert(ret.is_valid(), "Invalid box");
    return ret;
}

///
///@brief Expands the box with the input thickness in all dimensions
///
///@param box the box to expand
///@param thickness the thickness to expand
///@return Box<N> expanded box
///
template <size_t N> Box<N> expand(const Box<N>& box, index_type thickness) {

    std::array<index_type, N> thick{};
    for (auto& e : thick) { e = thickness; }
    return expand(box, thick, thick);
}

///
/// @brief Determines the shared edges of the two input boxes.
///
/// @param b1 first box to test.
/// @param b2 second box to test.
/// @return A pair of std::array<bool, N> where the first value describes the
/// shared beginning edges and the second value shared end edges.
template <size_t N> auto shared_edges(const Box<N>& b1, const Box<N>& b2) {

    std::array<bool, N> begin_edges{};
    std::array<bool, N> end_edges{};
    for (size_t i = 0; i < N; ++i) {

        begin_edges[i] = (b1.begin[i] == b2.begin[i]);
        end_edges[i]   = (b1.end[i] == b2.end[i]);
    }

    return std::make_pair(begin_edges, end_edges);
}

///
/// @brief Determines the begin and end bounds of the edge which has a normal to
/// the direction dir of the input box.
///
/// @param box The box to query the edge bounds of.
/// @param dir A normal vector to determine which edge to query the bounds for,
/// a negative value points towards beginning and a positive value towards end.
/// @return a pair of arrays [begin, end] containing the bounds of the edge.
template <size_t N>
static constexpr auto edge_bounds(const Box<N>&                    box,
                                  const std::array<index_type, N>& dir) {

    const auto dims = box.get_extent();

    std::array<index_type, rank(dims)> begin{};
    std::array<index_type, rank(dims)> end{};

    for (size_t i = 0; i < rank(dims); ++i) {
        end[i] = index_type(dims.extent(i));
    }

    for (size_t i = 0; i < rank(dims); ++i) {
        if (dir[i] == 1) { begin[i] = index_type(dims.extent(i)) - 1; }
        if (dir[i] == -1) { end[i] = 1; }
    }

    return std::make_pair(begin, end);
}

///
/// @brief Returns all indices of the edge which has a normal towards dir of the
/// input box.
///
/// @param box The box the query the edge indices of.
/// @param dir A normal vector to determine which edge to query the bounds for,
/// a negative value points towards beginning and a positive value towards end.
/// @return A view of all indices of the edge.
template <size_t N>
static constexpr auto edge_indices(const Box<N>&                    box,
                                   const std::array<index_type, N>& dir) {

    const auto [begin, end] = edge_bounds(box, dir);
    return md_indices(begin, end);
}

} // namespace jada