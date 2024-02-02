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

    SECTION("assignment"){

        extents<2> e1{10, 11};
        extents<2> e2;
        REQUIRE_NOTHROW(e1 = e2);

    }


    SECTION("flat_size"){

        CHECK(flat_size(extents<3>{1,2,3}) == 1*2*3);
        CHECK(flat_size(extents<3>{1,0,3}) == 0);

    }

    SECTION("indices_in_bounds"){

        auto tpl = std::make_tuple(size_t(1), size_t(2), size_t(3));
        auto ext1 = make_extent(std::array<size_t, 3>{4,4,4});
        auto ext2 = make_extent(std::array<size_t, 3>{0,0,0});

        //std::cout << ext1 << std::endl;


        CHECK(indices_in_bounds(tpl, ext1) == true);
        CHECK(indices_in_bounds(tpl, ext2) == false);

        std::array<size_t, 3> arr{1,2,3};

        CHECK(indices_in_bounds(arr, ext1) == true);

    }


    SECTION("add_padding"){
        auto ext1 = make_extent(std::array<size_t, 3>{1,2,3});
        CHECK(
            add_padding(ext1, std::array{1,1,1}, std::array{2,2,2}) == extents<3>{4, 5, 6}
        );

        CHECK(add_padding(ext1, extents<3>{1,1,1}) == extents<3>{3,4,5});
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

TEST_CASE("Test box") {

    SECTION("Constructors") {
        Box<3> b({1, 1, 1}, {2, 2, 2});

        CHECK(b.get_extent() == extents<3>{1, 1, 1});

        CHECK(b.size() == 1 * 1 * 1); // NOTE!

    }
    SECTION("is_valid") {
        Box<3> b1({0, 0, 0}, {2, 2, 2});
        Box<3> b2({4, 0, 0}, {3, 3, 3});

        CHECK(b1.is_valid());
        CHECK(!b2.is_valid());
    }

    SECTION("Comparison") {

        Box<3> b1({1, 1, 1}, {2, 2, 2});
        Box<3> b2({1, 1, 1}, {2, 2, 2});
        Box<3> b3({1, 1, 1}, {5, 2, 2});

        CHECK(b1 == b2);
        CHECK(b1 != b3);
        CHECK(b1 == b1);
    }

    SECTION("translate") {

        Box<3> b1({1, 1, 1}, {2, 2, 2});
        CHECK(translate(b1, {1, 2, 0}) == Box<3>({2, 3, 1}, {3, 4, 2}));
    }

    SECTION("expand") {

        Box<3> b1({1, 1, 1}, {2, 2, 2});
        // b1.expand({0,0,2}, {0,1,0});

        CHECK(expand(b1, {0, 0, 2}, {0, 1, 0}) ==
              Box<3>({1, 1, -1}, {2, 3, 2}));

        Box<3> b2({1, 1, 1}, {2, 2, 2});
        CHECK(expand(b2, 1) == Box<3>({0, 0, 0}, {3, 3, 3}));
    }

    SECTION("merge") {

        SECTION("Test 1") {
            Box<3> b1({0, 0, 0}, {2, 2, 2});
            Box<3> b2({2, 2, 2}, {3, 3, 3});

            CHECK(merge(b1, b2) == Box<3>({0, 0, 0}, {3, 3, 3}));
        }
        SECTION("Test 2") {
            Box<3> b1({0, 0, 0}, {2, 2, 2});
            Box<3> b2({0, 0, 0}, {2, 2, 2});

            CHECK(merge(b1, b2) == Box<3>({0, 0, 0}, {2, 2, 2}));
        }
    }

    SECTION("intersection") {

        SECTION("Test 1") {
            Box<3> b1({0, 0, 0}, {2, 2, 2});
            Box<3> b2({0, 0, 0}, {3, 3, 3});

            CHECK(intersection(b1, b2) == Box<3>({0, 0, 0}, {2, 2, 2}));
        }
        SECTION("Test 2") {
            Box<3> b1({0, 0, 0}, {3, 3, 3});
            Box<3> b2({1, 1, 1}, {1, 1, 5});

            CHECK(intersection(b1, b2) == Box<3>({1, 1, 1}, {1, 1, 3}));
        }

        SECTION("Test 3") {
            Box<3> b1({0, 0, 0}, {2, 2, 2});
            Box<3> b2({5, 5, 5}, {8, 8, 8});

            CHECK(intersection(b1, b2) == Box<3>({0, 0, 0}, {0, 0, 0}));
        }


    }

    SECTION("Contains") {
        Box<3> b1({0, 0, 0}, {3, 3, 3});

        CHECK(b1.contains({0, 0, 0}));
        CHECK(!b1.contains({3, 3, 3})); // End is not contained

        CHECK(b1.contains({1, 1, 1}));
        CHECK(b1.contains({1, 0, 1}));

        Box<3> b2({0, 0, 0}, {1, 1, 1});

        CHECK(b1.contains(b2));
        CHECK(!b2.contains(b1));

        Box<3> b3({-1, 1, 1}, {0, 0, 0});
        CHECK(!b1.contains(b3));

        Box<3> b4({3, 3, 3}, {4, 4, 4});
        CHECK(!b1.contains(b4));


        SECTION("Bug 1"){

            Box<1> bb({0}, {10});

            CHECK(bb.contains(std::array<index_type, 1>{0}));

        }



    }

    SECTION("distance") {
        Box<3> b1({0, 0, 0}, {3, 3, 3});
        Box<3> b2({2, 2, 2}, {3, 3, 3});
        Box<3> b3({1, 2, 3}, {3, 3, 3});

        CHECK(distance(b1, b2) == std::array<index_type, 3>{2, 2, 2});
        CHECK(distance(b1, b3) == std::array<index_type, 3>{1, 2, 3});
    }

    SECTION("shared_edges"){
        Box<3> domain({0,0,0}, {1,2,3});


        CHECK
        (
            shared_edges(Box<3>({0,0,0}, {1,2,3}), domain) == std::make_pair
            (
                std::array{true, true, true},
                std::array{true, true, true}
            )
        );

        CHECK
        (
            shared_edges(Box<3>({0,1,2}, {3,3,3}), domain) == std::make_pair
            (
                std::array{true, false, false},
                std::array{false, false, true}
            )
        );
    }


    SECTION("edge_bounds"){

        Box<2> b({0,0}, {2, 3});

        std::vector<int> data(flat_size(b.get_extent()));

        auto span = make_span(data, b.get_extent());

        SECTION("dir {1,0}"){

            const std::array<index_type, 2> dir = {1, 0};
            std::fill(data.begin(), data.end(), 0);

            auto [begin, end] = edge_bounds(b, dir);

            auto subspan = make_subspan(span, begin, end);

            for_each(subspan, [](auto& e){e = 1;});


            CHECK(data == std::vector<int>
                {
                    0,0,0,
                    1,1,1
                }
            );
        }

        SECTION("dir {-1,0}"){

            const std::array<index_type, 2> dir = {-1, 0};
            std::fill(data.begin(), data.end(), 0);

            auto [begin, end] = edge_bounds(b, dir);

            auto subspan = make_subspan(span, begin, end);

            for_each(subspan, [](auto& e){e = 1;});


            CHECK(data == std::vector<int>
                {
                    1,1,1,
                    0,0,0
                }
            );
        }

        SECTION("dir {1,1}"){

            const std::array<index_type, 2> dir = {1, 1};
            std::fill(data.begin(), data.end(), 0);

            auto [begin, end] = edge_bounds(b, dir);

            auto subspan = make_subspan(span, begin, end);

            for_each(subspan, [](auto& e){e = 1;});


            CHECK(data == std::vector<int>
                {
                    0,0,0,
                    0,0,1
                }
            );
        }

    }




    SECTION("edge_indices"){



        Box<2> b({0,0}, {2, 3});

        SECTION("dir {0, 1}"){

            auto indices = edge_indices(b, std::array{0, 1});
            auto [j1, i1] = indices[0];
            CHECK(j1 == 0);
            CHECK(i1 == 2);

            auto [j2, i2] = indices[1];
            CHECK(j2 == 1);
            CHECK(i2 == 2);

        }

    }
    SECTION("get_edge"){

        SECTION("Test 1"){
            Box<2> b({0,0}, {2, 2});

            CHECK
            (
                get_edge(b, {0, 1}) == Box<2>({0, 1}, {2, 2})
            );
        }

        SECTION("Test 2"){
            Box<2> b({0,0}, {3, 3});

            CHECK
            (
                get_edge(b, {0, -1}) == Box<2>({0, 0}, {3, 1})
            );
        }

        SECTION("Test 3"){
            Box<2> b({1, 1}, {3, 3});

            std::vector<int> data(flat_size(b.get_extent()), 0);
            auto span = make_span(data, b.get_extent());

            std::fill(data.begin(), data.end(), 0);
            for (auto [j, i] : edge_indices(b, {0, -1})){
                span(j, i) = 1;
            }

            CHECK(data ==
                std::vector<int>
                {
                    1, 0,
                    1, 0
                }
            );

            std::fill(data.begin(), data.end(), 0);
            for (auto [j, i] : edge_indices(b, {-1, 0})){
                span(j, i) = 1;
            }

            CHECK(data ==
                std::vector<int>
                {
                    1, 1,
                    0, 0
                }
            );

            std::fill(data.begin(), data.end(), 0);
            for (auto [j, i] : edge_indices(b, {1, 1})){
                span(j, i) = 1;
            }

            CHECK(data ==
                std::vector<int>
                {
                    0, 0,
                    0, 1
                }
            );

        }
    }




    SECTION("shared_edge"){

        SECTION("Test 1"){
            const Box<2> domain({0,0}, {3,3});
            const Box<2> sub({0, 0}, {2,2});

            auto r1 = shared_edge(domain, sub, {0, 1});

            CHECK(r1 == Box<2>({0,0}, {0,0}));

        }

        SECTION("Test 2"){
            const Box<2> domain({0,0}, {3,3});
            const Box<2> sub({0, 0}, {2,2});

            auto r1 = shared_edge(domain, sub, {0, -1});

            CHECK(r1 == Box<2>({0,0}, {2,1}));
        }

    }


    SECTION("shared_edge_indices"){


        SECTION("Test 1"){

            const Box<2> domain({0,0}, {3,4});
            const Box<2> sub({0, 0}, {2,2});

            std::vector<int> data(flat_size(domain.get_extent()), 0);
            auto span = make_span(data, domain.get_extent());



            SECTION("dir {0, 1}"){
                std::fill(data.begin(), data.end(), 0);
                for (auto [j, i] : shared_edge_indices(domain, sub, {0,1})){
                    span(j, i) = 1;
                }

                CHECK
                (
                    data ==
                    std::vector<int> {0,0,0,0,
                                      0,0,0,0,
                                      0,0,0,0}
                );
            }



            SECTION("dir {0, -1}"){
                std::fill(data.begin(), data.end(), 0);
                for (auto [j, i] : shared_edge_indices(domain, sub, {0,-1})){
                    span(j, i) = 1;
                }

                CHECK
                (
                    data ==
                    std::vector<int> {1,0,0,0,
                                      1,0,0,0,
                                      0,0,0,0}
                );
            }

            SECTION("dir {-1, -1}"){
                std::fill(data.begin(), data.end(), 0);
                for (auto [j, i] : shared_edge_indices(domain, sub, {-1,-1})){
                    span(j, i) = 1;
                }

                CHECK
                (
                    data ==
                    std::vector<int> {1,0,0,0,
                                      0,0,0,0,
                                      0,0,0,0}
                );
            }

            SECTION("dir {1, 1}"){
                std::fill(data.begin(), data.end(), 0);
                for (auto [j, i] : shared_edge_indices(domain, sub, {1, 1})){
                    span(j, i) = 1;
                }

                CHECK
                (
                    data ==
                    std::vector<int> {0,0,0,0,
                                      0,0,0,0,
                                      0,0,0,0}
                );
            }

        }

        /*
        SECTION("boundary_loops"){

            auto domain = Box<2>{{0,0}, {3, 4}};
            auto topo = decompose(domain, 3, {false, false});
            std::vector<int> data(flat_size(domain.get_extent()), 0);

            auto bigspan = make_span(data, domain.get_extent());

            for (auto box : topo.get_boxes()){

                auto subspan = make_subspan(bigspan, box.box.begin, box.box.end);

                auto indices = shared_edge_indices(domain, box.box, {0, 1});

                for (auto [j,i] : indices){
                    subspan(j,i) = 1;
                }

            }

            CHECK
            (
                data ==
                std::vector<int> {0,0,0,1,
                                    0,0,0,1,
                                    0,0,0,1}
            );

        }
        */
    }

}




auto test_dec1d(){
    Box<1> domain({0}, {10});
    Box<1> b0({0}, {3});
    Box<1> b1({3}, {6});
    Box<1> b2({6}, {10});

    std::vector<BoxRankPair<1>> boxes{BoxRankPair{.box = b0, .rank = 0},
                                    BoxRankPair{.box = b1, .rank = 1},
                                    BoxRankPair{.box = b2, .rank = 2}};

    return std::make_pair(domain, boxes);
}

auto test_dec2d() {
    Box<2> domain({0, 0}, {10, 7});
    Box<2> b0({0, 0}, {4, 7});
    Box<2> b1({4, 5}, {7, 7});
    Box<2> b2({7, 5}, {10, 7});
    Box<2> b3({7, 3}, {10, 5});
    Box<2> b4({4, 0}, {10, 3});
    Box<2> b5({4, 3}, {7, 5});

    std::vector<BoxRankPair<2>> boxes{BoxRankPair{.box = b0, .rank = 0},
                                        BoxRankPair{.box = b1, .rank = 1},
                                        BoxRankPair{.box = b2, .rank = 2},
                                        BoxRankPair{.box = b3, .rank = 3},
                                        BoxRankPair{.box = b4, .rank = 4},
                                        BoxRankPair{.box = b5, .rank = 5}};

    return std::make_pair(domain, boxes);
}


TEST_CASE("Test topology") {

    SECTION("Constructors") {

        SECTION("Test 1") {
            Box<3> domain({0, 0, 0}, {3, 3, 3});
            Box<3> b1({0, 0, 0}, {1, 3, 3});
            Box<3> b2({1, 0, 0}, {2, 3, 3});
            Box<3> b3({2, 0, 0}, {3, 3, 3});

            std::vector<BoxRankPair<3>> boxes1{
                BoxRankPair{.box = b1, .rank = 0},
                BoxRankPair{.box = b2, .rank = 1},
                BoxRankPair{.box = b3, .rank = 1}};


            CHECK(Topology(domain, boxes1, {false, false, false}).is_valid());
        }

        SECTION("Test 2") {
            Box<3> domain({0, 0, 0}, {3, 3, 3});
            Box<3> b1({0, 0, 0}, {2, 3, 3});
            Box<3> b2({1, 0, 0}, {2, 3, 3});
            Box<3> b3({2, 0, 0}, {3, 3, 3});

            std::vector<BoxRankPair<3>> boxes1{
                BoxRankPair{.box = b1, .rank = 0},
                BoxRankPair{.box = b2, .rank = 1},
                BoxRankPair{.box = b3, .rank = 1}};

            CHECK(!Topology(domain, boxes1, {false, false, false}).is_valid());
        }
    }

    SECTION("found") {

        Box<3> domain({0, 0, 0}, {3, 3, 3});
        Box<3> b1({0, 0, 0}, {1, 3, 3});
        Box<3> b2({1, 0, 0}, {2, 3, 3});
        Box<3> b3({2, 0, 0}, {3, 3, 3});

        std::vector<BoxRankPair<3>> boxes1{BoxRankPair{.box = b1, .rank = 0},
                                           BoxRankPair{.box = b2, .rank = 1},
                                           BoxRankPair{.box = b3, .rank = 1}};

        Topology topo(domain, boxes1, {false, false, false});

        CHECK(topo.found(boxes1[1]));

        CHECK(!topo.found(BoxRankPair{.box = b1, .rank = 433}));
    }


    SECTION("get_transfers"){

        using namespace Catch::Matchers;


        SECTION("1D tests"){
            auto [domain, boxes] = test_dec1d();
            auto bpad = std::array<index_type ,1>{}; bpad.fill(1);
            auto epad = std::array<index_type ,1>{}; epad.fill(1);
            Topology topo(domain, boxes, {true});

            SECTION("Test 1"){

                auto transfers = topo.get_transfers(boxes[0], boxes[1], bpad, epad);


                CHECK(transfers.front().sender_begin ==
                      std::array<index_type, 1>{3});

                CHECK(transfers.front().receiver_begin ==
                      std::array<index_type, 1>{0});

                CHECK(transfers.front().extent ==
                      std::array<size_type, 1>{1});

            }


            SECTION("Test 2"){

                auto transfers = topo.get_transfers(boxes[1], boxes[0], bpad, epad);

                CHECK(transfers.front().sender_begin ==
                      std::array<index_type, 1>{1});

                CHECK(transfers.front().receiver_begin ==
                      std::array<index_type, 1>{4});

                CHECK(transfers.front().extent ==
                      std::array<size_type, 1>{1});

            }

            SECTION("Test 3 periodicity"){

                auto transfers = topo.get_transfers(boxes[0], boxes[2], bpad, epad);

                CHECK(transfers.front().sender_begin ==
                      std::array<index_type, 1>{1});

                CHECK(transfers.front().receiver_begin ==
                      std::array<index_type, 1>{5});

                CHECK(transfers.front().extent ==
                      std::array<size_type, 1>{1});

            }


        }


        SECTION("2D tests"){

            SECTION("Test 1") {
                auto [domain, boxes] = test_dec2d();
                auto bpad = std::array<index_type ,2>{}; bpad.fill(1);
                auto epad = std::array<index_type ,2>{}; epad.fill(1);
                Topology topo(domain, boxes, {false, false});

                auto transfers = topo.get_transfers(boxes[0], boxes[1], bpad, epad);



                TransferInfo<2> correct
                {
                    .sender_rank = boxes[0].rank,
                    .receiver_rank = boxes[1].rank,
                    .sender_begin = std::array<index_type, 2>{4,5},
                    .receiver_begin = std::array<index_type, 2>{0,0},
                    .extent = std::array<size_type, 2>{1,3},
                };


                CHECK(transfers.front() == correct);

            }
        }



    }


    SECTION("make_subspans"){

        SECTION("Test 1"){
            Box<1> domain({0}, {10});
            Box<1> b0({0}, {3});
            Box<1> b1({3}, {6});
            Box<1> b2({6}, {10});

            std::vector<BoxRankPair<1>> boxes{BoxRankPair{.box = b0, .rank = 0},
                                            BoxRankPair{.box = b1, .rank = 0},
                                            BoxRankPair{.box = b2, .rank = 0}};
            auto bpad = std::array<index_type ,1>{}; bpad.fill(1);
            auto epad = std::array<index_type ,1>{}; epad.fill(1);
            Topology topo(domain, boxes, {false});

            std::vector<int> data(10);
            std::iota(data.begin(), data.end(), 0);

            CHECK(make_subspans(data, topo, 32).size() == 0);

            auto spans = make_subspans(data, topo, 0);

            CHECK(spans[0](0) == 0);
            CHECK(spans[0](1) == 1);
            CHECK(spans[1](0) == 3);
            CHECK(spans[1](1) == 4);
        }




        SECTION("bug 1"){


            index_type ny = 3;
            index_type nx = 4;

            Box<2> domain({0,0}, {ny, nx});

            std::vector<int> data(flat_size(domain.get_extent()));

            for (int world_size = 1; world_size < 4; ++world_size){

                auto topo = decompose(domain, world_size, {false, false});

                for (int rank = 0; rank < world_size; ++rank){
                    REQUIRE_NOTHROW(make_subspans(data, topo, rank));
                }

            }
        }

    }
}



TEST_CASE("Test Neighbours"){

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

    SECTION("Test 4"){

        auto middle_op = [](auto f){
            return
            -f(2) + 8*f(1) - 8*f(-1) + f(-2);
        };

        auto [min, max] = min_max_offset(middle_op);

        CHECK(min == -2);
        CHECK(max == 2);


    }

}














