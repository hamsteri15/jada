#pragma once

#include <vector>
#include <map>

#include "topology.hpp"

namespace jada{

template<size_t N>
struct Transfer{

    int sender_rank; int receiver_rank;
    std::array<index_type, N> sender_begin;
    std::array<index_type, N> receiver_begin;
    std::array<size_type, N> extent;

    bool operator==(const Transfer<N>& lhs) const = default;
    bool operator!=(const Transfer<N>& lhs) const = default;
};



template<size_t N, class T>
struct Channel{

    void put(const std::vector<T>& data, BoxRankPair<N> sendr_box){


        for (const auto& recvr_box : m_topology.get_boxes()){
            
            auto [sender_begins, receiver_begins, extents] =
                m_topology.get_locations(sendr_box, recvr_box);


            for (size_t i = 0; i < sender_begins.size(); ++i){

                Transfer<N> t
                {
                    .sender_rank = sendr_box.rank,
                    .receiver_rank = recvr_box.rank,
                    .sender_begin = sender_begins[i],
                    .receiver_begin = receiver_begins[i],
                    .extent = extents[i]
                };

                std::vector<T> buffer(flat_size(extents[i]));


                //auto big_span = make_span(data, m_topology.get_padded_extent(sendr_box));


            }


        }



    }


    /*
    auto get(int receiver_rank) const{



    }

    */


private:

    Topology<N> m_topology;
    std::map<Transfer<N>, std::vector<T>> m_sends;


};







}