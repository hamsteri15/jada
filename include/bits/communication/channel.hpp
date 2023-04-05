#pragma once

#include "topology.hpp"
#include "transfer_info.hpp"
#include <map>

namespace jada {

template <size_t N, class T> struct Channel {

    std::vector<std::vector<T>>  datas;
    std::vector<TransferInfo<N>> tags;
};

template <size_t L, class TT>
std::ostream& operator<<(std::ostream& os, const Channel<L, TT>& v) {

    for (const auto& tag : v.tags) { os << tag << std::endl; }
    return os;
}

static auto get_end(auto begin, auto extent) {

    auto ret = begin;
    for (size_t i = 0; i < begin.size(); ++i) {
        ret[i] += index_type(extent[i]);
    }
    return ret;
}

template <class Data, size_t N>
auto make_sendable_slice(const Data&            data,
                         const BoxRankPair<N>&  sender,
                         const Topology<N>&     topo,
                         const TransferInfo<N>& info) {

    using T = typename Data::value_type;

    std::vector<T> buffer(flat_size(info.extent));
    auto           buffer_span = make_span(buffer, info.extent);

    auto big_span = make_span(data, topo.get_padded_extent(sender));
    auto slice    = make_subspan(
        big_span, info.sender_begin, get_end(info.sender_begin, info.extent));

    transform(slice, buffer_span, [](auto val) { return val; });

    return buffer;
}

template <size_t N, class T>
void put(Channel<N, T>&         channel,
         const TransferInfo<N>& tag,
         const std::vector<T>&  data) {
    channel.tags.push_back(tag);
    channel.datas.push_back(data);
}


template <size_t N, class T>
auto get(const Channel<N, T>& channel, int receiver_rank) {

    std::vector<std::pair<TransferInfo<N>, std::vector<T>>> ret;

    for (size_t i = 0; i < channel.tags.size(); ++i) {
        if (channel.tags[i].receiver_rank == receiver_rank) {

            ret.push_back(make_pair(channel.tags[i], channel.datas[i]));
        }
    }
    return ret;
}




void put(const auto& data,
         auto&       channel,
         const auto& topo,
         const auto& sender) {

    for (auto recvr : topo.get_boxes()) {

        auto [s_begin, r_begin, extent] = topo.get_locations(sender, recvr);

        for (size_t i = 0; i < s_begin.size(); ++i) {

            TransferInfo t{.sender_rank    = sender.rank,
                           .receiver_rank  = recvr.rank,
                           .sender_begin   = s_begin[i],
                           .receiver_begin = r_begin[i],
                           .extent         = extent[i]};

            put(channel, t, make_sendable_slice(data, sender, topo, t));
        }
    }
}

void get(auto&       data,
         const auto& topo,
         const auto& channel,
         const auto& receiver) {

    for (const auto& [info, slice] : get(channel, receiver.rank)) {

        auto begin    = info.receiver_begin;
        auto end      = get_end(begin, info.extent);
        auto big_span = make_span(data, topo.get_padded_extent(receiver));
        auto to       = make_subspan(big_span, begin, end);

        auto from = make_span(slice, info.extent);

        transform(from, to, [](auto val) { return val; });
    }
}

} // namespace jada