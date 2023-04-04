#pragma once

#include "channel.hpp"

namespace jada{


template<class Data, size_t N>
auto send(const Data& data, const Topology<N>& topology, int rank){

    using T = typename Data::value_type;

    auto boxes = topology.get_boxes(rank);

    auto channel = Channel<N, T>(topology);

    for (const auto& box : boxes){
        channel.put(data, box);
    }

    return channel;

}


void receive(auto& data, auto& channel, int rank){

    for (const auto& box : channel.topology().get_boxes(rank)){
        channel.get(data, box);
    }

}

}