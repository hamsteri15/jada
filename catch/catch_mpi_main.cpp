#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include <mpi.h>

#include "catch.hpp"


int main(int argc, char* argv[])
{

    MPI_Init(NULL, NULL);

    // Get the number of processes
    //int world_size;
    //MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    Catch::Session session;
  
    const int result = session.run(argc, argv);

    
    MPI_Finalize();

    return result;
    
    
}


