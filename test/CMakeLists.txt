
# Build catch main
#add_library(catch_main STATIC catch_main.cpp)
find_package(Catch2 3 REQUIRED)


#Unit tests#
add_executable(SerialTest.bin test_jada.cpp)


target_link_libraries(SerialTest.bin PRIVATE Catch2::Catch2WithMain project_options)


if (${CMAKE_CXX_COMPILER_ID} MATCHES "NVHPC")
  find_package(Threads REQUIRED)
  find_package(CUDA REQUIRED)
  find_package(Thrust REQUIRED)
  message("${CUDA_LIBRARIES}")
  message("${THRUST_LIBRARIES}")
  target_compile_options(SerialTest.bin PUBLIC -x cu -stdpar)
  target_link_directories(SerialTest.bin PUBLIC /opt/nvidia/hpc_sdk/Linux_x86_64/22.11/cuda/11.8/lib64/)
  #target_link_libraries(SerialTest.bin PUBLIC cudart_static)
  target_link_options(SerialTest.bin PUBLIC -stdpar -MD /opt/nvidia/hpc_sdk/Linux_x86_64/22.11/cuda/11.8/lib64/libcudart_static.a)
else()
  
#gcc
  #Configure tbb
  find_package(TBB REQUIRED)
  target_link_libraries(SerialTest.bin PUBLIC TBB::tbb)
  
  #NVCC
  #target_compile_options(SerialTest.bin PUBLIC -x cu --extended-lambda --expt-relaxed-constexpr)
  #target_link_libraries(SerialTest.bin PUBLIC tbb)
endif()


#target_compile_options(SerialTest.bin PRIVATE -DDEBUG)
target_include_directories(SerialTest.bin PUBLIC
                            ${CMAKE_SOURCE_DIR}
#                            #${MPI_CXX_INCLUDE_PATH}
                          )

add_test( NAME SerialTest COMMAND SerialTest.bin)

###############Benchmark

#FIND_PACKAGE(OpenMP)

#add_executable(SerialBenchmark.bin benchmark_jada.cpp)



#target_link_libraries(SerialBenchmark.bin PUBLIC project_options catch_main TBB::tbb OpenMP::OpenMP_CXX )
#target_include_directories(SerialBenchmark.bin PUBLIC ${CMAKE_SOURCE_DIR}
                          #${MPI_CXX_INCLUDE_PATH}
#                         )

#add_test( NAME SerialBenchmark COMMAND SerialBenchmark.bin)