//#include <catch2/catch_test_macros.hpp>

#include "catch.hpp"


#include "include/jada.hpp"
#include "test.hpp"


using namespace jada;

TEST_CASE("Test index_conversions"){


    SECTION("get_shift"){

        SECTION("1D"){
            size_type n0 = 10;
            std::array<size_type, 1> dims{n0};

            CHECK(get_shift<0, StorageOrder::RowMajor>(dims) == 1);
            CHECK(get_shift<0, StorageOrder::ColMajor>(dims) == 1);
        }

        SECTION("2D"){

            size_type n0 = 10;
            size_type n1 = 11;
            std::array<size_type, 2> dims{n0, n1};

            //[nj, ni]
            CHECK(get_shift<0, StorageOrder::RowMajor>(dims) == n1); //ni
            CHECK(get_shift<1, StorageOrder::RowMajor>(dims) == 1);

            //[ni, nj]
            CHECK(get_shift<0, StorageOrder::ColMajor>(dims) == 1);
            CHECK(get_shift<1, StorageOrder::ColMajor>(dims) == n0); //ni
        }

        SECTION("3D"){

            size_type n0 = 10;
            size_type n1 = 1;
            size_type n2 = 12;
            std::array<size_type, 3> dims{n0, n1, n2};

            CHECK(get_shift<0, StorageOrder::RowMajor>(dims) == n1 * n2);
            CHECK(get_shift<1, StorageOrder::RowMajor>(dims) == n2);
            CHECK(get_shift<2, StorageOrder::RowMajor>(dims) == 1);

            CHECK(get_shift<0, StorageOrder::ColMajor>(dims) == 1);
            CHECK(get_shift<1, StorageOrder::ColMajor>(dims) == n0);
            CHECK(get_shift<2, StorageOrder::ColMajor>(dims) == n0 * n1);
        }
    
    }


    SECTION("flatten"){

        SECTION("1D"){
            
            using dimension = std::array<size_type, 1>;
            using position = std::array<index_type, 1>;
            dimension dim{12};
            position pos;

            pos = {3};
            CHECK(flatten<StorageOrder::RowMajor>(pos, dim) == 3);
            CHECK(flatten<StorageOrder::ColMajor>(pos, dim) == 3);

            pos = {0};
            CHECK(flatten<StorageOrder::RowMajor>(pos, dim) == 0);
            CHECK(flatten<StorageOrder::ColMajor>(pos, dim) == 0);
            
            pos = {11};
            CHECK(flatten<StorageOrder::RowMajor>(pos, dim) == 11);
            CHECK(flatten<StorageOrder::ColMajor>(pos, dim) == 11);
           
            #ifdef DEBUG
            pos = {12};
            REQUIRE_THROWS(flatten<StorageOrder::RowMajor>(pos, dim));
            REQUIRE_THROWS(flatten<StorageOrder::ColMajor>(pos, dim));
            #endif

        }
        
        SECTION("2D"){

            using dimension = std::array<size_type, 2>;
            using position = std::array<index_type, 2>;
            dimension dim{4,5};
            
            position pos;


            pos = {0,0};
            CHECK(flatten<StorageOrder::RowMajor>(pos, dim) == 0);
            CHECK(flatten<StorageOrder::ColMajor>(pos, dim) == 0);

            pos = {1,1};
            CHECK(flatten<StorageOrder::RowMajor>(pos, dim) == 6);
            CHECK(flatten<StorageOrder::ColMajor>(pos, dim) == 5);

            #ifdef DEBUG
            pos = {5,1};
            REQUIRE_THROWS(flatten<StorageOrder::RowMajor>(pos, dim));
            REQUIRE_THROWS(flatten<StorageOrder::ColMajor>(pos, dim));
            #endif
        }
        
        
        SECTION("3D"){

            using dimension = std::array<size_type, 3>;
            using position = std::array<index_type, 3>;

            dimension dim{4,1,6};
            position pos;

            pos = {0,0,0};
            CHECK(flatten<StorageOrder::RowMajor>(pos, dim) == 0);
            CHECK(flatten<StorageOrder::ColMajor>(pos, dim) == 0);

            pos = {1,0,3};
            CHECK(flatten<StorageOrder::RowMajor>(pos, dim) == 9);
            CHECK(flatten<StorageOrder::ColMajor>(pos, dim) == 13);

            #ifdef DEBUG
            pos = {1,1,3};
            REQUIRE_THROWS(flatten<StorageOrder::RowMajor>(pos, dim));
            REQUIRE_THROWS(flatten<StorageOrder::ColMajor>(pos, dim));
            #endif

        }

        
        SECTION("4D"){
            
            using dimension = std::array<size_type, 4>;
            using position = std::array<index_type, 4>;

            dimension dim{4,1,6,5};
            position pos;

            pos = {0,0,0,0};
            CHECK(flatten<StorageOrder::RowMajor>(pos, dim) == 0);
            CHECK(flatten<StorageOrder::ColMajor>(pos, dim) == 0);

            pos = {1,0,3,2};
            CHECK(flatten<StorageOrder::RowMajor>(pos, dim) == 47);
            CHECK(flatten<StorageOrder::ColMajor>(pos, dim) == 61);

            #ifdef DEBUG
            pos = {1,1,3,4};
            REQUIRE_THROWS(flatten<StorageOrder::RowMajor>(pos, dim));
            REQUIRE_THROWS(flatten<StorageOrder::ColMajor>(pos, dim));
            #endif

        }
        

    }

    SECTION("unflatten"){


        SECTION("1D"){

            using dimension = std::array<size_type, 1>;
            using position = std::array<index_type, 1>;

            index_type pos;
            dimension dim{7};

            pos = 0;
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{0});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{0});

            pos = 3;
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{3});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{3});

            #ifdef DEBUG
            pos = 7;
            REQUIRE_THROWS(unflatten<StorageOrder::RowMajor>(pos, dim));
            REQUIRE_THROWS(unflatten<StorageOrder::ColMajor>(pos, dim));
            #endif
        }



        SECTION("2D"){
            using dimension = std::array<size_type, 2>;
            using position = std::array<index_type, 2>;
            dimension dim{3, 6};
            index_type pos;
            
            pos = 0;
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{0,0});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{0,0});

            pos = 4;
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{0,4});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{1,1});

            pos = 7;
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{1,1});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{1,2});

            pos = 16;
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{2,4});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{1,5});

            #ifdef DEBUG
            pos = 18;
            REQUIRE_THROWS(unflatten<StorageOrder::RowMajor>(pos, dim));
            REQUIRE_THROWS(unflatten<StorageOrder::ColMajor>(pos, dim));
            #endif

        }
        
        SECTION("3D"){
            using dimension = std::array<size_type, 3>;
            using position = std::array<index_type, 3>;
            dimension dim{3, 1, 8};
            index_type pos;

            pos = 0;    
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{0,0,0});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{0,0,0});

            pos = 6;
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{0,0,6});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{0,0,2});

            pos = 1;
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{0,0,1});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{1,0,0});

            pos = 23;
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{2,0,7});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{2,0,7});


            #ifdef DEBUG
            pos = 24;
            REQUIRE_THROWS(unflatten<StorageOrder::RowMajor>(pos, dim));
            REQUIRE_THROWS(unflatten<StorageOrder::ColMajor>(pos, dim));
            #endif

        }
        SECTION("4D"){
            using dimension = std::array<size_type, 4>;
            using position = std::array<index_type, 4>;
            dimension dim{3, 1, 8, 1};
            index_type pos;

            pos = 0;
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{0,0,0,0});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{0,0,0,0});

            pos = 2;
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{0,0,2,0});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{2,0,0,0});

            pos = 3;
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{0,0,3,0});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{0,0,1,0});

            pos = 23;
            CHECK(unflatten<StorageOrder::RowMajor>(pos, dim) == position{2,0,7,0});
            CHECK(unflatten<StorageOrder::ColMajor>(pos, dim) == position{2,0,7,0});


            #ifdef DEBUG
            pos = 24;
            REQUIRE_THROWS(unflatten<StorageOrder::RowMajor>(pos, dim));
            REQUIRE_THROWS(unflatten<StorageOrder::ColMajor>(pos, dim));
            #endif

        }

    }
    
}

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

        
        std::vector<index_type> is;
        std::vector<index_type> js;

        
        for (auto tpl : t){

            is.push_back(std::get<0>(tpl));
            js.push_back(std::get<1>(tpl));            
        }
              
        CHECK(is == std::vector<index_type>{0,0,1,1});
        CHECK(js == std::vector<index_type>{0,1,0,1});
        
    }
    
    
    
    
    SECTION("Parallel"){

        const auto t = md_indices(std::array{0,0}, std::array{2,2});

        
        std::vector<index_type> v(4);

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

        CHECK(v == std::vector<index_type>{0,0,0,1});

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

        
        std::vector<index_type> f(10);

        auto span = make_span(f, extents<2>{2,5});


        std::vector<index_type> is;
        std::vector<index_type> js;

        for (auto [i, j] : all_indices(span)){
            is.push_back(i);
            js.push_back(j);
        }

        CHECK(is == std::vector<index_type>{0,0,0,0,0,1,1,1,1,1});
        CHECK(js == std::vector<index_type>{0,1,2,3,4,0,1,2,3,4});
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

TEST_CASE("algorithms"){

    SECTION("for_each"){
        
        SECTION("serial"){
            size_type ni = 3;
            size_type nj = 2;
            std::vector<int> a(ni*nj, 0);

            auto aa = make_span(a, extents<2>{nj, ni});

            auto op = [](int& asd){
                asd = 43;
            };

            for_each( aa, op);

            CHECK(a == std::vector<int>(ni*nj, 43));

        }

        SECTION("parallel"){
            size_type ni = 3;
            size_type nj = 2;
            std::vector<int> a(ni*nj, 0);

            auto aa = make_span(a, extents<2>{nj, ni});

            auto op = [](int& asd){
                asd = 43;
            };

            for_each(std::execution::par_unseq, aa, op);

            CHECK(a == std::vector<int>(ni*nj, 43));

        }

    }

    SECTION("for_each_indexed"){
        
        SECTION("serial"){
            size_type ni = 3;
            size_type nj = 2;
            std::vector<int> a(ni*nj, 0);

            auto aa = make_span(a, extents<2>{nj, ni});

            auto op = [](auto idx, int& asd){
                auto j = std::get<0>(idx);
                auto i = std::get<1>(idx);
                asd = int(i + j);
            };

            for_each_indexed(aa, op);

            std::vector<int> correct = 
            {
                0,1,2,
                1,2,3
            };

            CHECK(a == correct);

        }
        
        SECTION("parallel"){
            size_type ni = 3;
            size_type nj = 2;
            std::vector<int> a(ni*nj, 0);

            auto aa = make_span(a, extents<2>{nj, ni});

            auto op = [](auto idx, int& asd){
                auto j = std::get<0>(idx);
                auto i = std::get<1>(idx);
                asd = int(i + j);
            };

            for_each_indexed(std::execution::par_unseq, aa, op);

            std::vector<int> correct = 
            {
                0,1,2,
                1,2,3
            };

            CHECK(a == correct);

        }

    }
    SECTION("transform"){

        SECTION("serial"){
            size_type ni = 3;
            size_type nj = 2;
            std::vector<int> a(ni*nj, 1);
            std::vector<int> b(ni*nj, -1);

            auto aa = make_span(a, extents<2>{nj, ni});
            auto bb = make_span(b, extents<2>{nj, ni});

            auto op = [](int e){
                return e + 2;
            };

            transform(aa, bb, op);


            CHECK(b == std::vector<int>(ni*nj, 3));

        }
        
        SECTION("parallel"){
            size_type ni = 3;
            size_type nj = 2;
            std::vector<int> a(ni*nj, 1);
            std::vector<int> b(ni*nj, -1);

            auto aa = make_span(a, extents<2>{nj, ni});
            auto bb = make_span(b, extents<2>{nj, ni});

            auto op = [](int e){
                return e + 2;
            };

            transform(std::execution::par_unseq, aa, bb, op);


            CHECK(b == std::vector<int>(ni*nj, 3));

        }

    }
    
    SECTION("transform_indexed"){
        
        SECTION("serial"){
            size_type ni = 3;
            size_type nj = 2;
            std::vector<int> a(ni*nj, 1);
            std::vector<int> b(ni*nj, -1);

            auto aa = make_span(a, extents<2>{nj, ni});
            auto bb = make_span(b, extents<2>{nj, ni});

            auto op = [](auto idx, int e){
                auto j = std::get<0>(idx);
                auto i = std::get<1>(idx);

                return e + int(i + j);
            };

            transform_indexed(aa, bb, op);

            std::vector<int> correct = 
            {
                1,2,3,
                2,3,4
            };

            CHECK(b == correct);

        }

        SECTION("parallel"){
            size_type ni = 3;
            size_type nj = 2;
            std::vector<int> a(ni*nj, 1);
            std::vector<int> b(ni*nj, -1);

            auto aa = make_span(a, extents<2>{nj, ni});
            auto bb = make_span(b, extents<2>{nj, ni});

            auto op = [](auto idx, int e){
                auto j = std::get<0>(idx);
                auto i = std::get<1>(idx);

                return e + int(i + j);
            };

            transform_indexed(std::execution::par_unseq, aa, bb, op);

            std::vector<int> correct = 
            {
                1,2,3,
                2,3,4
            };

            CHECK(b == correct);

        }

    }

    SECTION("window_transform"){
        
        SECTION("serial"){
            size_type ni = 4;
            size_type nj = 3;
            std::vector<int> a(ni*nj, 1);
            std::vector<int> b(ni*nj, -1);

            auto temp1 = make_span(a, extents<2>{nj, ni});
            auto temp2 = make_span(b, extents<2>{nj, ni});

            auto aa = make_subspan(temp1,
                                   std::array<index_type, 2>{1, 1},
                                   std::array<index_type, 2>{2, 3});
            auto bb = make_subspan(temp2,
                                   std::array<index_type, 2>{1, 1},
                                   std::array<index_type, 2>{2, 3});

            auto op = [](auto f) {
                return f(0,-1) + f(0,1);
            };

            window_transform(aa, bb, op);

            std::vector<int> correct = 
            {
                -1,-1,-1,-1,
                -1,+2,+2,-1,
                -1,-1,-1,-1
            };

            CHECK(b == correct);

        }

        SECTION("parallel"){
            size_type ni = 4;
            size_type nj = 3;
            std::vector<int> a(ni*nj, 1);
            std::vector<int> b(ni*nj, -1);

            auto temp1 = make_span(a, extents<2>{nj, ni});
            auto temp2 = make_span(b, extents<2>{nj, ni});

            auto aa = make_subspan(temp1,
                                   std::array<index_type, 2>{1, 1},
                                   std::array<index_type, 2>{2, 3});
            auto bb = make_subspan(temp2,
                                   std::array<index_type, 2>{1, 1},
                                   std::array<index_type, 2>{2, 3});

            auto op = [](auto f) {
                return f(0,-1) + f(0,1);
            };

            window_transform(std::execution::par_unseq, aa, bb, op);

            std::vector<int> correct = 
            {
                -1,-1,-1,-1,
                -1,+2,+2,-1,
                -1,-1,-1,-1
            };

            CHECK(b == correct);

        }

    }


    SECTION("tile_transform"){

        
        SECTION("serial"){
            size_type ni = 4;
            size_type nj = 3;
            std::vector<int> a(ni*nj, 1);
            std::vector<int> b(ni*nj, -1);

            auto temp1 = make_span(a, extents<2>{nj, ni});
            auto temp2 = make_span(b, extents<2>{nj, ni});

            auto aa = make_subspan(temp1,
                                   std::array<index_type, 2>{1, 1},
                                   std::array<index_type, 2>{2, 3});
            auto bb = make_subspan(temp2,
                                   std::array<index_type, 2>{1, 1},
                                   std::array<index_type, 2>{2, 3});

            auto op = [](auto f) {
                return f(-1) + f(1);
            };

            tile_transform<0>(aa, bb, op);

            std::vector<int> correct1 = 
            {
                -1,-1,-1,-1,
                -1,+2,+2,-1,
                -1,-1,-1,-1
            };

            CHECK(b == correct1);

            b.assign(b.size(), -3);
            tile_transform<1>(aa, bb, op);
            
            std::vector<int> correct2 = 
            {
                -3,-3,-3,-3,
                -3,+2,+2,-3,
                -3,-3,-3,-3
            };

            CHECK(b == correct2);





        }
        
        SECTION("parallel"){
            size_type ni = 4;
            size_type nj = 3;
            std::vector<int> a(ni*nj, 1);
            std::vector<int> b(ni*nj, -1);

            auto temp1 = make_span(a, extents<2>{nj, ni});
            auto temp2 = make_span(b, extents<2>{nj, ni});

            auto aa = make_subspan(temp1,
                                   std::array<index_type, 2>{1, 1},
                                   std::array<index_type, 2>{2, 3});
            auto bb = make_subspan(temp2,
                                   std::array<index_type, 2>{1, 1},
                                   std::array<index_type, 2>{2, 3});

            auto op = [](auto f) {
                return f(-1) + f(1);
            };

            tile_transform<0>(std::execution::par, aa, bb, op);

            std::vector<int> correct1 = 
            {
                -1,-1,-1,-1,
                -1,+2,+2,-1,
                -1,-1,-1,-1
            };

            CHECK(b == correct1);

            b.assign(b.size(), -3);
            tile_transform<1>(std::execution::par, aa, bb, op);
            
            std::vector<int> correct2 = 
            {
                -3,-3,-3,-3,
                -3,+2,+2,-3,
                -3,-3,-3,-3
            };

            CHECK(b == correct2);

        }

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

TEST_CASE("boundary_indices"){
        
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

            for_each_index(boundary_indices(dimensions(aa), dir), op);

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

            for_each_index(boundary_indices(dimensions(aa), dir), op);

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

            for_each_index(boundary_indices(dimensions(aa), dir), op);

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

            for_each_index(boundary_indices(dimensions(aa), dir), op);

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

    SECTION("Assign to ghost"){
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

        evaluate_boundary_condition(internal, boundary_op, dir);

        std::vector<int> correct = 
        {
            1,  2,  3,  4,
            5,  6,  7,  8,
            9,  10, 11, 12,
            13, 10, 11, 16
        };

        CHECK(a == correct);

    }
    
    SECTION("Assign to internal"){
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
            f(0) = f(1);
        };

        std::array<index_type, 2> dir = {1,0};

        evaluate_boundary_condition(internal, boundary_op, dir);

        std::vector<int> correct = 
        {
            1,  2,  3,  4,
            5,  6,  7,  8,
            9,  14, 15, 12,
            13, 14, 15, 16
        };

        CHECK(a == correct);

    }

    SECTION("evaluate_spatial_boundary_condition"){
        std::vector<int> a = 
        {
            1,  2,  3,  4,
            5,  6,  7,  8,
            9,  10, 11, 12,
            13, 14, 15, 16
        };

        auto s = make_span(a, extents<2>{4,4});
        auto internal = make_subspan(s, std::array<index_type,2>{1,1}, std::array<index_type, 2>{3,3});

        auto boundary_op = [](auto f, auto idx){
            auto j = std::get<0>(idx);
            auto i = std::get<1>(idx);
            (void) j;
            f(1) = i;
        };

        std::array<index_type, 2> dir = {1,0};

        evaluate_spatial_boundary_condition(internal, boundary_op, dir);

        std::vector<int> correct = 
        {
            1,  2,  3,  4,
            5,  6,  7,  8,
            9,  10, 11, 12,
            13, 0, 1, 16
        };

        CHECK(a == correct);

    }
    
}

TEST_CASE("Stencil operations"){

    SECTION("1D cd-2"){
        extents<1> dims{7};

        std::vector<int> in(flat_size(dims), 0);
        std::vector<int> out(flat_size(dims), 0);
    
        auto temp1 = make_span(in, dims);
        auto temp2 = make_span(out, dims);
        set_linear<0>(temp1);

        auto s_in = make_subspan(temp1, std::array<index_type, 1>{1}, std::array<index_type, 1>{6});
        auto s_out = make_subspan(temp2, std::array<index_type, 1>{1}, std::array<index_type, 1>{6});
        
        tile_transform<0>(s_in, s_out, simpleDiff());         


        CHECK(out == std::vector<int>{0, 2, 2, 2, 2, 2, 0});

    }

    SECTION("2D cd-2"){

        SECTION("dir=0"){
            simpleDiff op;
            extents<2> dims{2 + op.begin_padding*2,3};

            std::vector<int> in(flat_size(dims), 0);
            std::vector<int> out(flat_size(dims), 0);

            auto temp1 = make_span(in, dims);
            auto temp2 = make_span(out, dims);
            set_linear<0>(temp1);

            auto s_in  = make_subspan(temp1,
                                    std::array<index_type, 2>{1, 0},
                                    std::array<index_type, 2>{3, 3});
            auto s_out = make_subspan(temp2,
                                    std::array<index_type, 2>{1, 0},
                                    std::array<index_type, 2>{3, 3});

            tile_transform<0>(std::execution::par, s_in, s_out, op);         

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

        SECTION("dir=1"){
            simpleDiff op;
            extents<2> dims{2,3 + 2*op.begin_padding};

            std::vector<int> in(flat_size(dims), 0);
            std::vector<int> out(flat_size(dims), 0);
            
            auto temp1 = make_span(in, dims);
            auto temp2 = make_span(out, dims);
            set_linear<1>(temp1);

            auto s_in  = make_subspan(temp1,
                                    std::array<index_type, 2>{0, 1},
                                    std::array<index_type, 2>{2, 4});
            auto s_out = make_subspan(temp2,
                                    std::array<index_type, 2>{0, 1},
                                    std::array<index_type, 2>{2, 4});

            tile_transform<1>(std::execution::par, s_in, s_out, op);         



            CHECK(
                out == std::vector<int>
                {
                    0, 2, 2, 2, 0,
                    0, 2, 2, 2, 0
                }
            );

        }


    }


}

TEST_CASE("split_to_subspans"){



    size_t nj = 6;
    size_t ni = 5;


    std::vector<int> v(ni * nj, 0);


    auto span = make_span(v, extents<2>{nj, ni});

    auto [beg, mid, end] = split_to_subspans<0>(span, 1, 2);

    for_each(beg, [](auto& e){e = 1;});
    for_each(mid, [](auto& e){e = 2;});
    for_each(end, [](auto& e){e = 3;});

    std::vector<int> correct = 
    {
        1,1,1,1,1,
        2,2,2,2,2,
        2,2,2,2,2,
        2,2,2,2,2,
        3,3,3,3,3,
        3,3,3,3,3
    };


    CHECK(v == correct);

}


TEST_CASE("min_max_offset"){


    SECTION("Test 1"){

        auto [min, max] = min_max_offset([](auto f){
            return f(-3) + f(324) + f(-65);
        });
        
        CHECK(min == -65);
        CHECK(max == 324);

    }
    
    SECTION("Test 2"){

        auto [min, max] = min_max_offset(simpleDiff{});
        
        CHECK(min == -1);
        CHECK(max == 1);

    }

    SECTION("Test 3"){
        constexpr auto pair = min_max_offset(simpleDiff{});

        constexpr auto min = std::get<0>(pair);
        constexpr auto max = std::get<1>(pair);

        static_assert(min == -1, "min_max_offset not constexpr");
        static_assert(max ==  1, "min_max_offset not constexpr");

    }

}



template<size_t Dir, class Beg, class Mid, class End>
struct TileOp{

    TileOp(Beg beg, Mid mid, End end) : 
    m_beg(beg),
    m_mid(mid),
    m_end(end)
    {

    }    


    constexpr size_t begin_padding(){
        auto [min, max] = min_max_offset(m_mid);
        return size_t(std::abs(min));
    }
    
    constexpr size_t end_padding(){
        auto [min, max] = min_max_offset(m_mid);
        return size_t(max);
    }


    static constexpr size_t direction(){return Dir;}

    Beg m_beg;
    Mid m_mid;
    End m_end;

};



void do_apply(auto i_span, auto o_span, auto tile_op){

    auto [i_beg, i_mid, i_end] = split_to_subspans<tile_op.direction()>(
        i_span, tile_op.begin_padding(), tile_op.end_padding());
    auto [o_beg, o_mid, o_end] = split_to_subspans<tile_op.direction()>(
        o_span, tile_op.begin_padding(), tile_op.end_padding());

    tile_transform<tile_op.direction()>
    (
        i_beg,
        o_beg,
        tile_op.m_beg
    );
    
    tile_transform<tile_op.direction()>
    (
        i_mid,
        o_mid,
        tile_op.m_mid
    );
    
    tile_transform<tile_op.direction()>
    (
        i_end,
        o_end,
        tile_op.m_end
    );

}
auto do_apply2(auto in, auto dims, auto tile_op){

    runtime_assert(flat_size(dims) == std::size(in), "dimensions mismatch in do apply2");

    decltype(in) out(std::size(in), 0); //!
    
    auto temp1 = make_span(in, dims);
    auto temp2 = make_span(out, dims);

    auto padding1 = tile_op.begin_padding();
    auto padding2 = tile_op.end_padding();


    auto begin = [=](){
        std::array<size_t, rank(dims)> ret{};
        for (size_t i = 0; i < rank(dims); ++i){
            ret[i] = padding1;
        }
        return ret;
    }();
    auto end = [=](){
        std::array<size_t, rank(dims)> ret{};
        for (size_t i = 0; i < rank(dims); ++i){
            ret[i] = dims[i] - padding2;
        }
        return ret;
    }();


    auto i_in = make_subspan(temp1, begin, end);
    auto i_out = make_subspan(temp2, begin, end);

    do_apply(i_in, i_out, tile_op);



    return out;



}




TEST_CASE("TEMP"){

    auto beg = simpleDiff{};
    auto mid = simpleDiff{};
    auto end = simpleDiff{};

    TileOp<0, decltype(beg), decltype(mid), decltype(end)> op(beg, mid, end);

    size_t nj = 6;
    size_t ni = 5;

    std::array<size_t, 2> dims = {nj, ni};

    std::vector<int> in(flat_size(dims), 0);

    set_linear<0>(make_span(in, dims));

    auto out = do_apply2(in, dims, op);


    std::vector<int> correct = 
    {
        0,0,0,0,0,
        0,2,2,2,0,
        0,2,2,2,0,
        0,2,2,2,0,
        0,2,2,2,0,
        0,0,0,0,0
    };

    CHECK(out == correct);



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

TEST_CASE("divide_equally"){

    CHECK(divide_equally<2>(100)  == std::array<size_t, 2>{10, 10});
    CHECK(divide_equally<2>(101)  == std::array<size_t, 2>{1, 101});
    CHECK(divide_equally<2>(10)  == std::array<size_t, 2>{2, 5});
    CHECK(divide_equally<2>(1)  == std::array<size_t, 2>{1,1});
    
    
    #ifdef DEBUG
    CHECK(divide_equally<2>(0)  == std::array<size_t, 2>{0,0});
    #endif

}



