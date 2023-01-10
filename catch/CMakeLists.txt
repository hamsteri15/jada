
# Build catch main
add_library(catch_main STATIC catch_main.cpp)
target_include_directories(catch_main PUBLIC ${CMAKE_SOURCE_DIR}/catch)
