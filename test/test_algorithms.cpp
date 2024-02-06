#include "catch.hpp"

#include "test.hpp"
#include "include/bits/algorithms/algorithms.hpp"
//#include "include/bits/core/core.hpp"


using namespace jada;

TEST_CASE("Test index handles"){


    SECTION("idxhandle_md_to_oned_base"){

        std::vector<int> data = 
        {
            1, 2, 3, 4,
            5, 6, 7, 8,
            9, 10, 11, 12
        };


        auto span = make_span(data, std::array<index_type, 2>{3, 4});
        std::array<index_type, 2> center = {1, 1};
        SECTION("dir = {0, 1}"){
            std::array<index_type, 2> dir  = {0, 1};
            const auto handle = idxhandle_md_to_oned_base(span, center, dir);
            
            CHECK(handle(-1) == 5);
            CHECK(handle(0) == 6);
            CHECK(handle(1) == 7);
        }
        SECTION("dir = {0, -1}"){
            std::array<index_type, 2> dir  = {0, -1};
            const auto handle = idxhandle_md_to_oned_base(span, center, dir);
            
            CHECK(handle(-1) == 7);
            CHECK(handle(0) == 6);
            CHECK(handle(1) == 5);
        }
        SECTION("dir = {1, 0}"){
            std::array<index_type, 2> dir  = {1, 0};
            const auto handle = idxhandle_md_to_oned_base(span, center, dir);
            
            CHECK(handle(-1) == 2);
            CHECK(handle(0) == 6);
            CHECK(handle(1) == 10);
        }
        SECTION("dir = {1, 1}"){
            std::array<index_type, 2> dir  = {1, 1};
            const auto handle = idxhandle_md_to_oned_base(span, center, dir);
            
            CHECK(handle(-1) == 1);
            CHECK(handle(0) == 6);
            CHECK(handle(1) == 11);
        }
    }



    SECTION("idxhandle_md_to_oned"){

        std::vector<int> data = 
        {
            1, 2, 3, 4,
            5, 6, 7, 8,
            9, 10, 11, 12
        };


        auto span = make_span(data, std::array<index_type, 2>{3, 4});
        std::array<index_type, 2> center = {1, 1};
        const auto handle = idxhandle_md_to_oned<1>(span, center);
        
        CHECK(handle(-1) == 5);
        CHECK(handle(0) == 6);
        CHECK(handle(1) == 7);
    
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