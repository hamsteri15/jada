#include "catch.hpp"


#include "include/jada.hpp"
#include "test.hpp"


using namespace jada;


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

TEST_CASE("Test data exchange"){



    SECTION("Periodic box"){
        auto domain = Box<2>{{0,0}, {4, 3}};
        auto boxes = std::vector<BoxRankPair<2>>{{domain, 0}};
        std::array<index_type, 2> bpad{}; bpad.fill(1);
        std::array<index_type, 2> epad{}; epad.fill(1);
        auto topo = Topology<2>{domain, boxes, {true, true}};


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


        send_receive(data, topo, bpad, epad, 0);

        CHECK(data == correct);


    }


    SECTION("Mpi periodic box"){

        //Each process creates an own topology and sends to self
        auto domain = Box<2>{{0,0}, {4, 3}};
        auto boxes = std::vector<BoxRankPair<2>>{{domain, mpi::get_world_rank()}};
        std::array<index_type, 2> bpad{}; bpad.fill(1);
        std::array<index_type, 2> epad{}; epad.fill(1);
        auto topo = Topology<2>{domain, boxes, {true, true}};

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


        mpi_send_receive(data, topo, bpad, epad, mpi::get_world_rank());

        CHECK(data == correct);

    }


}

TEST_CASE("reduce"){

    SECTION("Test 1"){

        int local = 1;

        auto global = mpi::sum_reduce(local, 0);

        if (mpi::get_world_rank() == 0){
            CHECK(global == mpi::world_size());
        }

    }

}


TEST_CASE("mpi functions"){


    SECTION("sum_reduce"){
        int data = 1;

        int root = 0;

        int recv = jada::mpi::sum_reduce(data, root, MPI_COMM_WORLD);
        mpi::wait(MPI_COMM_WORLD);

        if (mpi::get_world_rank() == root){
            CHECK(recv == data * mpi::world_size());
        }

    }


    SECTION("all_sum_reduce 1"){
        int data = 1;

        int recv = jada::mpi::all_sum_reduce(data);

        CHECK(recv == data * mpi::world_size());
    }

    SECTION("all_sum_reduce 1"){
        std::vector<int> data = {1};

        size_t recv = jada::mpi::all_sum_reduce(data.size());

        CHECK(recv == size_t(mpi::world_size()));
    }




}

TEST_CASE("all_gather"){




    SECTION("Single element test"){


        std::vector<int> local = {mpi::get_world_rank()};

        auto global = all_gather(local);

        std::vector<int> correct(size_t(mpi::world_size()));


        for (size_t i = 0; i < correct.size(); ++i){
            correct[i] = int(i);
        }

        CHECK(global == correct);

    }
    SECTION("Two element test"){


        std::vector<int> local(2, mpi::get_world_rank());

        auto global = all_gather(local);

        std::vector<int> correct(2 * size_t(mpi::world_size()));

        int j = 0;
        for (size_t i = 0; i < correct.size(); i+=2){
            correct[i] = j;
            correct[i+1] = j;
            j++;
        }



        CHECK(global == correct);

    }

    SECTION("Variable local element count test"){


        std::vector<int> local(size_t(mpi::get_world_rank()));

        for (auto& e : local){
            e = mpi::get_world_rank();
        }

        auto global = all_gather(local);

        std::vector<int> correct;
        for (int i = 0; i < mpi::world_size(); ++i){
            for (int j = 0; j < i; ++j){
                correct.push_back(i);
            }
        }

        CHECK(global == correct);

    }

}


struct DistributedTestData{

    static constexpr size_t nboxes = 3;
    static constexpr index_type ni = index_type(nboxes) * 1 + 1;
    static constexpr index_type nj = 3;
    static constexpr size_t unpadded_subspan_size = 1 * DistributedTestData::ni;
    static constexpr size_t padded_subspan_size = 1 * (DistributedTestData::ni + 1 + 2);


};

static inline auto make_test_topology(){

    size_t nprocs = size_t(mpi::world_size());

    if (nprocs > 3){
        throw std::logic_error("Only up to 3 mpi processes supported in tests.");
    }

    size_t nboxes = DistributedTestData::nboxes;

    index_type ni = DistributedTestData::ni;
    index_type nj = DistributedTestData::nj;

    Box<2> domain({0,0}, {nj, ni});

    std::vector<BoxRankPair<2>> boxes(nboxes);

    //This distributes the boxes in round robin fashion
    int rank = 0;
    for (size_t i = 0; i < nboxes; ++i){

        auto& pair = boxes[i];
        pair.box.m_begin = std::array<index_type, 2>{{index_type(i), 0}};
        pair.box.m_end = std::array<index_type, 2>{{index_type(i+1), ni}};
        //int rank = 0;
        pair.rank = rank;
        rank++;
        if (rank == int(nprocs)){
            rank = 0;
        }
    }

    return Topology<2>(domain, boxes, {false, false});


}


static inline auto make_test_array(bool add_padding){


    std::array<index_type, 2> bpad{};
    std::array<index_type, 2> epad{};

    if (add_padding){
        bpad[1] = 1;
        epad[1] = 2;
    }

    auto topo = make_test_topology();

    int myrank = mpi::get_world_rank();


    DistributedArray<2, int> arr(myrank, topo, bpad, epad);


    for (auto& v : arr.local_data()){
        std::fill(v.begin(), v.end(), myrank + 1);
    }

    return arr;

}



