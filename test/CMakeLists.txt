
# Build catch main
#add_library(catch_main STATIC catch_main.cpp)
find_package(Catch2 3 REQUIRED)


#Unit tests#
add_executable(SerialTest.bin test_jada.cpp)


target_link_libraries(SerialTest.bin PRIVATE Catch2::Catch2WithMain project_options)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "NVHPC")
  target_compile_options(SerialTest.bin PUBLIC -x cu -stdpar)
  target_link_options(SerialTest.bin PUBLIC -stdpar -MD)
else()
  target_link_libraries(SerialTest.bin PUBLIC TBB::tbb)
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