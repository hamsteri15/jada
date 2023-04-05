#include "catch.hpp"


#include "include/jada.hpp"
#include "test.hpp"


using namespace jada;

TEST_CASE("Test box") {

    SECTION("Constructors") {
        Box<3> b({1, 1, 1}, {2, 2, 2});

        CHECK(b.get_extent() == extents<3>{1, 1, 1});

        CHECK(b.size() == 1 * 1 * 1); // NOTE!

#ifdef DEBUG
        REQUIRE_THROWS(Box<3>({2, 0, 0}, {1, 1, 1}));
#endif
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
    }

    SECTION("distance") {
        Box<3> b1({0, 0, 0}, {3, 3, 3});
        Box<3> b2({2, 2, 2}, {3, 3, 3});
        Box<3> b3({1, 2, 3}, {3, 3, 3});

        CHECK(distance(b1, b2) == std::array<index_type, 3>{2, 2, 2});
        CHECK(distance(b1, b3) == std::array<index_type, 3>{1, 2, 3});
    }
}

TEST_CASE("Test block"){

    SECTION("Constructors"){

        Block<3>(Box<3>({0,0,0}, {1,1,1}), 1);

    }
}

TEST_CASE("Test topology") {

    auto test_dec1d = [](){
        Box<1> domain({0}, {10});
        Box<1> b0({0}, {3});
        Box<1> b1({3}, {6});
        Box<1> b2({6}, {10});

        std::vector<BoxRankPair<1>> boxes{BoxRankPair{.box = b0, .rank = 0},
                                          BoxRankPair{.box = b1, .rank = 1},
                                          BoxRankPair{.box = b2, .rank = 2}};

        return std::make_pair(domain, boxes);
    };

    auto test_dec2d = []() {
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
    };



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

            auto bpad = std::array<index_type ,3>{};
            auto epad = std::array<index_type ,3>{};

            CHECK(Topology(domain, boxes1, {false, false, false}, bpad, epad).is_valid());
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

            auto bpad = std::array<index_type ,3>{};
            auto epad = std::array<index_type ,3>{};

            CHECK(!Topology(domain, boxes1, {false, false, false}, bpad, epad).is_valid());
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

        auto bpad = std::array<index_type ,3>{};
        auto epad = std::array<index_type ,3>{};
        Topology topo(domain, boxes1, {false, false, false}, bpad, epad);

        CHECK(topo.found(boxes1[1]));

        CHECK(!topo.found(BoxRankPair{.box = b1, .rank = 433}));
    }

    SECTION("get_neighbours()") {

        using namespace Catch::Matchers;

        SECTION("non-periodic") {

            auto [domain, boxes] = test_dec2d();

            auto bpad = std::array<index_type ,2>{}; bpad.fill(1);
            auto epad = std::array<index_type ,2>{}; epad.fill(1);

            Topology topo(domain, boxes, {false, false}, bpad, epad);

            REQUIRE_THAT(
                topo.get_neighbours(boxes[0]),
                UnorderedEquals(std::vector{boxes[1], boxes[4], boxes[5]}));

            REQUIRE_THAT(topo.get_neighbours(boxes[1]),
                         UnorderedEquals(std::vector{
                             boxes[0], boxes[2], boxes[3], boxes[5]}));

            REQUIRE_THAT(
                topo.get_neighbours(boxes[2]),
                UnorderedEquals(std::vector{boxes[1], boxes[3], boxes[5]}));

            REQUIRE_THAT(topo.get_neighbours(boxes[3]),
                         UnorderedEquals(std::vector{
                             boxes[1], boxes[2], boxes[4], boxes[5]}));

            REQUIRE_THAT(
                topo.get_neighbours(boxes[4]),
                UnorderedEquals(std::vector{boxes[0], boxes[3], boxes[5]}));

            REQUIRE_THAT(
                topo.get_neighbours(boxes[5]),
                UnorderedEquals(std::vector{
                    boxes[0], boxes[1], boxes[2], boxes[3], boxes[4]}));
        }

        SECTION("periodic-x") {

            auto [domain, boxes] = test_dec2d();
            auto bpad = std::array<index_type ,2>{}; bpad.fill(1);
            auto epad = std::array<index_type ,2>{}; epad.fill(1);
            Topology topo(domain, boxes, {true, false}, bpad, epad);

            REQUIRE_THAT(
                topo.get_neighbours(boxes[0]),
                UnorderedEquals(std::vector{
                    boxes[1], boxes[2], boxes[3], boxes[4], boxes[5]}));
            REQUIRE_THAT(
                topo.get_neighbours(boxes[5]),
                UnorderedEquals(std::vector{
                    boxes[0], boxes[1], boxes[2], boxes[3], boxes[4]}));
        }

        SECTION("periodic-y") {

            auto [domain, boxes] = test_dec2d();
            auto bpad = std::array<index_type ,2>{}; bpad.fill(1);
            auto epad = std::array<index_type ,2>{}; epad.fill(1);
            Topology topo(domain, boxes, {false, true}, bpad, epad);

            REQUIRE_THAT(topo.get_neighbours(boxes[0]),
                         UnorderedEquals(std::vector{
                             boxes[0], boxes[1], boxes[4], boxes[5]}));
            REQUIRE_THAT(
                topo.get_neighbours(boxes[1]),
                UnorderedEquals(std::vector{
                    boxes[0], boxes[2], boxes[3], boxes[4], boxes[5]}));
        }

        SECTION("periodic-both") {

            auto [domain, boxes] = test_dec2d();

            auto bpad = std::array<index_type ,2>{}; bpad.fill(1);
            auto epad = std::array<index_type ,2>{}; epad.fill(1);
            Topology topo(domain, boxes, {true, true}, bpad, epad);

            REQUIRE_THAT(topo.get_neighbours(boxes[0]),
                         UnorderedEquals(std::vector{boxes[0],
                                                     boxes[1],
                                                     boxes[2],
                                                     boxes[3],
                                                     boxes[4],
                                                     boxes[5]}));
            REQUIRE_THAT(
                topo.get_neighbours(boxes[5]),
                UnorderedEquals(std::vector{
                    boxes[0], boxes[1], boxes[2], boxes[3], boxes[4]}));
        }

        SECTION("Single periodic box") {

            auto domain = Box<3>{{0, 0, 0}, {1, 1, 1}};

            std::vector<BoxRankPair<3>> boxes{
                BoxRankPair{.box = Box<3>{{0, 0, 0}, {1, 1, 1}}, .rank = 0}};

            auto bpad = std::array<index_type ,3>{}; bpad.fill(1);
            auto epad = std::array<index_type ,3>{}; epad.fill(1);
            Topology topo(domain, boxes, {true, true, true}, bpad, epad);

            auto result = topo.get_neighbours(boxes[0]);
            CHECK(result == std::vector{boxes[0]});

            CHECK(topo.get_intersections(boxes[0], boxes[0]).size() ==
                  Neighbours<3, ConnectivityType::Box>::count());
        }
    }

    SECTION("get_intersections") {

        SECTION("one dimensional domain"){

            auto domain = Box<1>{{0}, {10}};

            auto boxes = std::vector<BoxRankPair<1>>{
                BoxRankPair<1>{.box = Box<1>{{0}, {2}}, .rank = 0},
                BoxRankPair<1>{.box = Box<1>{{2}, {5}}, .rank = 1},
                BoxRankPair<1>{.box = Box<1>{{5}, {7}}, .rank = 2},
                BoxRankPair<1>{.box = Box<1>{{7}, {10}}, .rank = 3}};

            auto bpad = std::array<index_type ,1>{}; bpad.fill(1);
            auto epad = std::array<index_type ,1>{}; epad.fill(1);
            Topology topo(domain, boxes, {true}, bpad, epad);

            CHECK(topo.get_intersections(boxes[0], boxes[1]) ==
                  std::vector{Box<1>{{2}, {3}}});
            CHECK(topo.get_intersections(boxes[1], boxes[0]) ==
                  std::vector{Box<1>{{1}, {2}}});
            CHECK(topo.get_intersections(boxes[3], boxes[0]) ==
                  std::vector{Box<1>{{10}, {11}}});
            CHECK(topo.get_intersections(boxes[0], boxes[3]) ==
                  std::vector{Box<1>{{-1}, {0}}});
        }


        SECTION("two dimensional domain") {

            using namespace Catch::Matchers;

            auto [domain, boxes] = test_dec2d();

            auto bpad = std::array<index_type ,2>{}; bpad.fill(1);
            auto epad = std::array<index_type ,2>{}; epad.fill(1);
            Topology topo(domain, boxes, {true, false}, bpad, epad);

            CHECK(topo.get_intersections(boxes[0], boxes[1]) ==
                  std::vector{Box<2>{{4, 5}, {5, 7}}});

            CHECK(topo.get_intersections(boxes[0], boxes[5]) ==
                  std::vector{Box<2>{{4, 3}, {5, 5}}});

            REQUIRE_THAT(topo.get_intersections(boxes[0], boxes[4]),
                         UnorderedEquals(std::vector{Box<2>{{4, 0}, {5, 3}},
                                                     Box<2>{{-1, 0}, {0, 3}}}));
        }
    }

    SECTION("global_to_local") {

        SECTION("1D tests") {
            auto [domain, boxes] = test_dec1d();

            auto bpad = std::array<index_type ,1>{}; bpad.fill(1);
            auto epad = std::array<index_type ,1>{}; epad.fill(1);
            Topology topo(domain, boxes, {true}, bpad, epad);

            CHECK(topo.global_to_local(boxes[0], {0}) ==
                  std::array<index_type, 1>{1});

            CHECK(topo.global_to_local(boxes[1], {3}) ==
                  std::array<index_type, 1>{1});
        }
    }


    SECTION("local_to_global"){
        SECTION("1D tests") {
            auto [domain, boxes] = test_dec1d();

            auto bpad = std::array<index_type ,1>{}; bpad.fill(1);
            auto epad = std::array<index_type ,1>{}; epad.fill(1);
            Topology topo(domain, boxes, {true}, bpad, epad);

            CHECK(topo.local_to_global(boxes[0], {1}) ==
                  std::array<index_type, 1>{1});

            CHECK(topo.local_to_global(boxes[1], {1}) ==
                  std::array<index_type, 1>{4});
        }

    }

    SECTION("get_locations"){

        using namespace Catch::Matchers;


        SECTION("1D tests"){
            auto [domain, boxes] = test_dec1d();
            auto bpad = std::array<index_type ,1>{}; bpad.fill(1);
            auto epad = std::array<index_type ,1>{}; epad.fill(1);
            Topology topo(domain, boxes, {true}, bpad, epad);

            SECTION("Test 1"){

                auto [sender_begins, receiver_begins, extents] =
                    topo.get_locations(boxes[0], boxes[1]);

                CHECK(sender_begins ==
                      std::vector<std::array<index_type, 1>>{{3}});

                CHECK(receiver_begins ==
                      std::vector<std::array<index_type, 1>>{{0}});

                CHECK(extents ==
                      std::vector<std::array<size_type, 1>>{{1}});
            }

            SECTION("Test 2"){

                auto [sender_begins, receiver_begins, extents] =
                    topo.get_locations(boxes[1], boxes[0]);

                CHECK(sender_begins ==
                      std::vector<std::array<index_type, 1>>{{1}});

                CHECK(receiver_begins ==
                      std::vector<std::array<index_type, 1>>{{4}});

                CHECK(extents ==
                      std::vector<std::array<size_type, 1>>{{1}});
            }

            SECTION("Test 3 periodicity"){

                auto [sender_begins, receiver_begins, extents] =
                    topo.get_locations(boxes[0], boxes[2]);

                CHECK(sender_begins ==
                      std::vector<std::array<index_type, 1>>{{1}});

                CHECK(receiver_begins ==
                      std::vector<std::array<index_type, 1>>{{5}});

                CHECK(extents ==
                      std::vector<std::array<size_type, 1>>{{1}});
            }

        }


        SECTION("2D tests"){

            SECTION("Test 1") {
                auto [domain, boxes] = test_dec2d();
                auto bpad = std::array<index_type ,2>{}; bpad.fill(1);
                auto epad = std::array<index_type ,2>{}; epad.fill(1);
                Topology topo(domain, boxes, {false, false}, bpad, epad);

                auto [sender_begins, receiver_begins, extents] =
                    topo.get_locations(boxes[0], boxes[1]);

                REQUIRE_THAT(
                    extents,
                    UnorderedEquals(std::vector<std::array<size_type, 2>>{
                        {1, 3}}));


                REQUIRE_THAT(
                    sender_begins,
                    UnorderedEquals(std::vector<std::array<index_type, 2>>{
                        {4, 5}}));


                REQUIRE_THAT(
                    receiver_begins,
                    UnorderedEquals(std::vector<std::array<index_type, 2>>{
                        {0,0}}));


            }
        }


    }

   

}


