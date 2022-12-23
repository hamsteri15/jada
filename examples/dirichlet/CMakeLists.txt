
add_executable(DirichletSolver.bin dirichlet.cpp)

target_link_libraries(DirichletSolver.bin PRIVATE project_options project_warnings)


#target_compile_options(SerialTest.bin PRIVATE -DDEBUG)
target_include_directories(DirichletSolver.bin PUBLIC
                            ${CMAKE_SOURCE_DIR}
#                            #${MPI_CXX_INCLUDE_PATH}
                          )
