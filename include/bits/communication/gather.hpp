#pragma once

#include "mpi_functions.hpp"

namespace jada {


//NOTE! This does not take topology into account!!!!
template <class Container>
static Container gather(const Container& local,
                        int              root,
                        MPI_Comm         communicator = MPI_COMM_WORLD) {

    auto local_size  = static_cast<int>(std::size(local));
    auto global_size = mpi::sum_reduce(local_size, root, communicator);

    Container global(global_size);

    using element_type = typename Container::value_type;
    mpi::MakeDatatype<element_type> temp;

    mpi::gather(local.data(),
           local_size,
           temp(),
           global.data(),
           global_size,
           temp(),
           root,
           communicator);

    return global;
}

} // namespace jada
