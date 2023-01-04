cmake_minimum_required(VERSION 3.12)

project(jada LANGUAGES CXX)



# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)

target_compile_features(project_options INTERFACE cxx_std_20) #use c++20

if (${CMAKE_CXX_COMPILER_ID} MATCHES "NVHPC" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "PGI")
  #set(flags -stdpar)
  set(flags --gcc-toolchain=g++ -stdpar=gpu)
  target_compile_options(project_options INTERFACE ${flags})
  #This is annoying, cmake wants the flags defined above to be also specified to
  #link flags when nvc++ is used... probably some bug in cmake
  target_link_options(project_options INTERFACE ${flags})

elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} MATCHES ".*Clang")
  
  find_package(TBB REQUIRED)
  target_link_libraries(project_options INTERFACE TBB::tbb)
endif()


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)


#Configure MPI
#option(USE_MPI "Enable building with mpi" No)
#if (USE_MPI)
#find_package(MPI REQUIRED)
#message("STATUS MPI FOUND: ${MPI_CXX_FOUND}")
#add_compile_definitions(USE_MPI)
#endif()



#Set library and binary paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)




option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
  enable_testing()
  message(
    "Building tests."
  )
  add_subdirectory(test)
endif()


option(BUILD_BENCHMARKS "Build benchmarks" OFF)

if(BUILD_BENCHMARKS)
  enable_testing()
  message(
    "Building benchmarks."
  )
  add_subdirectory(benchmark)
endif()

option(BUILD_EXAMPLES "Build examples" OFF)

if(BUILD_EXAMPLES)
  message(
    "Building examples."
  )
  add_subdirectory(examples)
endif()