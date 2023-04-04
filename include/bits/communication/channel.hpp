#pragma once

#include <map>

#include "topology.hpp"
#include "transfer_info.hpp"

namespace jada {

template <size_t N, class T> struct Channel {

    Channel(Topology<N> topo)
        : m_topology(topo)
        , m_sends() {}

    const auto& topology() const { return m_topology; }
    auto&       topology() { return m_topology; }

    void put(const std::vector<T>& data, BoxRankPair<N> sender) {

        for (const auto& recvr : m_topology.get_boxes()) {
            call_sends(data, sender, recvr);
        }
    }

    void get(std::vector<T>& data, BoxRankPair<N> recvr_box) const {

        for (const auto& [transfer, buffer] : m_sends) {

            if (transfer.receiver_rank == recvr_box.rank) {
                place_to(recvr_box, data, buffer, transfer);
            }
        }
    }

    template <size_t L, class TT>
    friend std::ostream& operator<<(std::ostream& os, const Channel<L, TT>& t);

private:
    Topology<N>                               m_topology;
    std::map<TransferInfo<N>, std::vector<T>> m_sends;

    static auto get_end(auto begin, auto extent) {

        auto ret = begin;
        for (size_t i = 0; i < begin.size(); ++i) {
            ret[i] += index_type(extent[i]);
        }
        return ret;
    }

    void place_to(BoxRankPair<N>  recvr_box,
                  auto&           data,
                  const auto&     recvd_data,
                  TransferInfo<N> info) const {
        auto begin = info.receiver_begin;
        auto end   = get_end(begin, info.extent);

        auto big_span =
            make_span(data, m_topology.get_padded_extent(recvr_box));
        auto to = make_subspan(big_span, begin, end);

        auto from = make_span(recvd_data, info.extent);
        transform(from, to, [](auto val) { return val; });
    }

    auto get_send_buffer(const std::vector<T>& data,
                         BoxRankPair<N>        sender,
                         auto                  begin,
                         auto                  extent) {

        std::vector<T> buffer(flat_size(extent));
        auto           buffer_span = make_span(buffer, extent);

        auto big_span = make_span(data, m_topology.get_padded_extent(sender));
        auto slice    = make_subspan(big_span, begin, get_end(begin, extent));

        transform(slice, buffer_span, [](auto val) { return val; });

        return buffer;
    }

    void call_sends(const std::vector<T>& data,
                    BoxRankPair<N>        sender,
                    BoxRankPair<N>        recvr) {

        auto [sender_begins, receiver_begins, extents] =
            m_topology.get_locations(sender, recvr);

        for (size_t i = 0; i < sender_begins.size(); ++i) {

            TransferInfo<N> t{.sender_rank    = sender.rank,
                              .receiver_rank  = recvr.rank,
                              .sender_begin   = sender_begins[i],
                              .receiver_begin = receiver_begins[i],
                              .extent         = extents[i]};

            m_sends[t] =
                get_send_buffer(data, sender, sender_begins[i], extents[i]);
        }
    }
};

template <size_t L, class TT>
std::ostream& operator<<(std::ostream& os, const Channel<L, TT>& v) {

    for (const auto& [transfer, buffer] : v.m_sends) {
        os << transfer << std::endl;
    }
    return os;
}

} // namespace jada