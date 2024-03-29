
#Unit tests#
add_executable(SerialTest.bin test_core.cpp test_algorithms.cpp test_geometry.cpp)

target_link_libraries(SerialTest.bin PRIVATE catch_main project_options project_warnings)

target_include_directories(SerialTest.bin PUBLIC
                            ${CMAKE_SOURCE_DIR}
                            ${CMAKE_SOURCE_DIR}/catch
#                            #${MPI_CXX_INCLUDE_PATH}
                          )

add_test( NAME SerialTest COMMAND SerialTest.bin)


#Mpi unit tests#


add_executable(MpiTest.bin test_communication.cpp)

target_link_libraries(MpiTest.bin PRIVATE catch_mpi_main project_options project_warnings)

target_include_directories(MpiTest.bin PUBLIC
                            ${CMAKE_SOURCE_DIR}
                            ${CMAKE_SOURCE_DIR}/catch
                          )

#Note, the tests are written such that maximum 3 mpi-processes are assumed
add_test( NAME MpiTest COMMAND MpiTest.bin)

add_test( NAME MpiTest2
          COMMAND mpirun -np 2 --oversubscribe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MpiTest.bin
          )

add_test( NAME MpiTest3
          COMMAND mpirun -np 3 --oversubscribe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MpiTest.bin
       )
