
#Unit tests#
add_executable(SerialTest.bin test_jada.cpp)

target_link_libraries(SerialTest.bin PRIVATE catch_main project_options project_warnings)

target_include_directories(SerialTest.bin PUBLIC
                            ${CMAKE_SOURCE_DIR}
                            ${CMAKE_SOURCE_DIR}/catch
#                            #${MPI_CXX_INCLUDE_PATH}
                          )

add_test( NAME SerialTest COMMAND SerialTest.bin)

