#pragma once

#include <mpi.h>

#include "include/bits/core/utils.hpp"

#include "channel.hpp"

namespace jada {

namespace mpi {

///
///@brief Call MPI_Init.
///
///
static void init() { MPI_Init(NULL, NULL); }

///
///@brief Call mpi abort on the WORLD COMM
///
///
static void abort() { MPI_Abort(MPI_COMM_WORLD, 1); }

///
///@brief Checks if MPI_Initialized has been called.
///
///@return int called = 1, not called = 0
///
static int initialized() {

    int MPI_is_initialized;
    MPI_Initialized(&MPI_is_initialized);
    return MPI_is_initialized;
}

///
///@brief Checks if mpi-finalized has been called.
///
///@return int called = 1, not called = 0
///
static int finalized() {

    int MPI_is_finalized;
    MPI_Finalized(&MPI_is_finalized);
    return MPI_is_finalized;
}

///
///@brief Finalize mpi, throws on failure in debug mode.
///
///
static void finalize() {

    int err = MPI_Finalize();
    runtime_assert(err == MPI_SUCCESS, "Mpi::finalize fails");
}

///
///@brief Frees the given communicator handle, throws on failure in debug mode.
///
///@param comm the handle to free
///
static void comm_free(MPI_Comm comm) {

    int err = MPI_Comm_free(&comm);
    runtime_assert(err == MPI_SUCCESS, "Mpi::free fails");
}

///
///@brief Get the rank of this process in the given handle, throws on failure in
/// debug mode.
///
///@param comm the handle to check the rank from
///@return int rank
///
static int get_rank(MPI_Comm comm) {

    int rank;
    int err = MPI_Comm_rank(comm, &rank);
    runtime_assert(err == MPI_SUCCESS, "Mpi::Comm_rank fails");

    return rank;
}

///
///@brief Get the rank of this process in MPI_COMM_WORLD
///
///@return int rank of the process
///
static int get_world_rank() { return get_rank(MPI_COMM_WORLD); }

///
///@brief Checks the size of the given handle, throws on failure in debug mode.
///
///@param comm the handle to check the size from
///@return int the size
///
static int comm_size(MPI_Comm comm) {

    int size;
    int err = MPI_Comm_size(comm, &size);
    runtime_assert(err == MPI_SUCCESS, "MPI_Comm size fails.");

    return size;
}

///
///@brief Determines the number of processes
///
///@return number of processes specified when the program was launched
///
static int world_size() { return comm_size(MPI_COMM_WORLD); }

///
///@brief Free the given mpi-datatype, throws on failure in debug mode.
///
///@param t type to free
///
static void type_free(MPI_Datatype t) {

    int err = MPI_Type_free(&t);
    runtime_assert(err == MPI_SUCCESS, "MPI_Type_free fails");
}

///
///@brief Commit the given mpi datatype, throws on failure in debug mode.
///
///@param t datatype to commit
///
static void type_commit(MPI_Datatype t) {

    int err = MPI_Type_commit(&t);
    runtime_assert(err == MPI_SUCCESS, "MPI_Type_commit fails.");
}

///
///@brief Creates a contiguous datatype
///
///@param count number of times to repeat the old_type
///@param old_type the type to repeat
///@return MPI_Datatype a contiguous datatype
///
static MPI_Datatype type_contiguous(int count, MPI_Datatype old_type) {
    MPI_Datatype new_type;

    auto err = MPI_Type_contiguous(count, old_type, &new_type);
    runtime_assert(err == MPI_SUCCESS, "MPI_Type_contiguous fails.");
    return new_type;
}

template <class T> struct MakeDatatype {};

template <> struct MakeDatatype<int> {

    MPI_Datatype operator()() { return MPI_INT; }
};

/// @brief Make a contiguous datatype for contiguous stl-like containers
/// @param v input vector to make the contiguous datatype for 
/// @return contiguous mpi datatype
template <class Container>
static MPI_Datatype type_contiguous(const Container& v) {

    using element_type = typename Container::element_type;
    MakeDatatype<element_type> old_type;
    return type_contiguous(static_cast<int>(std::size(v)), old_type());
}

///
///@brief Call mpi abort on the given communicator
///
///@param comm communicator handle to abort
///
static void abort(MPI_Comm comm) { MPI_Abort(comm, 1); }

///
///@brief Checks if the handle has topology information attached, can throw in
/// debug mode
///
///@param comm the mpi communicator handle to test
///@return true if topology information available
///@return false if no topology information is available
///
static bool topo_test(MPI_Comm comm) {
    int status;
    int err = MPI_Topo_test(comm, &status);
    runtime_assert(err == MPI_SUCCESS, "MPI_Topo_test fails.");

    return status != MPI_UNDEFINED;
}

/// @brief Reduces data from all processes to the recv_data buffer on the _root
/// process_.
/// @param send_data the local data to reduce
/// @param recv_data the buffer to place the reduced data (significant only on
/// root)
/// @param count number of elements to reduce
/// @param datatype the element type of the send_data
/// @param op reduction operation
/// @param root the process to reduce the data onto
/// @param communicator the mpi communicator (defaults to MPI_COMM_WORLD)
static void reduce(const void*        send_data,
                   void*        recv_data,
                   int          count,
                   MPI_Datatype datatype,
                   MPI_Op       op,
                   int          root,
                   MPI_Comm     communicator = MPI_COMM_WORLD) {
    auto err = MPI_Reduce(
        send_data, recv_data, count, datatype, op, root, communicator);
    runtime_assert(err == MPI_SUCCESS, "MPI_Reduce fails");
}

template<class T>
T sum_reduce(const T& local, int root, MPI_Comm communicator = MPI_COMM_WORLD){

    T ret;
    MakeDatatype<T> dt;
    reduce(&local, &ret, 1, dt(), MPI_SUM, root, communicator);
    return ret;

}


/// @brief Reduces data from all processes to the recv_data buffer on _all
/// processes_.
/// @param send_data the local data to reduce
/// @param recv_data the buffer to place the reduced data
/// @param count number of elements to reduce
/// @param datatype the element type of the send_data
/// @param op reduction operation
/// @param communicator the mpi communicator (defaults to MPI_COMM_WORLD)
static void all_reduce(const void*  send_data,
                       void*        recv_data,
                       int          count,
                       MPI_Datatype datatype,
                       MPI_Op       op,
                       MPI_Comm     communicator = MPI_COMM_WORLD) {

    auto err =
        MPI_Allreduce(send_data, recv_data, count, datatype, op, communicator);
    runtime_assert(err == MPI_SUCCESS, "MPI_Allreduce fails.");
}

///
///@brief Gathers data from all processes to the recv_data buffer on the _root
/// process_.
///
///@param send_data the local data to gather
///@param send_count number of elements in the send_data
///@param send_datatype the element type of the send_data
///@param recv_data the buffer to place the gathered data (significant only on
/// root)
///@param recv_count element count of the recv_data
///@param recv_datatype element type of the recv_data
///@param root the process to gather data onto
///@param communicator the mpi communicator (defaults to MPI_COMM_WORLD)
///
static void gather(const void*        send_data,
                   int          send_count,
                   MPI_Datatype send_datatype,
                   void*        recv_data,
                   int          recv_count,
                   MPI_Datatype recv_datatype,
                   int          root,
                   MPI_Comm     communicator = MPI_COMM_WORLD) {
    auto err = MPI_Gather(send_data,
                          send_count,
                          send_datatype,
                          recv_data,
                          recv_count,
                          recv_datatype,
                          root,
                          communicator);
    runtime_assert(err == MPI_SUCCESS, "MPI_Gather fails");
}


///
///@brief Gathers data from all processes to _all processes_.
///
///@param send_data the local data to gather
///@param send_count number of elements in the send_data
///@param send_datatype the element type of the send buffer
///@param recv_data the buffer to place the gathered data to
///@param recv_count element count of the recv_data
///@param recv_datatype the element type of the recv_data
///@param communicator the mpi communicator (defaults to MPI_COMM_WORLD)
///
static void all_gather(void*        send_data,
                       int          send_count,
                       MPI_Datatype send_datatype,
                       void*        recv_data,
                       int          recv_count,
                       MPI_Datatype recv_datatype,
                       MPI_Comm     communicator) {
    auto err = MPI_Allgather(send_data,
                             send_count,
                             send_datatype,
                             recv_data,
                             recv_count,
                             recv_datatype,
                             communicator);
    runtime_assert(err == MPI_SUCCESS, "MPI_Allgather fails.");
}

} // namespace mpi
} // namespace jada