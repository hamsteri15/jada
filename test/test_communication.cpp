#include "catch.hpp"


#include "include/jada.hpp"
#include "test.hpp"


using namespace jada;




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
        pair.box.begin = std::array<index_type, 2>{{index_type(i), 0}};
        pair.box.end = std::array<index_type, 2>{{index_type(i+1), ni}};
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


    for (auto& v : arr.get_local_data()){
        std::fill(v.begin(), v.end(), myrank + 1);
    }

    return arr;

}



TEST_CASE("Test DistributedArray")
{



    SECTION("local_element_count/local_capacity"){
        auto arr = make_test_array(true);

        CHECK(local_element_count(arr) == arr.get_local_subdomain_count() * DistributedTestData::unpadded_subspan_size);
        CHECK(local_capacity(arr) == arr.get_local_subdomain_count() * DistributedTestData::padded_subspan_size);

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

            size_t correct_size = arr.get_local_subdomain_count() * DistributedTestData::unpadded_subspan_size;
            std::vector<int> correct(correct_size, mpi::get_world_rank() + 1);

            CHECK(serial == correct);
        }
        SECTION("with padding"){
            auto arr = make_test_array(true);
            auto serial = serialize_local(arr);

            size_t correct_size = arr.get_local_subdomain_count() * DistributedTestData::unpadded_subspan_size;
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

        for (auto span : make_subspans(data, topo, mpi::get_world_rank())){

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

    SECTION("to_vector"){

        index_type ny = 3;
        index_type nx = 4;

        std::array<index_type, 2> bpad{1,0};
        std::array<index_type, 2> epad{4,2};

        Box<2> domain({0,0}, {ny, nx});
        auto topo = decompose(domain, mpi::world_size(), {false, false});

        std::vector<int> data(flat_size(domain.get_extent()));

        std::iota(data.begin(), data.end(), 1);

        auto arr = distribute(data, topo, mpi::get_world_rank(), bpad, epad);


        CHECK(to_vector(arr) == data);

    }

    /*
    SECTION("reduce"){

        index_type ny = 3;
        index_type nx = 4;

        std::array<index_type, 2> bpad{1,0};
        std::array<index_type, 2> epad{4,2};

        Box<2> domain({0,0}, {ny, nx});
        auto topo = decompose(domain, mpi::world_size(), {false, false});

        std::vector<int> data(flat_size(domain.get_extent()));

        std::iota(data.begin(), data.end(), 1);

        auto arr = distribute(data, topo, mpi::get_world_rank(), bpad, epad);
        auto reduced = reduce(arr);

        std::cout << std::endl << reduced.topology() << std::endl;

        CAPTURE(mpi::get_world_rank());

        CHECK(serialize_local(reduced) == data);

    }
    */


    SECTION("algorithms"){

        const index_type nj = 2;
        const index_type ni = 3;
        const Box<2> domain({0,0}, {nj, ni});
        const auto topo = decompose(domain, mpi::world_size(), {false, false});



        SECTION("for_each"){
            std::array<index_type, 2> bpad{1,0};
            std::array<index_type, 2> epad{4,2};
            const std::vector<int> org_data(size_t(nj * ni), 0);

            auto arr = distribute(org_data, topo, mpi::get_world_rank(), bpad, epad);
            size_t n = global_element_count(arr);

            SECTION("serial"){
                auto op = [](auto& e){e = 43;};

                for_each( arr, op);

                CHECK
                (
                    to_vector(arr) == std::vector<int>(n, 43)
                );
            }
            SECTION("parallel"){
                auto op = [](auto& e){e = 11;};

                for_each(std::execution::par_unseq, arr, op);

                CHECK
                (
                    to_vector(arr) == std::vector<int>(n, 11)
                );
            }

        }


        SECTION("for_each_indexed"){

            std::array<index_type, 2> bpad{1,0};
            std::array<index_type, 2> epad{4,2};

            auto op = [](auto idx, int& e){
                auto j = std::get<0>(idx);
                auto i = std::get<1>(idx);
                e = int(i + j);
            };
            const std::vector<int> org_data(size_t(nj * ni), 0);

            SECTION("serial"){
                auto arr = distribute(org_data, topo, mpi::get_world_rank(), bpad, epad);
                for_each_indexed(arr, op);

                std::vector<int> correct =
                {
                    0,1,2,
                    1,2,3
                };
                CHECK(to_vector(arr) == correct);

            }

            SECTION("parallel"){
                auto arr = distribute(org_data, topo, mpi::get_world_rank(), bpad, epad);

                for_each_indexed(std::execution::par_unseq, arr, op);

                std::vector<int> correct =
                {
                    0,1,2,
                    1,2,3
                };

                CHECK(to_vector(arr) == correct);
            }


        }

        SECTION("transform"){

            std::array<index_type, 2> bpad{1,0};
            std::array<index_type, 2> epad{4,2};
            const std::vector<int> org_data(size_t(nj * ni), 1);
            auto op = [](int e){
                    return e + 1;
            };
            const std::vector<int> correct =
            {
                2,2,2,
                2,2,2
            };

            SECTION("serial"){

                const auto arr_a = distribute(org_data, topo, mpi::get_world_rank(), bpad, epad);
                auto arr_b = distribute(org_data, topo, mpi::get_world_rank(), bpad, epad);

                transform(arr_a, arr_b, op);

                CHECK(to_vector(arr_b) == correct);

            }
            SECTION("parallel"){

                const auto arr_a = distribute(org_data, topo, mpi::get_world_rank(), bpad, epad);
                auto arr_b = distribute(org_data, topo, mpi::get_world_rank(), bpad, epad);

                transform(std::execution::par_unseq, arr_a, arr_b, op);


                CHECK(to_vector(arr_b) == correct);

            }

        }


        SECTION("transform_indexed"){

            std::array<index_type, 2> bpad{1,0};
            std::array<index_type, 2> epad{4,2};
            const std::vector<int> org_data(size_t(nj * ni), 1);

            auto op = [](auto idx, int e){
                    auto j = std::get<0>(idx);
                    auto i = std::get<1>(idx);
                    return e + int(i + j);
            };

            const std::vector<int> correct =
                {
                    1,2,3,
                    2,3,4
                };

            SECTION("serial"){

                const auto arr_a = distribute(org_data, topo, mpi::get_world_rank(), bpad, epad);
                auto arr_b = distribute(org_data, topo, mpi::get_world_rank(), bpad, epad);

                transform_indexed(arr_a, arr_b, op);

                CHECK(to_vector(arr_b) == correct);

            }

            SECTION("parallel"){

                const auto arr_a = distribute(org_data, topo, mpi::get_world_rank(), bpad, epad);
                auto arr_b = distribute(org_data, topo, mpi::get_world_rank(), bpad, epad);

                transform_indexed(std::execution::par_unseq, arr_a, arr_b, op);

                CHECK(to_vector(arr_b) == correct);

            }

        }

        SECTION("window_transform"){

            std::array<index_type, 2> bpad{1,1};
            std::array<index_type, 2> epad{1,1};

            const std::vector<int> a(size_t(ni*nj), 0);
            const std::vector<int> b(size_t(ni*nj), 0);

            const std::vector<int> correct =
            {
                3,3,3,
                3,3,3
            };

            auto op = [](auto f) {
                return f(-1,-1) + f(0, 1) + f(1,1);
            };

            SECTION("serial"){

                auto arr_a = distribute(a, topo, mpi::get_world_rank(), bpad, epad);
                auto arr_b = distribute(b, topo, mpi::get_world_rank(), bpad, epad);

                for (auto& data : arr_a.get_local_data()){
                    std::fill(data.begin(), data.end(), 1);
                }
                for (auto& data : arr_b.get_local_data()){
                    std::fill(data.begin(), data.end(), -1);
                }

                window_transform(arr_a, arr_b, op);

                CHECK(to_vector(arr_b) == correct);

            }

            SECTION("parallel"){

                auto arr_a = distribute(a, topo, mpi::get_world_rank(), bpad, epad);
                auto arr_b = distribute(b, topo, mpi::get_world_rank(), bpad, epad);

                for (auto& data : arr_a.get_local_data()){
                    std::fill(data.begin(), data.end(), 1);
                }
                for (auto& data : arr_b.get_local_data()){
                    std::fill(data.begin(), data.end(), -1);
                }

                window_transform(std::execution::par_unseq, arr_a, arr_b, op);

                CHECK(to_vector(arr_b) == correct);

            }


        }


        SECTION("tile_transform"){

            std::array<index_type, 2> bpad{1,1};
            std::array<index_type, 2> epad{1,1};

            const std::vector<int> a(size_t(ni*nj), 0);
            const std::vector<int> b(size_t(ni*nj), 0);



            auto op = [](auto f) {
                return f(-1) + f(1);
            };

            const std::vector<int> correct =
            {
                2,2,2,
                2,2,2
            };

            SECTION("serial"){

                auto arr_a = distribute(a, topo, mpi::get_world_rank(), bpad, epad);
                auto arr_b = distribute(b, topo, mpi::get_world_rank(), bpad, epad);

                for (auto& data : arr_a.get_local_data()){
                    std::fill(data.begin(), data.end(), 1);
                }
                for (auto& data : arr_b.get_local_data()){
                    std::fill(data.begin(), data.end(), -1);
                }

                tile_transform<0>( arr_a, arr_b, op);

                CHECK(to_vector(arr_b) == correct);

                for (auto& data : arr_b.get_local_data()){
                    std::fill(data.begin(), data.end(), -3);
                }

                tile_transform<1>( arr_a, arr_b, op);
                CHECK(to_vector(arr_b) == correct);

            }
            SECTION("parallel"){

                auto arr_a = distribute(a, topo, mpi::get_world_rank(), bpad, epad);
                auto arr_b = distribute(b, topo, mpi::get_world_rank(), bpad, epad);

                for (auto& data : arr_a.get_local_data()){
                    std::fill(data.begin(), data.end(), 1);
                }
                for (auto& data : arr_b.get_local_data()){
                    std::fill(data.begin(), data.end(), -1);
                }


                tile_transform<0>(std::execution::par_unseq, arr_a, arr_b, op);

                CHECK(to_vector(arr_b) == correct);

                for (auto& data : arr_b.get_local_data()){
                    std::fill(data.begin(), data.end(), -3);
                }

                tile_transform<1>(std::execution::par_unseq, arr_a, arr_b, op);
                CHECK(to_vector(arr_b) == correct);

            }
        }
    }

    

    SECTION("boundary_algorithms"){

        index_type ni = 4;
        index_type nj = 3;

        std::array<index_type, 2> bpad{1, 1};
        std::array<index_type, 2> epad{1, 1};

        const Box<2> domain({0,0}, {nj, ni});
        const auto topo = decompose(domain, mpi::world_size(), {false, false});

        SECTION("for_each_boundary"){

            const std::vector<int> a(size_t(ni*nj), 0);

            auto op = [](auto& f) {
                f = 2;
            };

            auto arr = distribute(a, topo, mpi::get_world_rank(), bpad, epad);

            SECTION("dir {0, 1}"){

                for (auto& data : arr.get_local_data()){
                    std::fill(data.begin(), data.end(), 1);
                }
                for_each_boundary(std::execution::par_unseq, arr, {0, 1}, op );

                std::vector<int> correct =
                {
                    1, 1, 1, 2,
                    1, 1, 1, 2,
                    1, 1, 1, 2
                };

                CHECK(to_vector(arr) == correct);
            }

            SECTION("dir {-1, 1}"){

                for (auto& data : arr.get_local_data()){
                    std::fill(data.begin(), data.end(), 1);
                }
                for_each_boundary(std::execution::par_unseq, arr, {-1, 1}, op );

                std::vector<int> correct =
                {
                    1, 1, 1, 2,
                    1, 1, 1, 1,
                    1, 1, 1, 1
                };

                CHECK(to_vector(arr) == correct);
            }
        
        }

        SECTION("for_each_indexed_boundary"){

            const std::vector<int> a(size_t(ni*nj), 0);

            auto op_j = [](auto idx, int& e){
                auto j = std::get<0>(idx);
                e = j;
            };

            auto op_i = [](auto idx, int& e){
                auto i = std::get<1>(idx);
                e = i;
            };

            auto arr = distribute(a, topo, mpi::get_world_rank(), bpad, epad);

            SECTION("dir {0, 1}"){

                for (auto& data : arr.get_local_data()){
                    std::fill(data.begin(), data.end(), 1);
                }
                for_each_indexed_boundary(std::execution::par_unseq, arr, {0, 1}, op_j );

                std::vector<int> correct =
                {
                    1, 1, 1, 0,
                    1, 1, 1, 1,
                    1, 1, 1, 2
                };

                CHECK(to_vector(arr) == correct);
            }

            SECTION("dir {-1, 0}"){

                for (auto& data : arr.get_local_data()){
                    std::fill(data.begin(), data.end(), 1);
                }
                for_each_indexed_boundary(std::execution::par_unseq, arr, {-1, 0}, op_i );

                std::vector<int> correct =
                {
                    0, 1, 2, 3,
                    1, 1, 1, 1,
                    1, 1, 1, 1
                };

                CHECK(to_vector(arr) == correct);
            }
        
        }
    
    }




    



}


