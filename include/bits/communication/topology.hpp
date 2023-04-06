#pragma once

#include "box.hpp"
#include "box_rank_pair.hpp"
#include "transfer_info.hpp"

#include <vector>

namespace jada {

template <size_t N> struct Topology {

private:
    Box<N>                      m_domain;
    std::vector<BoxRankPair<N>> m_boxes;
    std::array<bool, N>         m_periodic;

public:
    Topology(Box<N>                      domain,
             std::vector<BoxRankPair<N>> boxes,
             std::array<bool, N>         periodic)
        : m_domain(domain)
        , m_boxes(boxes)
        , m_periodic(periodic) {
        runtime_assert(this->is_valid(), "Invalid topology");
    }

    const auto& get_domain() const { return m_domain; }

    const auto& get_boxes() const { return m_boxes; }

    auto get_boxes(int rank) const {

        std::vector<BoxRankPair<N>> ret;

        for (const auto& b : get_boxes()) {
            if (b.rank == rank) { ret.push_back(b); }
        }

        return ret;
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

    /// @brief Returns all the transfers between a pair of sender and a receiver
    /// box
    /// @param sender the box that sends data
    /// @param receiver the box that receives data
    /// @param begin_padding the amount of padding added to the beginning of
    /// receiver box
    /// @param end_padding the amount of padding added to the end of the
    /// receiver box
    /// @return Returns all transfers (overlaps) between sender and receiver
    /// when the receiver box is expanded by the input padding. Note that it is
    /// possible that sender == recevier in which case transfers can occur due
    /// to periodicity.
    auto get_transfers(const BoxRankPair<N>&     sender,
                       const BoxRankPair<N>&     receiver,
                       std::array<index_type, N> begin_padding,
                       std::array<index_type, N> end_padding) const {

        runtime_assert(found(sender), "Sender box not found");
        runtime_assert(found(receiver), "Sender box not found");

        std::vector<TransferInfo<N>> ret;

        // Check physical intersection
        if (sender != receiver) {

            const auto inter = intersection(
                expand(receiver.box, begin_padding, end_padding), sender.box);

            if (volume(inter) > 0) {

                auto sb = global_to_local(
                    sender, inter.m_begin, begin_padding, end_padding);
                auto rb = global_to_local(
                    receiver, inter.m_begin, begin_padding, end_padding);

                auto extent = extent_to_array(inter.get_extent());

                TransferInfo info{.sender_rank    = sender.rank,
                                  .receiver_rank  = receiver.rank,
                                  .sender_begin   = sb,
                                  .receiver_begin = rb,
                                  .extent         = extent};

                ret.push_back(info);
            }
        }

        // Check intersections due to periodicity, also handles owner ==
        // neighbour
        for (auto dir : get_directions()) {

            if (is_periodic_dir(dir)) {

                auto t     = get_translation(dir);
                auto n     = translate(receiver.box, t);
                auto inter = intersection(expand(n, begin_padding, end_padding),
                                          sender.box);

                if ((volume(inter) > 0)) {

                    auto sb = global_to_local(
                        sender, inter.m_begin, begin_padding, end_padding);

                    // Negate the translation vector to transform receiver back
                    // to original position
                    for (auto& e : t) { e *= -1; }

                    // Translate back here
                    auto rb = global_to_local(receiver,
                                              translate(inter, t).m_begin,
                                              begin_padding,
                                              end_padding);

                    auto extent = extent_to_array(inter.get_extent());

                    TransferInfo info{.sender_rank    = sender.rank,
                                      .receiver_rank  = receiver.rank,
                                      .sender_begin   = sb,
                                      .receiver_begin = rb,
                                      .extent         = extent};

                    ret.push_back(info);
                }
            }
        }

        return ret;
    }

    template <size_t L>
    friend std::ostream& operator<<(std::ostream& os, const Topology<L>& topo);

private:
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

    auto get_translation(auto dir) const {
        auto                      dims = extent_to_array(m_domain.get_extent());
        std::array<index_type, N> t{};

        for (size_t i = 0; i < N; ++i) { t[i] = index_type(dims[i]) * dir[i]; }
        return t;
    }

    bool is_periodic_dir(auto dir) const {

        for (size_t i = 0; i < N; ++i) {

            if (dir[i] && !m_periodic[i]) { return false; }
        }

        return true;
    }

    auto get_directions() const {
        return Neighbours<N, ConnectivityType::Box>::create();
    }

    bool are_neighbours(const BoxRankPair<N>& owner,
                        const BoxRankPair<N>& neighbour) const {
        return get_intersections(owner, neighbour).size() > 0;
    }

    auto global_to_local(const BoxRankPair<N>&            owner,
                         const std::array<index_type, N>& coord,
                         std::array<index_type, N>        begin_padding,
                         std::array<index_type, N>        end_padding) const {

        runtime_assert(m_domain.contains(coord), "Coordinate not in domain.");

        auto box = expand(owner.box, begin_padding, end_padding);

        std::array<index_type, N> ret{};
        for (size_t i = 0; i < N; ++i) { ret[i] = coord[i] - box.m_begin[i]; }

        runtime_assert(box.contains(ret), "Coordinate not in box.");

        return ret;
    }

    auto local_to_global(const BoxRankPair<N>&            owner,
                         const std::array<index_type, N>& coord) const {

        runtime_assert(owner.box.contains(coord), "Coordinate not in box.");

        auto box = owner.box;

        std::array<index_type, N> ret{};
        for (size_t i = 0; i < N; ++i) { ret[i] = box.m_begin[i] + coord[i]; }

        runtime_assert(m_domain.contains(ret), "Coordinate not in domain.");

        return ret;
    }
};

template <size_t L>
std::ostream& operator<<(std::ostream& os, const Topology<L>& topo) {

    os << "Domain: " << topo.m_domain << std::endl;
    for (auto e : topo.get_boxes()) { os << e << std::endl; }
    return os;
}

} // namespace jada