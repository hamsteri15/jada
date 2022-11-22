#define CATCH_CONFIG_ENABLE_BENCHMARKING
#define CATCH_CONFIG_MAIN // This tells the catch header to generate a main
#include "catch.hpp"

#include <omp.h>

#include "include/jada.hpp"
#include "test.hpp"

using namespace jada;

template<class Span1, class Span2>
void ref_kernel_2d_i(Span1 in, Span2 out, double delta){

    size_t nj = in.extent(0);
    size_t ni = in.extent(1);

    for (size_t j = 2; j < nj-2; ++j){
    for (size_t i = 2; i < ni-2; ++i){

        out(j, i) =
        (
            -     in(j, i+2)
            + 8.0*in(j, i+1)
            - 8.0*in(j, i-1)
            +     in(j, i-2) 
        ) / 12.0 * delta;
    }}
}



template<class Span1, class Span2>
void ref_kernel_2d_j(Span1 in, Span2 out, double delta){

    size_t nj = in.extent(0);
    size_t ni = in.extent(1);

    for (size_t j = 2; j < nj-2; ++j){
    for (size_t i = 2; i < ni-2; ++i){

        out(j, i) =
        (
            -     in(j+2, i)
            + 8.0*in(j+1, i)
            - 8.0*in(j-1, i)
            +     in(j-2, i) 
        ) / 12.0 * delta;
    }}
}




template<class Span1, class Span2>
void ref_kernel_3d_i(Span1 in, Span2 out, double delta){

    #pragma omp parallel
    {


    size_t nk = in.extent(0);
    size_t nj = in.extent(1);
    size_t ni = in.extent(2);

    #pragma omp for schedule(dynamic)
    for (size_t k = 2; k < nk-2; ++k){
    for (size_t j = 2; j < nj-2; ++j){
    for (size_t i = 2; i < ni-2; ++i){

        out(k, j, i) =
        (
            -     in(k, j, i+2)
            + 8.0*in(k, j, i+1)
            - 8.0*in(k, j, i-1)
            +     in(k, j, i-2) 
        ) / 12.0 * delta;
    }}}
    }
}

template<class Span1, class Span2>
void ref_kernel_3d_j(Span1 in, Span2 out, double delta){

    #pragma omp parallel
    {

    size_t nk = in.extent(0);
    size_t nj = in.extent(1);
    size_t ni = in.extent(2);

    #pragma omp for schedule(dynamic)
    for (size_t k = 2; k < nk-2; ++k){
    for (size_t j = 2; j < nj-2; ++j){
    for (size_t i = 2; i < ni-2; ++i){

        out(k, j, i) =
        (
            -     in(k, j+2, i)
            + 8.0*in(k, j+1, i)
            - 8.0*in(k, j-1, i)
            +     in(k, j-2, i) 
        ) / 12.0 * delta;
    }}}
    }
}

template<class Span1, class Span2>
void ref_kernel_3d_k(Span1 in, Span2 out, double delta){

    #pragma omp parallel
    {

    size_t nk = in.extent(0);
    size_t nj = in.extent(1);
    size_t ni = in.extent(2);

    #pragma omp for schedule(dynamic)
    for (size_t k = 2; k < nk-2; ++k){
    for (size_t j = 2; j < nj-2; ++j){
    for (size_t i = 2; i < ni-2; ++i){

        out(k, j, i) =
        (
            -     in(k+2, j, i)
            + 8.0*in(k+1, j, i)
            - 8.0*in(k-1, j, i)
            +     in(k-2, j, i) 
        ) / 12.0 * delta;
    }}}
    }
}

TEST_CASE("2D benchmarks"){
    
    size_t nj = 50;
    size_t ni = 51;

    double Lx = 1.0;
    double Ly = 1.0;
    
    double dx = Lx / ni;
    double dy = Ly / nj;

    std::array<size_t, 2> dims{nj, ni};
    std::vector<double> in_v(nj * ni); 
    std::vector<double> out_v(in_v.size()); 
    
    assign_random(in_v);
    assign_random(out_v);

    auto in = make_span(in_v, dims);
    auto out = make_span(out_v, dims);


    BENCHMARK("2D-i"){
        d_CD4 op(dx);
        evaluate_tiled<1>(in, out, op);
        return out;
    };

    BENCHMARK("Reference 2D-i"){
        ref_kernel_2d_i(in, out, dx);
        return out;
    };



    BENCHMARK("2D-j"){
        d_CD4 op(dy);
        evaluate_tiled<0>(in, out, op);
        return out;
    };

    BENCHMARK("Reference 2D-j"){
        ref_kernel_2d_j(in, out, dy);
        return out;
    };
    


}

TEST_CASE("3D benchmarks"){
    
    size_t nk = 201;
    size_t nj = 202;
    size_t ni = 203;

    double Lx = 1.0;
    double Ly = 1.0;
    double Lz = 1.0;
    
    double dx = Lx / ni;
    double dy = Ly / nj;
    double dz = Lz / nk;

    std::array<size_t, 3> dims{nk, nj, ni};
    std::vector<double> in_v(nk * nj * ni); 
    std::vector<double> out_v(in_v.size()); 
    
    assign_random(in_v);
    assign_random(out_v);
    
    auto in = make_span(in_v, dims);
    auto out = make_span(out_v, dims);


    BENCHMARK("3D-i"){
        d_CD4 op(dx);
        evaluate_tiled<2>(in, out, op);
        return out;
    };

    BENCHMARK("Reference 3D-i"){
        ref_kernel_3d_i(in, out, dx);
        return out;
    };


    
    BENCHMARK("3D-j"){
        d_CD4 op(dy);
        evaluate_tiled<1>(in, out, op);
        return out;
    };

    BENCHMARK("Reference 3D-j"){
        ref_kernel_3d_j(in, out, dy);
        return out;
    };
    


    BENCHMARK("3D-k"){
        d_CD4 op(dz);
        evaluate_tiled<0>(in, out, op);
        return out;
    };

    BENCHMARK("Reference 3D-k"){
        ref_kernel_3d_k(in, out, dz);
        return out;
    };
    


}

