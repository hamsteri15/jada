#pragma once

#ifdef __CUDACC__
#include <ranges>
#include <thrust/execution_policy.h>
#include <thrust/for_each.h>
#include <thrust/iterator/counting_iterator.h>
//#define CUDA_HOSTDEV __host__ __device__
#define CUDA_HOSTDEV __device__
#else
#include <algorithm>
#define CUDA_HOSTDEV
#endif

namespace jada {

template <class Indices, class Op> 
static constexpr void for_each_index(Indices indices, Op op) {

    std::for_each(
        // std::execution::par,
        std::begin(indices),
        std::end(indices),
        [=](auto tpl) { op(tpl); });
}

} // namespace jada