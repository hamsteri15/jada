#include "grid.hpp"
#include "common.hpp"
#include <experimental/stdexec/execution.hpp>
#include <experimental/exec/static_thread_pool.hpp>
#include <span>
//#include "experimental/nvexec/stream_context.cuh"


template <Dir dir> stdexec::sender auto D2_di(auto i_span, auto o_span, Grid grid) {


    // boundary ops
    auto [neg, pos]   = get_direction(dir);
    auto [bc_0, bc_1] = get_boundary_conditions<dir>(grid);

    auto call_bc1 = [=](){
        evaluate_spatial_boundary_condition(i_span, bc_0, neg);
    };
    auto call_bc2 = [=](){
        evaluate_spatial_boundary_condition(i_span, bc_1, pos);
    };

    // inner op
    auto indices = all_indices(i_span);
    auto cd_op = CD2(grid.delta(dir));

    auto call_inner = [=](auto i){
        const auto stencil = idxhandle_md_to_oned<size_t(dir)>(i_span, indices[i]);
        o_span(tuple_to_array(indices[i])) = cd_op(stencil);
    };



    stdexec::sender auto first =  stdexec::just() | stdexec::then(call_bc1);
    stdexec::sender auto second = stdexec::just() | stdexec::then(call_bc2);
    stdexec::sender auto third =  stdexec::just() | stdexec::bulk(indices.size(), call_inner);
    
    return stdexec::when_all(first, second) | stdexec::let_value([=]() { return third;});


}

stdexec::sender auto compute_increment(std::vector<double>& f,
            std::vector<double>& ddx,
            std::vector<double>& ddy,
            std::vector<double>& df,
            Grid                 grid,
            double               dt) {


    stdexec::sender auto d2_dx = D2_di<Dir::x>(internal_span(f, grid), internal_span(ddx, grid), grid); 
    stdexec::sender auto d2_dy = D2_di<Dir::y>(internal_span(f, grid), internal_span(ddy, grid), grid); 
    

    auto lhs = std::span{ddx.begin(), ddx.size()};
    auto rhs = std::span{ddy.begin(), ddy.size()};
    auto ret = std::span{df.begin(), df.size()};
    auto increment2 = [=](auto i){
        ret[i] = (dt/grid.kappa()) * (lhs[i] + rhs[i]);
    };


    stdexec::sender auto last =  stdexec::just() | stdexec::bulk(df.size(), increment2);

    return stdexec::when_all(d2_dx, d2_dy) | stdexec::let_value([=](){return last;});
}


int main() {
    // Declare a pool of 8 worker threads:
    //exec::static_thread_pool pool(8);
    //exec::static_thread_pool pool{std::thread::hardware_concurrency()};
    // Get a handle to the thread pool:
    exec::static_thread_pool pool;
    auto sched = pool.get_scheduler();

    //nvexec::stream_context stream_context{};
    //nvexec::stream_scheduler sched = stream_context.get_scheduler(nvexec::stream_priority::low);
    //nvexec::stream_scheduler sched = stream_context.get_scheduler();

    size_t nx      = 128;
    size_t ny      = 128;
    size_t padding = 1;

    Grid   grid(nx, ny, padding, padding);


    double dt = compute_time_step(grid);

    std::vector<double> U(grid.padded_size(), double(0));
    std::vector<double> dU(U.size(), double(0));
    std::vector<double> newU(U.size(), double(0));
    std::vector<double> ddx(U.size(), double(0));
    std::vector<double> ddy(U.size(), double(0));

    assign_for_each_index(internal_span(U, grid), initial_condition(grid));



    while(1){

        stdexec::sender auto compute = compute_increment(U, ddx, ddy, dU, grid, dt);
        auto work = stdexec::on(sched, compute);
        stdexec::sync_wait(std::move(work));

        std::transform
        (
            std::execution::par_unseq,
            std::begin(U), std::end(U),
            std::begin(dU),
            std::begin(newU),
            std::plus{}
        );
        
        std::swap(newU, U);

        auto norm = mag(dU);
        std::cout << norm << " " << dt << std::endl;
        if (norm < 1E-5) { break; }

        dU = std::vector<double>(U.size(), double(0));


    }


    /*
    print(internal_span(U, grid));
    std::cout << " ================= "<< std::endl;
    auto correct = analytic(grid);
    print(internal_span(correct, grid));
    */
    std::cout << l2_error(U, grid) << std::endl;
    std::cout << "done" << std::endl;


    return 0;
}