auto make_slice(std::vector<int> global, auto topo, auto box){

    std::vector<int> ret(flat_size(box.box.get_extent()));

    auto big_span = make_span(global, topo.get_domain().get_extent());
    auto smaller = make_subspan(big_span, box.box.m_begin, box.box.m_end);

    auto out = make_span(ret, box.box.get_extent());

    transform(smaller, out, [](auto val){return val;});
    return ret;

}

auto add_padding(std::vector<int> vec, auto extent, auto pbegin, auto pend){

    auto temp = extent_to_array(extent);

    for (size_t i = 0; i < rank(pbegin); ++i){
        temp[i] += size_t(pbegin[i] + pend[i]);
    }

    auto new_extent = make_extent(temp);
    
    std::vector<int> ret(flat_size(new_extent), 1);


    auto i_span = make_span(vec, extent);

    auto sbegin = pbegin;
    auto send = get_end(pbegin, extent_to_array(extent));

    auto o_span = make_subspan(make_span(ret, new_extent), sbegin, send);


    transform(i_span, o_span, [](auto val){return val;});


    return ret;


}


auto decompose(std::vector<int> global, auto topology){

    runtime_assert(global.size() == flat_size(topology.get_domain().get_extent()), "Can not decompose.");

    std::vector<std::vector<int>> datas;


    for (auto box : topology.get_boxes()){

        std::vector<int> local(flat_size(box.box.get_extent()));

        auto local_span = make_span(local, box.box.get_extent());
        auto global_span = make_span(global, topology.get_domain().get_extent());
        auto global_subspan = make_subspan(global_span, box.box.m_begin, box.box.m_end);
        
        transform_indexed
        (
            global_subspan,
            local_span,
            [](auto val) {return val;}
        );

        datas.push_back(local);

    }

}

