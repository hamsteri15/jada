#pragma once

#include "channel.hpp"
#include "mpi_channel.hpp"

namespace jada {

namespace detail {

void send(auto&       data,
          const auto& topology,
          auto        begin_padding,
          auto        end_padding,
          auto&       channel,
          int         rank) {

    for (const auto& sender : topology.get_boxes(rank)) {
        put(data, topology, begin_padding, end_padding, channel, sender);
    }
}

void receive(auto&       data,
             const auto& topology,
             auto        begin_padding,
             auto        end_padding,
             auto&       channel,
             int         rank) {

    for (const auto& receiver : topology.get_boxes(rank)) {
        get(data, begin_padding, end_padding, channel, receiver);
    }
}

} // namespace detail

template <class Data, size_t N>
void send_receive(Data&                     data,
                  const Topology<N>&        topology,
                  std::array<index_type, N> begin_padding,
                  std::array<index_type, N> end_padding,
                  int                       rank) {

    using T = typename Data::value_type;

    Channel<N, T> channel;

    detail::send(data, topology, begin_padding, end_padding, channel, rank);
    detail::receive(data, topology, begin_padding, end_padding, channel, rank);
}

template <class Data, size_t N>
void mpi_send_receive(Data&                     data,
                      const Topology<N>&        topology,
                      std::array<index_type, N> begin_padding,
                      std::array<index_type, N> end_padding,
                      int                       rank) {

    using T = typename Data::value_type;

    MpiChannel<N, T> channel;

    detail::send(data, topology, begin_padding, end_padding, channel, rank);
    detail::receive(data, topology, begin_padding, end_padding, channel, rank);
}

} // namespace jada