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

template <size_t L, class T>
std::ostream& operator<<(std::ostream& os, const std::array<T, L>& v) {

    os << "{ ";
    for (auto e : v) { os << e << " "; }
    os << " }";
    return os;
}

template <size_t N> struct Topology {

private:
    static constexpr int NULL_NEIGHBOUR = -435;

    Box<N>                      m_domain;
    std::vector<BoxRankPair<N>> m_boxes;
    std::array<bool, N>         m_periodic;

    // TODO: fix
    const std::array<index_type, N> m_begin_expansion = []() {
        std::array<index_type, N> ret{};
        for (size_t i = 0; i < N; ++i) { ret[i] = 1; }
        return ret;
    }();

    // TODO: fix
    const std::array<index_type, N> m_end_expansion = []() {
        std::array<index_type, N> ret{};
        for (size_t i = 0; i < N; ++i) { ret[i] = 1; }
        return ret;
    }();

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

    std::vector<BoxRankPair<N>>
    get_neighbours(const BoxRankPair<N>& owner) const {

        runtime_assert(found(owner), "Box not in topology.");

        std::vector<BoxRankPair<N>> ret;

        for (const auto& neighbour : m_boxes) {

            if (are_neighbours(owner, neighbour)) { ret.push_back(neighbour); }
        }

        return ret;
    }

    auto get_intersections(const BoxRankPair<N>& owner,
                           const BoxRankPair<N>& neighbour) const {

        std::vector<Box<N>> intersections;

        // Check physical intersection
        if (owner != neighbour) {
            const auto physical_inter = intersection(
                expand(owner.box, m_begin_expansion, m_end_expansion),
                neighbour.box);
            if (volume(physical_inter) > 0) {
                intersections.push_back(physical_inter);
            }
        }

        // Check intersections due to periodicity, also handles owner ==
        // neighbour
        for (auto dir : get_directions()) {

            if (is_periodic_dir(dir)) {

                auto t     = get_translation(dir);
                auto n     = translate(neighbour.box, t);
                auto inter = intersection(
                    expand(owner.box, m_begin_expansion, m_end_expansion), n);

                if ((volume(inter) > 0)) { intersections.push_back(inter); }
            }
        }

        return intersections;
    }

    auto global_to_local(const BoxRankPair<N>&            owner,
                         const std::array<index_type, N>& coord) const{

        auto box = expand(owner.box, m_begin_expansion, m_end_expansion);

        std::array<index_type, N> ret{};
        for (size_t i = 0; i < N; ++i) { ret[i] = coord[i] - box.m_begin[i]; }

        runtime_assert(box.contains(coord), "Coordinate not in box.");
        return ret;
    }

    auto get_locations(const BoxRankPair<N>& sender,
                       const BoxRankPair<N>& receiver) const {

        std::vector<std::array<index_type, N>> sender_begins;
        std::vector<std::array<index_type, N>> receiver_begins;
        std::vector<std::array<size_type, N>> extents;


        // Check physical intersection
        if (sender != receiver) {
            const auto physical_inter = intersection(
                expand(receiver.box, m_begin_expansion, m_end_expansion),
                sender.box);
            if (volume(physical_inter) > 0) {

                
                auto sb = global_to_local(sender, physical_inter.m_begin);                
                auto rb = global_to_local(receiver, physical_inter.m_begin);                

                extents.push_back(extent_to_array(physical_inter.get_extent()));
                sender_begins.push_back(sb);
                receiver_begins.push_back(rb);
            }
        }

        
        // Check intersections due to periodicity, also handles owner ==
        // neighbour
        for (auto dir : get_directions()) {

            if (is_periodic_dir(dir)) {

                auto t     = get_translation(dir);
                auto n     = translate(receiver.box, t);
                auto inter = intersection(
                    expand(n, m_begin_expansion, m_end_expansion), sender.box);

                if ((volume(inter) > 0)) {
                    auto t_neg = [=](){
                        auto ret = t;
                        for (auto& elem : ret){
                            elem *= -1;
                        }
                        return ret;
                    }();


                    auto sb = global_to_local(sender, inter.m_begin);
                    //Translate back here                
                    auto rb = global_to_local(receiver, translate(inter, t_neg).m_begin);                
                    
                    extents.push_back(extent_to_array(inter.get_extent()));
                    sender_begins.push_back(sb);
                    receiver_begins.push_back(rb);
                

                }
            }
        }
        
        
        
        
        return std::make_tuple(sender_begins, receiver_begins, extents);
    }
};

} // namespace jada