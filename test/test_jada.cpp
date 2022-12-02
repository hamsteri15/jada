//#include <catch2/catch_test_macros.hpp>

#include "catch.hpp"


#include "include/jada.hpp"
#include "test.hpp"


using namespace jada;

TEST_CASE("rank"){


    CHECK(rank(std::make_tuple(1,2,3)) == 3);
    CHECK(rank(std::array<size_t, 2>{}) == 2);
    CHECK(rank(std::array<int, 2>{}) == 2);
    //CHECK(rank(std::make_tuple(1,2, 3.43)) == 3);

}

TEST_CASE("extents"){

    SECTION("constructors"){

        REQUIRE_NOTHROW(extents<0>{});
        REQUIRE_NOTHROW(extents<2>{});
        REQUIRE_NOTHROW(extents<2>{4,4});
        REQUIRE_NOTHROW(extents<3>{4,4,0});
        

    }
    

    SECTION("flat_size"){

        CHECK(flat_size(extents<3>{1,2,3}) == 1*2*3);
        CHECK(flat_size(extents<3>{1,0,3}) == 0);

    }

    SECTION("indices_in_bounds"){

        auto tpl = std::make_tuple(size_t(1), size_t(2), size_t(3));
        auto ext1 = make_extent(std::array<size_t, 3>{4,4,4});
        auto ext2 = make_extent(std::array<size_t, 3>{0,0,0});

        CHECK(indices_in_bounds(tpl, ext1) == true);
        CHECK(indices_in_bounds(tpl, ext2) == false);

        std::array<size_t, 3> arr{1,2,3};

        CHECK(indices_in_bounds(arr, ext1) == true);

    }
    
}




TEST_CASE("Test indices"){

    auto idx = indices(0, 3);

    CHECK(*std::begin(idx) == 0);
    CHECK(*(std::begin(idx)+1) == 1);
    CHECK(*(std::begin(idx)+2) == 2);

}


TEST_CASE("md_indices"){

    
    SECTION("Test 1"){
        
        auto t = md_indices(std::array{0,0}, std::array{2,2});

        
        std::vector<int> is;
        std::vector<int> js;

        
        for (auto tpl : t){

            is.push_back(std::get<0>(tpl));
            js.push_back(std::get<1>(tpl));            
        }
              
        CHECK(is == std::vector<int>{0,0,1,1});
        CHECK(js == std::vector<int>{0,1,0,1});
        
    }
    
    
    
    
    SECTION("Parallel"){

        const auto t = md_indices(std::array{0,0}, std::array{2,2});

        
        std::vector<int> v(4);

        auto s = make_span(v, std::array<size_t,1>{4});

        std::for_each(
            //std::execution::par, //TODO: this does not compile with -stdpar automatically
            std::begin(t),
            std::end(t),
            [=](auto idx){
                //std::cout << idx << std::endl;
                auto j = std::get<0>(idx);
                auto i = std::get<1>(idx);
                s[j*2 + i] = j*i;
            }
        );

        CHECK(v == std::vector<int>{0,0,0,1});

    }
    
    

}


