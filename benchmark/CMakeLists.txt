
# Build catch main
add_library(benchmark_main STATIC benchmark_main.cpp)


#Unit tests#
add_executable(BenchmarkJada.bin benchmark_jada.cpp)

find_package(OpenMP)

target_link_libraries(BenchmarkJada.bin PRIVATE test_main project_options project_warnings OpenMP::OpenMP_CXX)


if (${CMAKE_CXX_COMPILER_ID} MATCHES "NVHPC")
  #target_compile_options(SerialTest.bin PUBLIC -w -fast -Mnouniform -Mfprelaxed -stdpar=gpu)
  #target_link_options(SerialTest.bin PUBLIC -w -fast -Mnouniform -Mfprelaxed -stdpar=gpu)
  target_compile_options(BenchmarkJada.bin PUBLIC -stdpar=gpu)
  target_link_options(BenchmarkJada.bin PUBLIC -stdpar=gpu)
else()
  
#gcc
  #Configure tbb
  find_package(TBB REQUIRED)
  target_link_libraries(BenchmarkJada.bin PUBLIC TBB::tbb)
  
  #find_package(OpenMP REQUIRED)
  #target_link_libraries(SerialTest.bin PUBLIC OpenMP::OpenMP_CXX)


  #NVCC
  #target_compile_options(SerialTest.bin PUBLIC -x cu --extended-lambda --expt-relaxed-constexpr)
  #target_link_libraries(SerialTest.bin PUBLIC tbb)
endif()


#target_compile_options(SerialTest.bin PRIVATE -DDEBUG)
target_include_directories(BenchmarkJada.bin PUBLIC
                            ${CMAKE_SOURCE_DIR}
#                            #${MPI_CXX_INCLUDE_PATH}
                          )

add_test( NAME BenchmarkJada.bin COMMAND BenchmarkJada.bin)
