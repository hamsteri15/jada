#pragma once


#ifdef __CUDACC__
#include <thrust/device_vector.h>
#include <thrust/device_malloc_allocator.h>
#include <thrust/host_vector.h>
#include <nvfunctional>
template<class T>
using vector_t = thrust::device_vector<T>;

#pragma message("using_cuda_vector")
#else
#include <vector>
template<class T>
using vector_t = std::vector<T>;
#pragma message("using_std_vector")
#endif




namespace jada {





template <size_t Dir, class Span> void set_linear(Span s) {
    
    
    
    auto op = [=](auto idx){
        using T = typename Span::value_type;
        auto ii                = std::get<Dir>(idx);
        s(tuple_to_array(idx)) = T(ii);
    };
    
    for_each_index(all_indices(s), op);
    
    
    
        
}


struct d_CD2 : public TiledStencil<d_CD2> {

    static constexpr size_t padding = 1;

    template <class F> auto operator()(F f) const { return 0.5 * (f(1) - f(-1)); }
};

struct d_CD4 : public TiledStencil<d_CD2> {

    static constexpr size_t padding = 2;

    d_CD4(double delta) : m_delta(delta) {}

    template <class F> auto operator()(F f) const { 
       return (-f(+2) + 8.0 * f(+1) - 8.0 * f(-1) + f(-2)) / m_delta;
    }

private:
    double m_delta;

};



template <class Container> bool is_unique(const Container& arr) {

    auto copy = arr;
    std::sort(std::begin(copy), std::end(copy));
    auto it       = std::unique(copy.begin(), copy.end());
    bool isUnique = (it == copy.end());

    return isUnique;
}


template <class Span> void print(Span span) {

    if constexpr (rank(span) == 1) {

        for (size_t i = 0; i < span.extent(0); ++i) { std::cout << span(i) << " "; }
        std::cout << std::endl;
    } else if constexpr (rank(span) == 2) {
        for (size_t i = 0; i < span.extent(0); ++i) {
            for (size_t j = 0; j < span.extent(1); ++j) { std::cout << span(i, j) << " "; }
            std::cout << std::endl;
        }
    } else {
        throw std::logic_error("Only ranks 1 and 2 spans can be printed");
    }
}


double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

template<class C>
void assign_random(C& c){

    for (auto& e : c){
        e = fRand(-32.8, 4532.3245);
    }


}


} // namespace jada