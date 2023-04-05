
# Build catch main
add_library(catch_main STATIC catch_main.cpp)
target_include_directories(catch_main PUBLIC ${CMAKE_SOURCE_DIR}/catch)

# Build catch mpi main
add_library(catch_mpi_main STATIC catch_mpi_main.cpp)
target_link_libraries(catch_mpi_main MPI::MPI_CXX)