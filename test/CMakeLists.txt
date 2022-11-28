
# Build catch main
add_library(test_main STATIC test_main.cpp)


#Unit tests#
add_executable(SerialTest.bin test_jada.cpp)


target_link_libraries(SerialTest.bin PRIVATE test_main project_options project_warnings)


if (${CMAKE_CXX_COMPILER_ID} MATCHES "NVHPC")
  #target_compile_options(SerialTest.bin PUBLIC -w -fast -Mnouniform -Mfprelaxed -stdpar=gpu)
  #target_link_options(SerialTest.bin PUBLIC -w -fast -Mnouniform -Mfprelaxed -stdpar=gpu)
  target_compile_options(SerialTest.bin PUBLIC -O1 -stdpar=gpu)
  target_link_options(SerialTest.bin PUBLIC -stdpar=gpu)
else()
  
#gcc
  #Configure tbb
  find_package(TBB REQUIRED)
  target_link_libraries(SerialTest.bin PUBLIC TBB::tbb)
  
endif()


#target_compile_options(SerialTest.bin PRIVATE -DDEBUG)
target_include_directories(SerialTest.bin PUBLIC
                            ${CMAKE_SOURCE_DIR}
#                            #${MPI_CXX_INCLUDE_PATH}
                          )

add_test( NAME SerialTest COMMAND SerialTest.bin)