/*
auto apply_kernel(auto data, auto box, auto kernel){

    //TODO: This function requires all kinds of extents and stuffs

    dectlype(data) ret(data.size(), 0);

    return ret;

}
*/

TEST_CASE("Test data exchange"){

    auto test_dec2d = []() {
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
    };
    
    SECTION("Periodic box"){
        auto domain = Box<2>{{0,0}, {4, 3}};
        auto boxes = std::vector<BoxRankPair<2>>{{domain, 0}};
        auto topo = Topology<2>{domain, boxes, {true, true}, {1,1}, {1,1}};

        std::vector<int> data = 
        {
            0,  0,  0,  0,  0,
            0,  1,  2,  3,  0,
            0,  4,  5,  6,  0,
            0,  7,  8,  9,  0,
            0,  10, 11, 12, 0,
            0,  0,  0,  0,  0
        };
        
        std::vector<int> correct = 
        {
            12, 10, 11, 12, 10,
            3,  1,  2,  3,  1,
            6,  4,  5,  6,  4,
            9,  7,  8,  9,  7,
            12, 10, 11, 12, 10,
            3,  1,  2,  3,  1
        };


        send_receive(data, topo, 0);
        

        CHECK(data == correct);

    }
    
    SECTION("Mpi periodic box"){
        
        //Each process creates an own topology and sends to self
        auto domain = Box<2>{{0,0}, {4, 3}};
        auto boxes = std::vector<BoxRankPair<2>>{{domain, mpi::get_world_rank()}};
        auto topo = Topology<2>{domain, boxes, {true, true}, {1,1}, {1,1}};

        std::vector<int> data = 
        {
            0,  0,  0,  0,  0,
            0,  1,  2,  3,  0,
            0,  4,  5,  6,  0,
            0,  7,  8,  9,  0,
            0,  10, 11, 12, 0,
            0,  0,  0,  0,  0
        };
        
        std::vector<int> correct = 
        {
            12, 10, 11, 12, 10,
            3,  1,  2,  3,  1,
            6,  4,  5,  6,  4,
            9,  7,  8,  9,  7,
            12, 10, 11, 12, 10,
            3,  1,  2,  3,  1
        };


        mpi_send_receive(data, topo, mpi::get_world_rank());
        
        CHECK(data == correct);
    
    }
    
    SECTION("Distributed transform"){

        auto domain = Box<2>{{0,0}, {10, 11}};
        int n_ranks = 3;
        auto periods = std::array<bool, 2>{true, true};
        auto padding = std::array<index_type, 2>{1,1};

        auto topo = decompose(domain, n_ranks, periods, padding, padding);


        auto global = std::vector<int>(flat_size(domain.get_extent()));
        std::iota(global.begin(), global.end(), 0);

        auto kernel = [](auto f){
            return f(0,0) + f(-1,1) + f(0, 1) + f(-1,-1) + f(1,0) + f(1,1);
        };

        //This mocks mpi processes
        for (int i = 0; i < n_ranks; ++i){

            int rank = i; 

            for (auto box : topo.get_boxes(rank)){

                auto local = make_slice(global, topo, box);
                auto padded_local = add_padding(local, box.get_extent(), padding, padding);
                auto padded_local_extent = topo.get_padded_extent(box);

                




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
