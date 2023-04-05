#pragma once

#include "channel.hpp"
#include "mpi_functions.hpp"

namespace jada {

template <size_t N, class T> struct MpiChannel {


    MPI_Comm comm_handle = MPI_COMM_WORLD;

    std::vector<TransferInfo<N>> tags;
};
template <size_t L, class TT>
std::ostream& operator<<(std::ostream& os, const MpiChannel<L, TT>& v) {

    for (const auto& tag : v.tags) { os << tag << std::endl; }
    return os;
}

template <size_t N, class T>
auto create_datatype(const MpiChannel<N, T>& channel) {
    (void) channel;
    return MPI_INT;
}

template <size_t N, class T>
void put(MpiChannel<N, T>&      channel,
         const TransferInfo<N>& tag,
         const std::vector<T>&  data) {

    channel.tags.push_back(tag);

    auto count = flat_size(tag.extent);

    int mpi_tag = 1;

    MPI_Send(data.data(),
             static_cast<int>(count),
             create_datatype(channel),
             tag.receiver_rank,
             mpi_tag,
             channel.comm_handle);
}


template <size_t N, class T>
auto get(const MpiChannel<N, T>& channel, int receiver_rank) {

    std::vector<std::pair<TransferInfo<N>, std::vector<T>>> ret;

    for (size_t i = 0; i < channel.tags.size(); ++i) {
        if (channel.tags[i].receiver_rank == receiver_rank) {

            auto           tag   = channel.tags[i];
            auto            count = flat_size(tag.extent);
            std::vector<T> buffer(count);

            int mpi_tag = 1;

            MPI_Recv(buffer.data(),
                     static_cast<int>(count),
                     create_datatype(channel),
                     tag.sender_rank,
                     mpi_tag,
                     channel.comm_handle,
                     MPI_STATUS_IGNORE);

            ret.push_back(make_pair(tag, buffer));

            // ret.push_back(make_pair(channel.tags[i], channel.datas[i]));
        }
    }
    return ret;
}

} // namespace jada