#include "grid.hpp"
#include "common.hpp"
#include <span>
#include "experimental/nvexec/stream_context.cuh"
#include <experimental/stdexec/execution.hpp>
#include <experimental/exec/static_thread_pool.hpp>

template <Dir dir> stdexec::sender auto D2_di(auto i_span, auto o_span, Grid grid, double dt) {


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
        o_span(tuple_to_array(indices[i])) = (dt/grid.kappa()) * cd_op(stencil);
    };



    stdexec::sender auto first =  stdexec::just() | stdexec::then(call_bc1);
    stdexec::sender auto second = stdexec::just() | stdexec::then(call_bc2);
    stdexec::sender auto third =  stdexec::just() | stdexec::bulk(indices.size(), call_inner);
    
    return stdexec::when_all(first, second) | stdexec::let_value([=]() { return third;});


}

template<Dir dir>
auto Ri(std::vector<double> f, Grid grid, double dt){


    std::vector<double> df(f.size(), 0);

    auto i_span = internal_span(f, grid);
    auto o_span = internal_span(df, grid);
    
    // boundary ops
    auto [neg, pos]   = get_direction(dir);
    auto [bc_0, bc_1] = get_boundary_conditions<dir>(grid);


    evaluate_spatial_boundary_condition(i_span, bc_0, neg);
    evaluate_spatial_boundary_condition(i_span, bc_1, pos);

    evaluate<size_t(dir)>(i_span, o_span, CD2(grid.delta(dir)), all_indices(i_span));

    std::transform
    (
        std::execution::par_unseq,
        std::begin(df), std::end(df),
        std::begin(df),
        [=](auto e) {return e * (dt/grid.kappa());}
    );

    return df;

}


auto residual(std::vector<double> f, Grid grid, double dt){

    auto op = [](std::vector<double> lhs, std::vector<double> rhs){

        std::vector<double> ret(lhs.size(), 0);

        std::transform
        (
            std::execution::par_unseq,
            std::begin(lhs), std::end(lhs),
            std::begin(rhs),
            std::begin(ret),
            std::plus<double>{}
        );
        
        return ret;
    };

    auto dx = stdexec::just(f, grid, dt) | stdexec::then(Ri<Dir::x>);
    auto dy = stdexec::just(f, grid, dt) | stdexec::then(Ri<Dir::y>);


    return stdexec::when_all(dx, dy) | stdexec::then(op);


    //return op(Ri<Dir::x>(f, grid, dt), Ri<Dir::y>(f, grid, dt));
}

auto add(std::vector<double> lhs, std::vector<double> rhs){


    auto op = [=](auto l, auto r){
        std::vector<double> ret(lhs.size(), 0);
        std::transform
        (
            std::execution::par_unseq,
            std::begin(l),
            std::end(l),
            std::begin(r),
            std::begin(ret),
            std::plus{}
        );
        return ret;
    };

    return
    stdexec::just(lhs, rhs) | stdexec::then(op);


}







int main() {

    
    
    exec::static_thread_pool pool(3);
    auto sched = pool.get_scheduler();

    //nvexec::stream_context stream_context{};
    //nvexec::stream_scheduler sched = stream_context.get_scheduler();

    size_t nx      = 128;
    size_t ny      = 128;
    size_t padding = 1;

    Grid   grid(nx, ny, padding, padding);


    double dt = compute_time_step(grid);

    std::vector<double> U(grid.padded_size(), double(0));

    assign_for_each_index(internal_span(U, grid), initial_condition(grid));


    while(1) {


        stdexec::sender auto dU = residual(U, grid, dt);
        auto& ref = dU;

        stdexec::sender auto compute = stdexec::when_all(
            ref | stdexec::let_value(
                      [=](std::vector<double> v) { return add(v, U); }),
            ref | stdexec::then(mag)
        );

        auto work = stdexec::on(sched, compute);
        auto [newU, norm] = stdexec::sync_wait(std::move(work)).value();
        std::swap(newU, U);
        std::cout << norm << " " << dt << std::endl;
        if (norm < 1E-5) { break; }
    }

    /*
    print(internal_span(U, grid));
    std::cout << " ================= "<< std::endl;
    auto correct = analytic(grid);
    print(internal_span(correct, grid));
    */
    //std::cout << l2_error(U, grid) << std::endl;
    //std::cout << "done" << std::endl;


    return 0;
}
