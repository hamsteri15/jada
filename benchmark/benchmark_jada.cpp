#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"

#include <ranges>
#include "include/jada.hpp"
#include "benchmark.hpp"
#include "reference_kernels.hpp"

using namespace jada;



TEST_CASE("serial"){

    SECTION("for_each"){

        size_t n = 200;

        
        BENCHMARK("1d"){
            std::vector<int> v(n, 0);
            auto op = [](auto& e){e = 42;};
            auto span = make_span(v, std::array<size_t, 1>{n});
            for_each(span, op);
            return v;
        };
        
        BENCHMARK("2d"){
            auto dims = divide_equally<2>(n);
            size_t ni = dims[1];
            size_t nj = dims[0];

            CHECK(n == ni*nj);

            std::vector<int> v(ni*nj, 0);
            auto op = [](auto& e){e = 42;};
            auto span = make_span(v, std::array<size_t, 2>{nj, ni});
            for_each(span, op);
            return v;
        
        };
        


    }


}

/*
TEST_CASE("2D benchmarks"){
    
    size_t nj = 50;
    size_t ni = 51;

    double Lx = 1.0;
    double Ly = 1.0;
    
    double dx = Lx / double(ni);
    double dy = Ly / double(nj);

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
    
    double dx = Lx / double(ni);
    double dy = Ly / double(nj);
    double dz = Lz / double(nk);

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
*/
