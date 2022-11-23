#pragma once

#ifdef __CUDACC__
#include <thrust/device_vector.h>
#endif


namespace jada{

template<class T>
static constexpr auto data(const T& t) -> decltype(t.data()){
    return std::data(t);
}

template<class T>
static constexpr auto data(T& t) -> decltype(t.data()){
    return std::data(t);
}


#ifdef __CUDACC__
template<class T>
static constexpr auto data(const thrust::device_vector<T>& t){
    return std::data(t).get();
}

template<class T>
static constexpr auto data(thrust::device_vector<T>& t){
    return std::data(t).get();
}
#endif


}
