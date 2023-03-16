#pragma once

#include "box.hpp"
#include <vector>

namespace jada {

template <size_t N> struct BoxRankPair {
    Box<N> box;
    int    rank;

    bool operator==(const BoxRankPair<N>& lhs) const = default;
    bool operator!=(const BoxRankPair<N>& lhs) const = default;

    template <size_t L>
    friend std::ostream& operator<<(std::ostream&         os,
                                    const BoxRankPair<L>& box);
};

template <size_t L>
std::ostream& operator<<(std::ostream& os, const BoxRankPair<L>& v) {

    os << "Rank: " << v.rank << " " << v.box;
    return os;
}

template <size_t N> struct Topology {

private:
    static constexpr int NULL_NEIGHBOUR = -435;

    Box<N>                      m_domain;
    std::vector<BoxRankPair<N>> m_boxes;
    std::array<bool, N>         m_periodic;

    ///
    ///@brief Checks that none of the m_boxes have overlap
    ///
    ///@return true if no overlapping boxes found
    ///@return false otherwise
    ///
    bool is_unique() const {

        for (auto i = m_boxes.begin(); i != m_boxes.end(); ++i) {
            for (auto j = i; ++j != m_boxes.end(); /**/) {

                const auto& box1 = i->box;
                const auto& box2 = j->box;
                if (have_overlap(box1, box2)) { return false; }
            }
        }

        return true;
    }

    ///
    ///@brief Checks that the area covered by the subboxes matches the area
    /// spanned by the domain.
    ///
    ///@return true if subboxes fully cover the domain
    ///@return false otherwise
    ///
    bool fully_covered() const {

        size_type area(0);
        for (const auto& e : m_boxes) { area += e.box.size(); }

        return area == m_domain.size();
    }

public:
    Topology(Box<N>                      domain,
             std::vector<BoxRankPair<N>> boxes,
             std::array<bool, N>         periodic)
        : m_domain(domain)
        , m_boxes(boxes)
        , m_periodic(periodic) {

        runtime_assert(this->is_valid(), "Invalid topology");
    }

    /// @brief Checks that the topology is valid
    /// @return true if valid topology, false otherwise
    bool is_valid() const {
        if (!m_domain.is_valid()) { return false; }
        for (const auto& b : m_boxes) {
            if (!b.box.is_valid()) { return false; }
        }

        return is_unique() && fully_covered();
    }

    bool found(const BoxRankPair<N>& b) const {
        return std::find(m_boxes.begin(), m_boxes.end(), b) != m_boxes.end();
    }

    std::vector<BoxRankPair<N>> get_neighbours(const BoxRankPair<N>& b) const {

        runtime_assert(found(b), "Box not in topology.");

        // 1) Expand b with some amount
        // 2) Loop over all boxes and find an intersection with b
        // 3) From the intersection and dimensions of b, determine the direction
        // of the neighbour. 4) Store the direction, neihbour box and neighbour
        // rank into some type and return a vector of those 5) Handle
        // periodicity

        // Pitfalls, ensure that after expansion only the nearest neighbour is
        // found. Possibly always only expand with single unit first, find a
        // neighbour and when actual data is transferred use the actual size of
        // the halo which depends on the stencil shape.
        std::vector<BoxRankPair<N>> ret;
        return {};
    }
};

} // namespace jada