TEST_CASE("mdspan tests"){

    
  
    SECTION("extents"){

        REQUIRE_NOTHROW(extents<3>{3,3,3});

        std::array<size_t, 3> dims = {3,3,3};
        REQUIRE_NOTHROW(extents<3>{dims});

        extents<3> a{2,2,2};
        extents<3> b{2,3,2};
        extents<3> c{2,3,2};

        CHECK(a != b);
        CHECK(b == c);

        //auto c = a + b;

    }

    
    SECTION("make_span"){

        SECTION("vector tests"){

            std::vector<int> a(10, 1);
            const std::vector<int> b(10, 1);

            auto s1 = make_span(a, extents<2>{2,5});
            auto s2 = make_span(b, extents<2>{2,5});
            //auto s3 = make_span(a, std::array<size_t, 2>{2,5});

            CHECK(s1(1,1) == s2(1,1));

            s1(1,1) = 43;
            CHECK(s1(1,1) == 43);
            //CHECK(s3(1,1) == 1);
        }

        

        SECTION("equal size span"){
            std::vector<double> f(10, 1.0);

            auto span = make_span(f, extents<2>{2,5});

            CHECK(span(0, 3) == 1.0);
            CHECK(span(1, 4) == 1.0);
            span(1,4) = 43.0;
            CHECK(span(1,4) == 43.0);

            static_assert(rank(span)== size_t(2), "rank() not found");
            CHECK(rank(span) == size_t(2));

            CHECK(span(std::array<size_t,2>{1,1}) == 1.0);

        }

        SECTION("non-equal size span"){
            
            #ifdef DEBUG
            std::vector<double> f(10, 1.0);

            REQUIRE_THROWS(make_span(f, extents<2>{3,5}));
            #endif
        }

    }
    


    SECTION("all_indices"){

        
        std::vector<int> f(10);

        auto span = make_span(f, extents<2>{2,5});


        std::vector<int> is;
        std::vector<int> js;

        for (auto [i, j] : all_indices(span)){
            is.push_back(i);
            js.push_back(j);
        }

        CHECK(is == std::vector<int>{0,0,0,0,0,1,1,1,1,1});
        CHECK(js == std::vector<int>{0,1,2,3,4,0,1,2,3,4});
    }


    SECTION("index span"){
        std::vector<int> f(10, 1.0);
        auto span = make_span(f, extents<2>{2,5});

        for (auto t : all_indices(span)){
            auto tt = tuple_to_array(t);
            span(tt) = 43;
        }

        for (auto t : all_indices(span)){
            auto tt = tuple_to_array(t);
            CHECK(span(tt) == 43);
        }

        //auto t = std::make_tuple<size_t, size_t>(1,1);
        //span(t) = 21;

    }


}

TEST_CASE("for_each_index"){

        size_type ni = 3;
        size_type nj = 4;
        std::vector<int> a(ni*nj, 0);
        const std::vector<int> b(ni*nj, 1);

        auto aa = make_span(a, extents<2>{nj, ni});
        auto bb = make_span(b, extents<2>{nj, ni});

        

        auto op = [=](auto idx){
                auto [i, j] = idx;
                aa(i, j) = bb(i, j);
        };

        std::array<index_type, 2> begin = {1,1};
        std::array<index_type, 2> end = {2,3};

        for_each_index(begin, end, op);

        std::vector<int> correct = 
        {
            0,0,0,
            0,1,1,
            0,0,0,
            0,0,0
        };

        CHECK(a == correct);

}

TEST_CASE("for_each_boundary_index"){
        
        size_type ni = 3;
        size_type nj = 4;


        SECTION("dir = {1,0}"){

            std::vector<int> a(ni*nj, 0);
            auto aa = make_span(a, extents<2>{nj, ni});
            auto op = [=](auto idx){
                    auto [i, j] = idx;
                    aa(i, j) = 1;
            };

            std::array<index_type, 2> dir = {1,0};

            for_each_boundary_index(dir, dimensions(aa), op);

            std::vector<int> correct = 
            {
                0,0,0,
                0,0,0,
                0,0,0,
                1,1,1
            };

            CHECK(a == correct);

        }
        SECTION("dir = {-1,0}"){

            std::vector<int> a(ni*nj, 0);
            auto aa = make_span(a, extents<2>{nj, ni});
            auto op = [=](auto idx){
                    auto [i, j] = idx;
                    aa(i, j) = 1;
            };

            std::array<index_type, 2> dir = {-1,0};

            for_each_boundary_index(dir, dimensions(aa), op);

            std::vector<int> correct = 
            {
                1,1,1,
                0,0,0,
                0,0,0,
                0,0,0
            };

            CHECK(a == correct);

        }
        SECTION("dir = {1,1}"){

            std::vector<int> a(ni*nj, 0);
            auto aa = make_span(a, extents<2>{nj, ni});
            auto op = [=](auto idx){
                    auto [i, j] = idx;
                    aa(i, j) = 1;
            };

            std::array<index_type, 2> dir = {1,1};

            for_each_boundary_index(dir, dimensions(aa), op);

            std::vector<int> correct = 
            {
                0,0,0,
                0,0,0,
                0,0,0,
                0,0,1
            };

            CHECK(a == correct);

        }

        SECTION("boundary of subspan"){
            std::vector<int> a(ni*nj, 0);
            auto aa_temp = make_span(a, extents<2>{nj, ni});
            auto aa = make_subspan(aa_temp, std::array<index_type,2>{1, 0}, std::array<index_type, 2>{3,3});
            auto op = [=](auto idx){
                    auto [i, j] = idx;
                    aa(i, j) = 1;
            };

            std::array<index_type, 2> dir = {0,-1};

            for_each_boundary_index(dir, dimensions(aa), op);

            std::vector<int> correct = 
            {
                0,0,0,
                1,0,0,
                1,0,0,
                0,0,0
            };

            CHECK(a == correct);

        }


}



