#pragma once

#include <map>
#include <vector>

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

        auto l = (sender_rank +1) * (receiver_rank+1) * flat_size(make_extent(sender_begin)) * flat_size(make_extent(extent));
        auto r = (rhs.sender_rank + 1) * (rhs.receiver_rank + 1) * flat_size(make_extent(rhs.sender_begin)) * flat_size(make_extent(rhs.extent));
    
        return l < r;
    }
    
    

    template <size_t L>
    friend std::ostream& operator<<(std::ostream& os, const Transfer<L>& t);
};

template <size_t L>
std::ostream& operator<<(std::ostream& os, const Transfer<L>& t) {

    os << "Sender: " << t.sender_rank;
    os << " Receiver: " << t.receiver_rank;
    os << " N elements: " << flat_size(t.extent);
    return os;
}

auto get_end(auto begin, auto extent) {

    auto ret = begin;
    for (size_t i = 0; i < begin.size(); ++i) { begin[i] += index_type(extent[i]); }
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

        for (const auto& [transfer, buffer] : m_sends) {

            if (transfer.receiver_rank == recvr_box.rank) {

                auto o_span = make_subspan(
                    make_span(data, m_topology.get_padded_extent(recvr_box)),
                    transfer.receiver_begin,
                    get_end(transfer.receiver_begin, transfer.extent));

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