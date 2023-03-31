#pragma once

#include "include/bits/algorithms/algorithms.hpp"

namespace jada{

/*
auto get_ends(auto begins1, auto begins2, auto extents){

    auto ends1 = begins1;
    auto ends2 = begins2;

    for (size_t i = 0; i < ends1.size(); ++i){
    for (size_t j = 0; j < ends1[i].size(); ++j){
        ends1[i][j] += extents[i][j];
        ends2[i][j] += extents[i][j];
    }}

    return std::make_pair(ends1, ends2);

}

auto receive_data(auto& recv_buffer,
                  auto  recv_box,
                  auto  topology,
                  auto  channel) {

    for (const auto& neigh_box : topology.get_boxes()) {

        
        auto [sender_begins, receiver_begins, extents] =
            topology.get_locations(neigh_box, recv_box);

        auto [sender_ends, receiver_ends] = get_ends(sender_begins, receiver_begins, extents);

       
        auto data = channel.get_data(recv_box);

        auto dspan = make_span(data, )



    }
}
*/

}