# Build catch main
add_library(catch_main STATIC catch_main.cpp)


#Unit tests#
add_executable(SerialTest.bin test_jada.cpp)

target_link_libraries(SerialTest.bin PUBLIC project_options catch_main)
#target_compile_options(SerialTest.bin PRIVATE -DDEBUG)
target_include_directories(SerialTest.bin PUBLIC
                            ${CMAKE_SOURCE_DIR}
                            #${MPI_CXX_INCLUDE_PATH}
                          )

add_test( NAME SerialTest COMMAND SerialTest.bin)

###############Benchmark
add_executable(SerialBenchmark.bin benchmark_jada.cpp)

target_link_libraries(SerialBenchmark.bin PUBLIC project_options catch_main)
target_include_directories(SerialBenchmark.bin PUBLIC
                            ${CMAKE_SOURCE_DIR}
                            #${MPI_CXX_INCLUDE_PATH}
                          )

add_test( NAME SerialBenchmark COMMAND SerialBenchmark.bin)