TEST_CASE("make_boundary_subspan"){

        size_type ni = 3;
        size_type nj = 4;

        static constexpr int A = 0;
        static constexpr int X = 1;

        SECTION("dir = {1,0}"){

            std::vector<int> a(ni*nj, A);
            std::array<index_type, 2> dir = {1,0};
            auto parent = make_span(a, extents<2>{nj, ni});
            auto child = make_boundary_subspan(parent, dir);

            for_each(child, [](auto& v){v = X;});

            std::vector<int> correct =
            {
                A,A,A,
                A,A,A,
                A,A,A,
                X,X,X
            };

            CHECK(a == correct);

        }

        SECTION("dir = {-1,0}"){

            std::vector<int> a(ni*nj, A);
            std::array<index_type, 2> dir = {-1,0};
            auto parent = make_span(a, extents<2>{nj, ni});
            auto child = make_boundary_subspan(parent, dir);

            for_each(child, [](auto& v){v = X;});

            std::vector<int> correct =
            {
                X,X,X,
                A,A,A,
                A,A,A,
                A,A,A
            };

            CHECK(a == correct);

        }
        SECTION("dir = {1,1}"){

            std::vector<int> a(ni*nj, A);
            std::array<index_type, 2> dir = {1,1};
            auto parent = make_span(a, extents<2>{nj, ni});
            auto child = make_boundary_subspan(parent, dir);

            for_each(child, [](auto& v){v = X;});

            std::vector<int> correct =
            {
                A,A,A,
                A,A,A,
                A,A,A,
                A,A,X
            };

            CHECK(a == correct);

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