TEST_CASE("Test DistributedArray")
{



    SECTION("local_element_count/local_capacity"){
        auto arr = make_test_array(true);

        CHECK(local_element_count(arr) == arr.local_subdomain_count() * DistributedTestData::unpadded_subspan_size);
        CHECK(local_capacity(arr) == arr.local_subdomain_count() * DistributedTestData::padded_subspan_size);

    }



    SECTION("make_subspans"){

        SECTION("unpadded"){
            auto arr = make_test_array(false);

            for (auto s : make_subspans(arr)){

                for (index_type i = 0; i < 4; ++i){
                    CHECK(s(0, i) == mpi::get_world_rank() + 1);
                }
            }
        }

        SECTION("padded"){
            auto arr = make_test_array(true);

            for (auto s : make_subspans(arr)){

                for (index_type i = 0; i < 4; ++i){
                    CHECK(s(0, i) == mpi::get_world_rank() + 1);
                }
            }
        }
        
    }


    

    SECTION("serialize_local"){

        SECTION("without padding"){
            auto arr = make_test_array(false);
            auto serial = serialize_local(arr);

            size_t correct_size = arr.local_subdomain_count() * DistributedTestData::unpadded_subspan_size;
            std::vector<int> correct(correct_size, mpi::get_world_rank() + 1);

            CHECK(serial == correct);
        }
        SECTION("with padding"){
            auto arr = make_test_array(true);
            auto serial = serialize_local(arr);

            size_t correct_size = arr.local_subdomain_count() * DistributedTestData::unpadded_subspan_size;
            std::vector<int> correct(correct_size, mpi::get_world_rank() + 1);

            CHECK(serial == correct);
        }


    }
    

    SECTION("distribute"){
        
        index_type ny = 3;
        index_type nx = 4;

        Box<2> domain({0,0}, {ny, nx});


        auto topo = decompose(domain, mpi::world_size(), {false, false});

        std::vector<int> data(flat_size(domain.get_extent()));


        auto subspans = make_subspans(data, topo, mpi::get_world_rank());

        for (auto span : subspans){

            for_each(span, [](auto& e){e = mpi::get_world_rank() + 1;});

        }

        std::array<index_type, 2> bpad{1,1};
        std::array<index_type, 2> epad{2,3};
        auto arr = distribute(data, topo, mpi::get_world_rank(), bpad, epad);

        
        if (mpi::world_size() == 1){

            std::vector<int> correct = {1, 1, 1, 1,
                                        1, 1, 1, 1,
                                        1, 1, 1, 1};

            CHECK(serialize_local(arr) == correct);

        }
        if (mpi::world_size() == 2){

            if (mpi::get_world_rank() == 0){

                std::vector<int> correct = {1, 1,
                                            1, 1,
                                            1, 1};

                CHECK(serialize_local(arr) == correct);
            }

            if (mpi::get_world_rank() == 1){

                std::vector<int> correct = {2, 2,
                                            2, 2,
                                            2, 2};
            }

        }

        if (mpi::world_size() == 3){

            if (mpi::get_world_rank() == 0){

                std::vector<int> correct = {1,
                                            1,
                                            1};

                CHECK(serialize_local(arr) == correct);
            }
            if (mpi::get_world_rank() == 1){

                std::vector<int> correct = {2,
                                            2,
                                            2};

                CHECK(serialize_local(arr) == correct);
            }

            if (mpi::get_world_rank() == 2){

                std::vector<int> correct = {3, 3,
                                            3, 3,
                                            3, 3};

                CHECK(serialize_local(arr) == correct);
            }

        }
        

    }
    
    /*

    SECTION("Unpadded distribute/reduce"){
        auto bpad = std::array<index_type ,2>{};
        auto epad = std::array<index_type ,2>{};
        Topology topo(domain, boxes, {false, false});

        std::vector<int> global_data(flat_size(domain.get_extent()));
        std::iota(global_data.begin(), global_data.end(), 0);


        auto darray = distribute(global_data, topo, mpi::get_world_rank(), bpad, epad);

        CHECK
        (
            global_data == reduce(darray)
        );

    }


    SECTION("Padded distribute/reduce"){
        auto bpad = std::array<index_type ,2>{}; bpad.fill(1);
        auto epad = std::array<index_type ,2>{}; epad.fill(1);
        Topology topo(domain, boxes, {false, false});

        std::vector<int> global_data(flat_size(domain.get_extent()));
        std::iota(global_data.begin(), global_data.end(), 0);


        auto darray = distribute(global_data, topo, mpi::get_world_rank(), bpad, epad);

        CHECK
        (
            global_data == reduce(darray)
        );

    }
    */

    /*
    SECTION("distribute"){


        auto make_test_array(true);

        std::vector<int> global = {1,  2,  3,  4,
                                   6,  7,  8,  9,
                                   10, 11, 12, 13};

        auto dist = distribute(global, topo, mpi::get_world_rank(), bpad, epad);


        if (mpi::get_world_rank() == 0){

            auto spans = make_subspans(dist);
            auto s1 = spans[0];

            CHECK(s1(0, 0) == 1);
            CHECK(s1(0, 1) == 2);
            CHECK(s1(0, 2) == 3);
            CHECK(s1(0, 3) == 4);

            auto s2 = spans[1];

            CHECK(s2(0, 0) == 6);
            CHECK(s2(0, 1) == 7);
            CHECK(s2(0, 2) == 8);
            CHECK(s2(0, 3) == 9);


        }
    }
    */

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
