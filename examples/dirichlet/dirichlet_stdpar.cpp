#include "jada.hpp"
#include "grid.hpp"
#include "common.hpp"
#include <numeric>
#include <vector>






template <Dir dir> void D2_di(auto i_span, auto o_span, Grid grid) {

    auto [neg, pos]   = get_direction(dir);
    auto [bc_0, bc_1] = get_boundary_conditions<dir>(grid);

    evaluate_spatial_boundary_condition(i_span, bc_0, neg);
    evaluate_spatial_boundary_condition(i_span, bc_1, pos);
    evaluate<size_t(dir)>(
        i_span, o_span, CD2(grid.delta(dir)), all_indices(i_span));
}

void compute_increment(std::vector<double>& f,
            std::vector<double>& ddx,
            std::vector<double>& ddy,
            std::vector<double>& df,
            Grid                 grid,
            double               dt) {

    D2_di<Dir::x>(internal_span(f, grid), internal_span(ddx, grid), grid);

    D2_di<Dir::y>(internal_span(f, grid), internal_span(ddy, grid), grid);

    auto op = [=](double dd_dx, double dd_dy) {
        return (dt / grid.kappa()) * (dd_dx + dd_dy);
    };

    std::transform(std::execution::par_unseq,
                   std::begin(ddx),
                   std::end(ddx),
                   std::begin(ddy),
                   std::begin(df),
                   op);

    // new_f = f + new_f;

}



int main() {

    size_t nx      = 128;
    size_t ny      = 128;
    size_t padding = 1;

    Grid   grid(nx, ny, padding, padding);

    double dt = compute_time_step(grid);

    std::vector<double> U(grid.padded_size(), double(0));
    std::vector<double> dU(grid.padded_size(), double(0));
    std::vector<double> ddx(grid.padded_size(), double(0));
    std::vector<double> ddy(grid.padded_size(), double(0));
    std::vector<double> newU(grid.padded_size(), double(0));

    assign_for_each_index(internal_span(U, grid), initial_condition(grid));

    while (1) {

        compute_increment(U, ddx, ddy, dU, grid, dt);
        
        //newU = U + dU;
        std::transform
        (
            std::execution::par_unseq,
            std::begin(U), std::end(U),
            std::begin(dU),
            std::begin(newU),
            std::plus{}
        );
        
        std::swap(newU, U);

        //auto norm = l2_norm(newU, U, grid);
        auto norm = mag(dU);
        std::cout << norm << " " << dt << std::endl;
        if (norm < 1E-5) { break; }
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