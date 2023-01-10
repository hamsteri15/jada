
#Benchmarks
add_executable(BenchmarkJada.bin benchmark_jada.cpp)

find_package(OpenMP)
target_link_libraries(BenchmarkJada.bin PRIVATE catch_main project_options project_warnings OpenMP::OpenMP_CXX)


#target_compile_options(SerialTest.bin PRIVATE -DDEBUG)
target_include_directories(BenchmarkJada.bin PUBLIC
                            ${CMAKE_SOURCE_DIR}
#                            #${MPI_CXX_INCLUDE_PATH}
                          )

add_test( NAME BenchmarkJada.bin COMMAND BenchmarkJada.bin)
