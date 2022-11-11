cmake_minimum_required(VERSION 3.12)

project(jada)

message("COMPILER ID " ${CMAKE_CXX_COMPILER_ID})


message("BINARY DIR" ${CMAKE_BINARY_DIR})



# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
set(STANDARD -std=c++17)
target_compile_options(project_options INTERFACE -std=c++17) 
#This does not work for nvcc correctly
#target_compile_features(project_options INTERFACE cxx_std_14)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)


#Configure MPI
option(USE_MPI "Enable building with mpi" No)


if (USE_MPI)
find_package(MPI REQUIRED)
message("STATUS MPI FOUND: ${MPI_CXX_FOUND}")
add_compile_definitions(USE_MPI)
endif()


option(ENABLE_TESTING "Enable Test Builds" ON)

#Set library and binary paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



#Build apps
#INCLUDE_DIRECTORIES(apps)
#ADD_SUBDIRECTORY(apps)

if(ENABLE_TESTING)
  enable_testing()
  message(
    "Building Tests."
  )
  add_subdirectory(test)
endif()


