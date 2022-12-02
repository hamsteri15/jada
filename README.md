# jada
A C++ library for generic hpc stencil operations

# dependencies
  * c++20 supporting compiler (g++11+, clang++11+, nvc++22.9+)
  * If benchmarks are compiled (BUILD_BENCHMARKS=Yes): OpenMP
  * If benchmarks are compiled with g++ or clang++ (CMAKE_CXX_COMPILER=g++/clang++): TBB
  
# compiling and running tests and benchmarks
````
cd jada
mkdir build
cd build
cmake -D CMAKE_CXX_COMPILER=g++ -D CMAKE_BUILD_TYPE=Release -D BUILD_BENCHMARKS=Yes -D BUILD_TESTS=Yes  ..
make
ctest
````
