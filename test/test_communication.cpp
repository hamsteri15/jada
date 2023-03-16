#include "catch.hpp"


#include "include/jada.hpp"
#include "test.hpp"


using namespace jada;


TEST_CASE("Test box"){

    SECTION("Constructors"){
        Box<3> b({1,1,1}, {2,2,2});

        CHECK(b.get_extent() == extents<3>{1,1,1});

        CHECK(b.size() == 1 * 1 * 1); //NOTE!

        #ifdef DEBUG
            REQUIRE_THROWS(Box<3>({2,0,0}, {1, 1, 1}));
        #endif

    }
    SECTION("is_valid"){
        Box<3> b1({0,0,0}, {2,2,2});
        Box<3> b2({4,0,0}, {3,3,3});

        CHECK(b1.is_valid());
        CHECK(!b2.is_valid());

    }

    SECTION("Comparison"){

        Box<3> b1({1,1,1}, {2,2,2});
        Box<3> b2({1,1,1}, {2,2,2});
        Box<3> b3({1,1,1}, {5,2,2});

        CHECK( b1 == b2 );
        CHECK( b1 != b3 );

    }

    SECTION("translate"){
        
        Box<3> b1({1,1,1}, {2,2,2});
        b1.translate({1,2,0});
        CHECK(b1 == Box<3>({2, 3, 1}, {3, 4, 2}));

    }

    SECTION("expand"){
        
        Box<3> b1({1,1,1}, {2,2,2});
        b1.expand({0,0,2}, {0,1,0});


        CHECK(b1 == Box<3>({1,1,-1}, {2,3,2}));

        Box<3> b2({1,1,1}, {2,2,2});
        b2.expand(1);
        CHECK(b2 == Box<3>({0,0,0}, {3,3,3}));
    }

    

    SECTION("merge"){

        SECTION("Test 1"){
            Box<3> b1({0,0,0}, {2,2,2});
            Box<3> b2({2,2,2}, {3,3,3});

            CHECK(merge(b1, b2) == Box<3>({0,0,0}, {3,3,3}));

        }        
        SECTION("Test 2"){
            Box<3> b1({0,0,0}, {2,2,2});
            Box<3> b2({0,0,0}, {2,2,2});

            CHECK(merge(b1, b2) == Box<3>({0,0,0}, {2,2,2}));

        }        

    }
    
    SECTION("intersection"){
        
        SECTION("Test 1"){
            Box<3> b1({0,0,0}, {2,2,2});
            Box<3> b2({0,0,0}, {3,3,3});

            CHECK(intersection(b1, b2) == Box<3>({0,0,0}, {2,2,2}));

        }        
        SECTION("Test 2"){
            Box<3> b1({0,0,0}, {3,3,3});
            Box<3> b2({1,1,1}, {1,1,5});

            CHECK(intersection(b1, b2) == Box<3>({1,1,1}, {1,1,3}));
        }        

        SECTION("Test 3"){
            Box<3> b1({0,0,0}, {2,2,2});
            Box<3> b2({5,5,5}, {8,8,8});

            CHECK(intersection(b1, b2) == Box<3>({0,0,0}, {0,0,0}));
        }


    }

    SECTION("Contains"){
        Box<3> b1({0,0,0}, {3,3,3});


        CHECK(b1.contains({1,1,1}));
        CHECK(b1.contains({1,0,1}));

        Box<3> b2({0,0,0}, {1,1,1});

        CHECK(b1.contains(b2));
        CHECK(!b2.contains(b1));


    }

    

}


TEST_CASE("Test block"){

    SECTION("Constructors"){

        Block<3>(Box<3>({0,0,0}, {1,1,1}), 1);

    }
}



TEST_CASE("Test topology"){

    SECTION("Constructors"){


        SECTION("Test 1"){
            Box<3> domain({0,0,0}, {3,3,3});
            Box<3> b1({0,0,0}, {1,3,3});
            Box<3> b2({1,0,0}, {2,3,3});
            Box<3> b3({2,0,0}, {3,3,3});

            std::vector<BoxRankPair<3>> boxes1{
                BoxRankPair{.box=b1, .rank=0},
                BoxRankPair{.box=b2, .rank=1},
                BoxRankPair{.box=b3, .rank=1}
            };

            CHECK
            (
                Topology(domain, boxes1, {false, false, false}).is_valid()
            );

        }
        
        SECTION("Test 2"){
            Box<3> domain({0,0,0}, {3,3,3});
            Box<3> b1({0,0,0}, {2,3,3});
            Box<3> b2({1,0,0}, {2,3,3});
            Box<3> b3({2,0,0}, {3,3,3});

            std::vector<BoxRankPair<3>> boxes1{
                BoxRankPair{.box=b1, .rank=0},
                BoxRankPair{.box=b2, .rank=1},
                BoxRankPair{.box=b3, .rank=1}
            };

            CHECK
            (
                !Topology(domain, boxes1, {false, false, false}).is_valid()
            );

        }


    }


    SECTION("found"){

        
        Box<3> domain({0,0,0}, {3,3,3});
        Box<3> b1({0,0,0}, {1,3,3});
        Box<3> b2({1,0,0}, {2,3,3});
        Box<3> b3({2,0,0}, {3,3,3});

        std::vector<BoxRankPair<3>> boxes1{
            BoxRankPair{.box=b1, .rank=0},
            BoxRankPair{.box=b2, .rank=1},
            BoxRankPair{.box=b3, .rank=1}
        };
        
        Topology topo(domain, boxes1, {false, false, false});

        CHECK(topo.found(boxes1[1]));

        CHECK(!topo.found
        (
            BoxRankPair{.box=b1, .rank=433}
        ));

    }

    SECTION("get_neighbours"){
        Box<3> domain({0,0,0}, {3,3,3});
        Box<3> b1({0,0,0}, {1,3,3});
        Box<3> b2({1,0,0}, {2,3,3});
        Box<3> b3({2,0,0}, {3,3,3});


        std::vector<BoxRankPair<3>> boxes{
            BoxRankPair{.box=b1, .rank=0},
            BoxRankPair{.box=b2, .rank=1},
            BoxRankPair{.box=b3, .rank=1}
        };
        
        Topology topo(domain, boxes, {false, false, false});

        /*
        SECTION("Test1"){

            std::vector<BoxRankPair<3>> correct{
                boxes[0], boxes[2]
            };


            auto result = topo.get_neighbours(boxes[1]);

            CHECK(result == correct);


        }
        */


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
