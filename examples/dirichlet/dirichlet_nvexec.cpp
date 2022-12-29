#include "grid.hpp"
#include "common.hpp"
#include <experimental/stdexec/execution.hpp>
#include <experimental/exec/static_thread_pool.hpp>


/*
auto maxwell_eqs_snr(float dt,
                     float *time,
                     bool write_results,
                     std::size_t n_iterations,
                     fields_accessor accessor,
                     stdexec::scheduler auto &&computer) {
  return ex::just()
       | exec::on(computer,
                  repeat_n(n_iterations,
                           ex::bulk(accessor.cells, update_h(accessor))
                         | ex::bulk(accessor.cells, update_e(time, dt, accessor))))
       | ex::then(dump_vtk(write_results, accessor));
}
*/

template <Dir dir> std::vector<double> D2_di(std::vector<double> f, Grid grid) {

    std::vector<double> df(f.size(), 0);
    auto i_span = internal_span(f, grid);
    auto o_span = internal_span(df, grid);

    auto [neg, pos]   = get_direction(dir);
    auto [bc_0, bc_1] = get_boundary_conditions<dir>(grid);

    evaluate_spatial_boundary_condition(i_span, bc_0, neg);
    evaluate_spatial_boundary_condition(i_span, bc_1, pos);
    evaluate<size_t(dir)>(
        i_span, o_span, CD2(grid.delta(dir)), all_indices(i_span));

    return df;
}

auto compute_increment(std::vector<double> f,
            Grid                 grid,
            double               dt) {

    auto df_x = [=](auto v){
        return D2_di<Dir::x>(v, grid);
    };
    auto df_y = [=](auto v){
        return D2_di<Dir::y>(v, grid);
    };

    // Declare a pool of 8 worker threads:
    exec::static_thread_pool pool(8);

    // Get a handle to the thread pool:
    auto sched = pool.get_scheduler();

    stdexec::sender auto first = stdexec::just(f) | stdexec::then(df_x);
    stdexec::sender auto second = stdexec::just(f) | stdexec::then(df_y);

    auto work = stdexec::when_all
    (
        stdexec::on(sched, first),
        stdexec::on(sched, second)
    );


    auto [ddx, ddy] = stdexec::sync_wait(std::move(work)).value();


    std::vector<double> df(f.size(), double(0));

    auto op = [=](double dd_dx, double dd_dy) {
        return (dt / grid.kappa()) * (dd_dx + dd_dy);
    };
    std::transform(//std::execution::par_unseq,
                   std::begin(ddx),
                   std::end(ddx),
                   std::begin(ddy),
                   std::begin(df),
                   op);


    return df;



    /*

    auto third = [=](std::vector<double> f1, std::vector<double> f2){

        auto op = [=](double dd_dx, double dd_dy) {
            return (dt / grid.kappa()) * (dd_dx + dd_dy);
        };
        std::vector<double> ret(f1.size());
        std::transform(
            std::begin(f1), std::end(f1),
            std::begin(f2),
            std::begin(ret),
            op
        );
        return ret;

    };

    return stdexec::when_all(first, second) | stdexec::then(third);
    */
    
}

/*
// Handler for the "classify" request type
ex::sender auto handle_classify_request(const http_request& req) {
    return
        // start with the input buffer
        ex::just(req)
        // extract the image from the input request
        | ex::then(extract_image)
        // analyze the content of the image and classify it
        // we are doing the processing on the same thread
        | ex::then(do_classify)
        // handle errors
        | ex::upon_error(on_classification_error)
        // handle cancellation
        | ex::upon_stopped(on_classification_cancelled)
        // transform this into a response
        | ex::then(to_response)
        // done
        ;
}
*/

int main() {

    size_t nx      = 5;
    size_t ny      = 5;
    size_t padding = 1;

    Grid   grid(nx, ny, padding, padding);


    double dt = compute_time_step(grid);

    std::vector<double> U(grid.padded_size(), double(0));
    std::vector<double> newU(grid.padded_size(), double(0));

    assign_for_each_index(internal_span(U, grid), initial_condition(grid));

   





    while(1){

        std::vector<double> dU = compute_increment(U, grid, dt);

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




    }



    print(internal_span(U, grid));
    std::cout << " ================= "<< std::endl;
    auto correct = analytic(grid);
    print(internal_span(correct, grid));
    
    std::cout << l2_error(U, grid) << std::endl;
    std::cout << "done" << std::endl;


    return 0;
}
