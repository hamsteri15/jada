#pragma once

#include "channel.hpp"
#include "mpi_channel.hpp"

namespace jada {

namespace detail {

void send(auto& data, const auto& topology, auto& channel, int rank) {

    for (const auto& sender : topology.get_boxes(rank)) {
        put(data, channel, topology, sender);
    }
}

void receive(auto& data, const auto& topology, auto& channel, int rank) {

    for (const auto& receiver : topology.get_boxes(rank)) {
        get(data, topology, channel, receiver);
    }
}

} // namespace detail

template <class Data, size_t N>
void send_receive(Data& data, const Topology<N>& topology, int rank) {

    using T = typename Data::value_type;

    Channel<N, T> channel;

    detail::send(data, topology, channel, rank);
    detail::receive(data, topology, channel, rank);
}

template <class Data, size_t N>
void mpi_send_receive(Data& data, const Topology<N>& topology, int rank) {

    using T = typename Data::value_type;

    MpiChannel<N, T> channel;

    detail::send(data, topology, channel, rank);
    detail::receive(data, topology, channel, rank);
}

} // namespace jada