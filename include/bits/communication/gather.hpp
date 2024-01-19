#pragma once

#include "mpi_functions.hpp"

namespace jada {

///
///@brief Given an stl-like container, returns the global size of the container
///on all processes.
///
/// @param local_data The local data to gather.
/// @param communicator The mpi communicator to use.
///@return size_t The global size of the container.
///
template <class Container>
static inline size_t get_global_size(const Container& local_data,
                                     MPI_Comm communicator = MPI_COMM_WORLD) {

    auto local_size  = static_cast<int>(std::size(local_data));
    auto global_size = mpi::all_sum_reduce(local_size, communicator);
    return static_cast<size_t>(global_size);
}

/// @brief Givan an stl like container of local data, gathers the data from all
/// processes to all processes. The returned global data container does not take
/// into account any topology and has the shape [rank0_0...rank0_n,
/// rank1_0...rank1_n ].
///
/// @param local_data The local data to gather.
/// @param communicator The mpi communicator to use.
/// @return A combined global data container on all processes.
template <class Container>
static inline Container all_gather(const Container& local_data,
                                   MPI_Comm communicator = MPI_COMM_WORLD) {

    size_t size = size_t(mpi::comm_size(communicator));

    // Determine the size of data on each process
    size_t              localSize = local_data.size();
    std::vector<int> recvCounts(size);

    // Calculate the total size of data on all processes
    mpi::all_gather(
        &localSize, 1, MPI_INT, recvCounts.data(), 1, MPI_INT, communicator);

    // Determine displacements for the gathered data
    std::vector<int> displacements(size, 0);
    for (size_t i = 1; i < size; ++i) {
        displacements[i] = displacements[i - 1] + recvCounts[i - 1];
    }

    using element_type = typename Container::value_type;
    mpi::MakeDatatype<element_type> temp;

    Container global_data(get_global_size(local_data));

    // Use MPI_Allgatherv to send the gathered data to all processes
    mpi::all_gatherv(local_data.data(),
                     int(localSize),
                     temp(),
                     global_data.data(),
                     recvCounts.data(),
                     displacements.data(),
                     temp(),
                     communicator);
    return global_data;
}

} // namespace jada
