#pragma once

#include <cstddef>

#include <omp.h>

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