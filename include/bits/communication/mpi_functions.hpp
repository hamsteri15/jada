#pragma once

#include <mpi.h>

#include "include/bits/utils.hpp"

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

} // namespace mpi
} // namespace jada