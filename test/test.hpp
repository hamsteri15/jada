#pragma once

#ifdef __CUDACC__
#include <thrust/device_vector.h>
#include <thrust/device_malloc_allocator.h>
#include <thrust/host_vector.h>
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

    using T = typename Span::value_type;

    auto indices = all_indices(s);
    for (auto idx : indices) {
        auto ii                = std::get<Dir>(idx);
        s(tuple_to_array(idx)) = T(ii);
    }
}
struct d_CD2 : public TiledStencil<d_CD2> {

    static constexpr size_t padding = 1;

    template <class F> auto operator()(F f) const { return 0.5 * (f(1) - f(-1)); }
};

template <class Container> bool is_unique(const Container& arr) {

    auto copy = arr;
    std::sort(std::begin(copy), std::end(copy));
    auto it       = std::unique(copy.begin(), copy.end());
    bool isUnique = (it == copy.end());

    return isUnique;
}

} // namespace jada