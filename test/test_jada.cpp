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














