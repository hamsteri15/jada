#pragma once

#include "channel.hpp"

namespace jada {

namespace detail {

template <class Data, size_t N>
auto send(const Data& data, const Topology<N>& topology, int rank) {

    using T = typename Data::value_type;

    Channel<N, T> buffer;

    for (const auto& sender : topology.get_boxes(rank)){
        put(data, buffer, topology, sender); 
    }
    return buffer;

}

void receive(auto& data, const auto& topology, auto& channel, int rank) {

    for (const auto& box : topology.get_boxes(rank)) {
        get(data, topology, channel, box);
    }
}

} // namespace detail

template <class Data, size_t N>
void send_receive(Data& data, const Topology<N>& topology, int rank) {

    auto channel = detail::send(data, topology, rank);
    detail::receive(data, topology, channel, rank);
}

} // namespace jada