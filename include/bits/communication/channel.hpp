#pragma once

#include <map>
#include <vector>
#include <sstream>

#include "topology.hpp"

namespace jada {

template <size_t N> struct Transfer {

    int                       sender_rank;
    int                       receiver_rank;
    std::array<index_type, N> sender_begin;
    std::array<index_type, N> receiver_begin;
    std::array<size_type, N>  extent;

    bool operator==(const Transfer<N>& rhs) const = default;
    bool operator!=(const Transfer<N>& rhs) const = default;

    
    bool operator<(const Transfer<N>& rhs) const
    {
        std::stringstream s1; s1 << *this;
        std::stringstream s2; s2 << rhs;

        return s1.str() < s2.str();
    }
    
    

    template <size_t L>
    friend std::ostream& operator<<(std::ostream& os, const Transfer<L>& t);
};

template <size_t L>
std::ostream& operator<<(std::ostream& os, const Transfer<L>& t) {

    os << "Sender: " << t.sender_rank;
    os << " Receiver: " << t.receiver_rank;
    os << " Sender begin: ";
    for (auto e : t.sender_begin) {os << e << " ";}

    os << " Receiver begin: ";
    for (auto e : t.receiver_begin) {os << e << " ";}

    os << " Extent: ";
    for (auto e : t.extent) {os << e << " ";}

    return os;
}

auto get_end(auto begin, auto extent) {

    auto ret = begin;
    for (size_t i = 0; i < begin.size(); ++i) { ret[i] += index_type(extent[i]); }
    return ret;
}

template <size_t N, class T> struct Channel {

    Channel(Topology<N> topo)
        : m_topology(topo)
        , m_sends() {}

    void put(const std::vector<T>& data, BoxRankPair<N> sendr_box) {

        for (const auto& recvr_box : m_topology.get_boxes()) {

            auto [sender_begins, receiver_begins, extents] =
                m_topology.get_locations(sendr_box, recvr_box);

            for (size_t i = 0; i < sender_begins.size(); ++i) {


                Transfer<N> t{.sender_rank    = sendr_box.rank,
                              .receiver_rank  = recvr_box.rank,
                              .sender_begin   = sender_begins[i],
                              .receiver_begin = receiver_begins[i],
                              .extent         = extents[i]};

                std::vector<T> buffer(flat_size(extents[i]));

                auto big_span =
                    make_span(data, m_topology.get_padded_extent(sendr_box));
                auto small_span =
                    make_subspan(big_span,
                                 sender_begins[i],
                                 get_end(sender_begins[i], extents[i]));

                auto buffer_span = make_span(buffer, extents[i]);

                transform(
                    small_span, buffer_span, [](auto val) { return val; });

                m_sends[t] = buffer;

            }
        }
    }

    auto get(std::vector<T>& data, BoxRankPair<N> recvr_box) const {

        auto big_span = make_span(data, m_topology.get_padded_extent(recvr_box));
        

        for (const auto& [transfer, buffer] : m_sends) {

            if (transfer.receiver_rank == recvr_box.rank) {

                auto begin = transfer.receiver_begin;
                auto end = get_end(transfer.receiver_begin, transfer.extent);
                auto o_span = make_subspan(big_span, begin, end);

                auto i_span = make_span(buffer, transfer.extent);


                transform(i_span, o_span, [](auto val) { return val; });
            }
        }
    }

    
    template <size_t L, class TT>
    friend std::ostream& operator<<(std::ostream& os, const Channel<L, TT>& t);

private:
    Topology<N>                           m_topology;
    std::map<Transfer<N>, std::vector<T>> m_sends;
};


template <size_t L, class TT>
std::ostream& operator<<(std::ostream& os, const Channel<L, TT>& v) {

    for (const auto& [transfer, buffer] : v.m_sends) {
        os << transfer << std::endl;
    }
    return os;
}


} // namespace jada