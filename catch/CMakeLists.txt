
# Build catch main
add_library(test_main STATIC catch_main.cpp)


#Unit tests#
add_executable(SerialTest.bin test_jada.cpp)

target_link_libraries(SerialTest.bin PRIVATE test_main project_options project_warnings)

target_include_directories(SerialTest.bin PUBLIC
                            ${CMAKE_SOURCE_DIR}
#                            #${MPI_CXX_INCLUDE_PATH}
                          )

add_test( NAME SerialTest COMMAND SerialTest.bin)