TEST_CASE("subspan tests"){

    SECTION("make_subspan"){

        std::vector<int> a = 
        {
            0,0,0,0,
            0,1,1,0,
            0,1,1,0,
            0,0,0,0
        };
        auto s = make_span(a, extents<2>{4,4});
        auto ss = make_subspan(s, std::array<size_t,2>{1,1}, std::array<size_t,2>{3,3});
        CHECK(ss(0,0) == 1);

        CHECK(ss.extent(0) == 2);
        CHECK(ss.extent(1) == 2);

        ss(-1, -1) = 4;
        ss(2,2) = 7;
        ss(2,1) = 5;

        std::vector<int> correct = 
        {
            4,0,0,0,
            0,1,1,0,
            0,1,1,0,
            0,0,5,7
        };
        CHECK(a == correct);


    }
    

}

TEST_CASE("Test index_handle"){

    SECTION("idxhandle_md_to_md"){
        std::vector<int> a = 
        {
            1,  2,  3,  4,
            5,  6,  7,  8,
            9,  10, 11, 12,
            13, 14, 15, 16
        };
        auto s = make_span(a, extents<2>{4,4});

        std::array<size_t, 2> center = {1,1};


        auto ss = idxhandle_md_to_md(s, center);

        CHECK(ss(std::make_tuple(-1,0)) == 2);
        CHECK(ss(std::make_tuple(-1,-1)) == 1);
        CHECK(ss(std::make_tuple(1,0)) == 10);
        CHECK(ss(std::array<index_type, 2>{1,0}) == 10);
        
    }

    SECTION("idxhandle_md_to_oned"){
        std::vector<int> a = 
        {
            1,  2,  3,  4,
            5,  6,  7,  8,
            9,  10, 11, 12,
            13, 14, 15, 16
        };
        auto s = make_span(a, extents<2>{4,4});

        std::array<size_t, 2> center = {2,2};


        auto ss = idxhandle_md_to_oned<1>(s, center);

        CHECK(ss(-1) == 10);
        CHECK(ss(0) == 11);
        CHECK(ss(1) == 12);
        
        
        auto ss2 = idxhandle_md_to_oned<0>(s, center);

        CHECK(ss2(-1) == 7);
        CHECK(ss2(0) == 11);
        CHECK(ss2(1) == 15);
        
    }
        
    SECTION("idxhandle_boundary_md_to_oned"){
        std::vector<int> a = 
        {
            1,  2,  3,  4,
            5,  6,  7,  8,
            9,  10, 11, 12,
            13, 14, 15, 16
        };
        auto s = make_span(a, extents<2>{4,4});

        std::array<index_type, 2> center = {2,2};

        auto ss = idxhandle_boundary_md_to_oned(s, center, std::array<index_type, 2>{0,1});

        CHECK(ss(-2) == 9);
        CHECK(ss(-1) == 10);
        CHECK(ss(0) == 11);
        CHECK(ss(1) == 12);
        
        auto ss2 = idxhandle_boundary_md_to_oned(s, center, std::array<index_type, 2>{0,-1});
        

        CHECK(ss2(-1) == 12);
        CHECK(ss2(0) == 11);
        CHECK(ss2(1) == 10);
        ss2(0) = 43;
        CHECK(ss2(0) == 43);
        
    }


}

