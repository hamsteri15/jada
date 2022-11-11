#pragma once

namespace jada {

#ifdef USE_MPI
int mpi_function() { return 5; }
#else
int mpi_function() { return 2; }
#endif

} // namespace jada