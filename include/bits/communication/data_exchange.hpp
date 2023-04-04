#pragma once

#include "channel.hpp"

namespace jada {

namespace detail {

template <class Data, size_t N>
auto send(const Data& data, const Topology<N>& topology, int rank) {

    using T = typename Data::value_type;

    auto channel = Channel<N, T>(topology);

    for (const auto& box : topology.get_boxes(rank)) { channel.put(data, box); }

    return channel;
}

void receive(auto& data, auto& channel, int rank) {

    for (const auto& box : channel.topology().get_boxes(rank)) {
        channel.get(data, box);
    }
}

} // namespace detail

template <class Data, size_t N>
void send_receive(Data& data, const Topology<N>& topology, int rank) {

    auto channel = detail::send(data, topology, rank);
    detail::receive(data, channel, rank);
}

} // namespace jada