TEST_CASE("evaluate_boundary"){

    
    std::vector<int> a = 
    {
        1,  2,  3,  4,
        5,  6,  7,  8,
        9,  10, 11, 12,
        13, 14, 15, 16
    };

    auto s = make_span(a, extents<2>{4,4});
    auto internal = make_subspan(s, std::array<index_type,2>{1,1}, std::array<index_type, 2>{3,3});

    auto boundary_op = [](auto f){
        f(1) = f(0);
    };

    std::array<index_type, 2> dir = {1,0};
    
    evaluate_boundary(internal, boundary_op, dir);

    std::vector<int> correct = 
    {
        1,  2,  3,  4,
        5,  6,  7,  8,
        9,  10, 11, 12,
        13, 10, 11, 16
    };

    CHECK(a == correct);
}



TEST_CASE("1D cd-2"){

    
    SECTION("evaluate_tiled 1"){

        extents<1> dims{7};

        vector_t<int> in(flat_size(dims), 0);
        vector_t<int> out(flat_size(dims), 0);
    
        auto s_in = make_span(in, dims);
        auto s_out = make_span(out, dims);
        
        set_linear<0>(s_in);
        
        evaluate_tiled<0>(s_in, s_out, simpleDiff());
        CHECK(out == std::vector<int>{0, 2, 2, 2, 2, 2, 0});
        
    }
    
    
    
    SECTION("evaluate_tiled 2"){

        size_t N = 3000;
        extents<1> dims{N};

        std::vector<int> in(flat_size(dims));
        std::vector<int> out(flat_size(dims));
        auto s_in = make_span(in, dims);
        auto s_out = make_span(out, dims);
        
        set_linear<0>(s_in);
        
        evaluate_tiled<0>(s_in, s_out, simpleDiff());

        for (size_t i = 1; i < N-2; ++i){
            REQUIRE(out.at(i) == 2);
        }
        CHECK(out.at(0) == 0);
        CHECK(out.at(N-1) == 0);

    }
    

}



TEST_CASE("2D cd-2"){

    
    SECTION("0-dir evaluate_tiled"){

        simpleDiff op;

        extents<2> dims{2 + op.padding*2,3};

        vector_t<int> in(flat_size(dims), 0);
        vector_t<int> out(flat_size(dims), 0);

        set_linear<0>(make_span(in, dims));

        evaluate_tiled<0>(
            make_span(in, dims),
            make_span(out, dims),
            op
        );        

        CHECK(
            out == std::vector<int>
            {
                0, 0, 0,
                2, 2, 2,
                2, 2, 2,
                0, 0, 0
            }
        );
        
    }
        
     
    SECTION("1-dir evaluate_tiled"){

        simpleDiff op;
        extents<2> dims{2,3 + 2*op.padding};

        std::vector<int> in(flat_size(dims), 0);
        std::vector<int> out(flat_size(dims), 0);

        set_linear<1>(make_span(in, dims));

        evaluate_tiled<1>(
            make_span(in, dims),
            make_span(out, dims),
            op
        );        


        CHECK(
            out == std::vector<int>
            {
                0, 2, 2, 2, 0,
                0, 2, 2, 2, 0
            }
        );
        
    }    
    
    SECTION("Both dirs evaluate_tiled"){


        simpleDiff op0;
        simpleDiff op1;
        extents<2> dims{2 + 2*op0.padding,3 + 2*op1.padding};

        std::vector<int> in(flat_size(dims), 0);
        std::vector<int> out(flat_size(dims), 0);

        set_linear<0>(make_span(in, dims));

        evaluate_tiled<0>(
            make_span(in, dims),
            make_span(out, dims),
            op0
        );        


        CHECK(
            out == std::vector<int>
            {
                0, 0, 0, 0, 0,
                2, 2, 2, 2, 2,
                2, 2, 2, 2, 2,
                0, 0, 0, 0, 0
            }
        );

        std::transform(out.begin(), out.end(), out.begin(), [](auto a){(void) a; return 0.0;});

        set_linear<1>(make_span(in, dims));

        evaluate_tiled<1>(
            make_span(in, dims),
            make_span(out, dims),
            op1
        );        
        
        CHECK(
            out == std::vector<int>
            {
                0, 2, 2, 2, 0,
                0, 2, 2, 2, 0,
                0, 2, 2, 2, 0,
                0, 2, 2, 2, 0
            }
        );

    }
        
}


