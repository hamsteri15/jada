
add_executable(DirichletSolverStdpar.bin dirichlet_stdpar.cpp)
target_link_libraries(DirichletSolverStdpar.bin PRIVATE project_options project_warnings)
target_include_directories(DirichletSolverStdpar.bin PUBLIC ${CMAKE_SOURCE_DIR}/include)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "NVHPC" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "PGI")

    add_executable(DirichletSolverNvexec.bin dirichlet_nvexec.cpp)
    target_link_libraries(DirichletSolverNvexec.bin PRIVATE project_options)
    #For some reason the code doesnt compile with these warnings enabled
    #target_link_libraries(DirichletSolverNvexec.bin PRIVATE project_warnings)

    target_include_directories(DirichletSolverNvexec.bin PUBLIC ${CMAKE_SOURCE_DIR}/include)
endif()