TEST_CASE("Block neighbours"){



    //ensures that same comparator is used for all array of array comparisons
    auto comp = [](auto lhs, auto rhs) {
        return std::lexicographical_compare
        (
            lhs.begin(),
            lhs.end(),
            rhs.begin(),
            rhs.end()
        );
    };


    SECTION("Neighbours"){

        REQUIRE_NOTHROW(Neighbours<3, ConnectivityType::Star>());
        REQUIRE_NOTHROW(Neighbours<3, ConnectivityType::Box>());

        CHECK(is_unique(Neighbours<3, ConnectivityType::Box>().get()));
        CHECK(is_unique(Neighbours<2, ConnectivityType::Star>().get()));
        CHECK(is_unique(Neighbours<4, ConnectivityType::Box>().get()));



        Neighbours<2, ConnectivityType::Star> ns1;
        Neighbours<2, ConnectivityType::Box> ns2;

//        static constexpr auto ns_const = Neighbours<2, ConnectivityType::Star>();

        constexpr Neighbours<2, ConnectivityType::Star> ns_const1;
        constexpr Neighbours<2, ConnectivityType::Box> ns_const2;

        CHECK(ns1.get().size() == 4);
        CHECK(ns2.get().size() == 8);

        static_assert(ns_const1.get().size() == 4, "Neighbours not constexpr");
        static_assert(ns_const2.get().size() == 8, "Neighbours not constexpr");

    }


    

    SECTION("Star connectivity"){

        SECTION("1D") {
            auto test = Neighbours<1, ConnectivityType::Star>().get();

            std::array<direction<1>, 2> correct
            {
                direction<1>{1}, direction<1>{-1}
            };

            std::sort(test.begin(), test.end());
            std::sort(correct.begin(), correct.end());

            CHECK(test == correct);

        }


        SECTION("2D") {

            //using namespace Catch::Matchers;

            std::array<direction<2>, 4> test = Neighbours<2, ConnectivityType::Star>().get();

            std::array<direction<2>, 4> correct
            {
                direction<2>{1, 0},
                direction<2>{0, 1},
                direction<2>{-1, 0},
                direction<2>{0, -1}
            };

            std::sort(test.begin(), test.end(), comp);
            std::sort(correct.begin(), correct.end(), comp);

            CHECK(test == correct);

            test[2][0] = 3;
            CHECK(test != correct);

        }


        SECTION("3D") {
            auto test = Neighbours<3, ConnectivityType::Star>().get();

            std::array<direction<3>, 6> correct
            {
                direction<3>{1, 0, 0},
                direction<3>{0, 1, 0},
                direction<3>{0, 0, 1},
                direction<3>{-1, 0, 0},
                direction<3>{0, -1, 0},
                direction<3>{0, 0, -1}

            };

            std::sort(test.begin(), test.end(), comp);
            std::sort(correct.begin(), correct.end(), comp);

            CHECK(test == correct);

            test[2][0] = 3;
            CHECK(test != correct);


        }

    }



    SECTION("Box connecitvity"){

        SECTION("1D") {
            auto test = Neighbours<1, ConnectivityType::Box>().get();

            std::array<direction<1>, 2> correct
            {
                direction<1>{1}, direction<1>{-1}
            };

            std::sort(test.begin(), test.end(), comp);
            std::sort(correct.begin(), correct.end(), comp);

            CHECK(test == correct);


        }


        SECTION("2D") {
            auto test = Neighbours<2, ConnectivityType::Box>().get();

            std::array<direction<2>, 8> correct
            {
                direction<2>{1, 0},
                direction<2>{0, 1},
                direction<2>{1, 1},
                direction<2>{-1, 0},
                direction<2>{0, -1},
                direction<2>{-1, -1},
                direction<2>{-1, 1},
                direction<2>{1, -1}

            };

            std::sort(test.begin(), test.end(), comp);
            std::sort(correct.begin(), correct.end(), comp);

            CHECK(test == correct);



        }

    }
    

}

TEST_CASE("Test decomposition"){

    SECTION("get_offset"){

        SECTION("Even splitting"){
            std::array<size_t, 1> global_grid_dims  = {4};
            std::array<size_t, 1> coord_dims = {4};

            CHECK(offset(std::array<size_t, 1>{0}, coord_dims, global_grid_dims) == std::array<size_t, 1>{0});
            CHECK(offset(std::array<size_t, 1>{1}, coord_dims, global_grid_dims) == std::array<size_t, 1>{1});
            CHECK(offset(std::array<size_t, 1>{2}, coord_dims, global_grid_dims) == std::array<size_t, 1>{2});
            CHECK(offset(std::array<size_t, 1>{3}, coord_dims, global_grid_dims) == std::array<size_t, 1>{3});

        }
        SECTION("Uneven splitting"){
            std::array<size_t, 1> global_grid_dims  = {5};
            std::array<size_t, 1> coord_dims = {4};

            CHECK(offset(std::array<size_t, 1>{0}, coord_dims, global_grid_dims) == std::array<size_t, 1>{0});
            CHECK(offset(std::array<size_t, 1>{1}, coord_dims, global_grid_dims) == std::array<size_t, 1>{1});
            CHECK(offset(std::array<size_t, 1>{2}, coord_dims, global_grid_dims) == std::array<size_t, 1>{2});
            CHECK(offset(std::array<size_t, 1>{3}, coord_dims, global_grid_dims) == std::array<size_t, 1>{3});

            #ifdef DEBUG
            REQUIRE_THROWS(offset(std::array<size_t, 1>{4}, coord_dims, global_grid_dims));
            #endif
        }
        
    }


    
    SECTION("local_extent()") {


        SECTION("Even splitting"){
            std::array<size_t, 3> global_grid_dims  = {4,4,4};
            std::array<size_t, 3> coord_dims = {4,4,4};

            CHECK(local_dimensions(std::array<size_t, 3>{0,0,0}, coord_dims, global_grid_dims) == std::array<size_t, 3>{1,1,1});
            CHECK(local_dimensions(std::array<size_t, 3>{1,1,1}, coord_dims, global_grid_dims) == std::array<size_t, 3>{1,1,1});
            CHECK(local_dimensions(std::array<size_t, 3>{0,1,0}, coord_dims, global_grid_dims) == std::array<size_t, 3>{1,1,1});
            CHECK(local_dimensions(std::array<size_t, 3>{3,1,0}, coord_dims, global_grid_dims) == std::array<size_t, 3>{1,1,1});

            #ifdef DEBUG
            REQUIRE_THROWS(local_dimensions(std::array<size_t, 3>{4,0,0}, coord_dims, global_grid_dims));
            #endif
        }
        SECTION("Uneven splitting"){
            std::array<size_t, 3> global_grid_dims  = {4,5,4};
            std::array<size_t, 3> coord_dims = {4,4,4};

            CHECK(local_dimensions(std::array<size_t, 3>{0,0,0}, coord_dims, global_grid_dims) == std::array<size_t, 3>{1,1,1});
            CHECK(local_dimensions(std::array<size_t, 3>{1,1,1}, coord_dims, global_grid_dims) == std::array<size_t, 3>{1,1,1});
            CHECK(local_dimensions(std::array<size_t, 3>{0,1,0}, coord_dims, global_grid_dims) == std::array<size_t, 3>{1,1,1});
            CHECK(local_dimensions(std::array<size_t, 3>{3,1,0}, coord_dims, global_grid_dims) == std::array<size_t, 3>{1,1,1});
            CHECK(local_dimensions(std::array<size_t, 3>{3,3,0}, coord_dims, global_grid_dims) == std::array<size_t, 3>{1,2,1});
            CHECK(local_dimensions(std::array<size_t, 3>{2,3,1}, coord_dims, global_grid_dims) == std::array<size_t, 3>{1,2,1});


        }
        
    }
}



