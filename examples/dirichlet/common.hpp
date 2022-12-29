#pragma once

#include <cmath>
#include "grid.hpp"

auto analytic(double x, double y) {
    size_t N  = 100;
    double pi = 3.14;

    double sum = 0.0;

    for (size_t n = 1; n < N; ++n) {
        auto nd = double(n);
        sum +=
            (std::sin(2.0 * nd * pi / 3.0) / (nd * nd * std::sinh(nd * pi))) *
            std::sin(nd * pi * x) * std::sinh(nd * pi * y);
    }
    return (450.0 / (pi * pi)) * sum;
}

auto analytic(Grid grid) {

    std::vector<double> ret(grid.padded_size(), double(0));

    auto span = internal_span(ret, grid);

    auto op = [=](auto idx) {
        auto coord = grid.coord(idx);

        double x = std::get<Dir::x>(coord);
        double y = std::get<Dir::y>(coord);

        return analytic(x, y);
    };

    assign_for_each_index(span, op);

    return ret;
}

auto l2_error(const std::vector<double>& U, Grid grid) {

    auto v2 = analytic(grid);

    auto s1 = internal_span(U, grid);
    auto s2 = internal_span(v2, grid);

    auto indices = all_indices(s1);

    double mag_diff = 0.0;
    double mag_v2 = 0.0;
    
    //NOTE! capture by reference here, i.e. does not run parallel.
    auto op = [&](auto idx){
        
        auto idx_arr = tuple_to_array(idx);
        auto diff = s1(idx_arr) - s2(idx_arr);
        mag_diff += (diff*diff);
        mag_v2   += (s2(idx_arr)*s2(idx_arr));
    };

    std::for_each(
        std::execution::seq, //!
        std::begin(indices), std::end(indices),
        op
    );

    return std::sqrt(mag_diff) / std::sqrt(mag_v2);
}

auto dirichlet_boundary_condition(Grid grid, auto u_wall) {

    return [=](auto f, auto idx) {
        double wall_value = u_wall(grid.coord(idx));
        f(1)              = 2.0 * wall_value - f(0);
    };
}

template <Dir dir> auto get_boundary_conditions(Grid grid) {

    auto u_wall_zero = [](auto coord){(void) coord; return double(0);};

    auto u_wall_profile = [](auto coord){
        auto x = std::get<Dir::x>(coord);
        if (x < double(2.0 / 3.0)) {
            return 75.0 * x;
        } else {
            return 150.0 * (1.0 - x);
        }
    };

    if constexpr (dir == Dir::x) {
        return std::make_pair(
            dirichlet_boundary_condition(grid, u_wall_zero),
            dirichlet_boundary_condition(grid, u_wall_zero)
        );
    } else {
        return std::make_pair(
            dirichlet_boundary_condition(grid, u_wall_zero),
            dirichlet_boundary_condition(grid, u_wall_profile)
        );
    }
}


auto initial_condition(Grid grid) {

    return [=](auto idx) {
        auto coord = grid.coord(idx);
        (void)coord;
        return double(0);
    };
}

auto mag(const std::vector<double>& v) {

    
    auto l = std::transform_reduce(std::execution::par_unseq,
                                   v.begin(),
                                   v.end(),
                                   double(0),
                                   std::plus<double>{},
                                   [](auto d) { return d * d; });
    return std::sqrt(l);
}


double compute_time_step(Grid grid, double kappa) {

    double dx = grid.delta(Dir::x);
    double dy = grid.delta(Dir::y);

    double dtx = dx * dx / (2.0 * kappa);
    double dty = dy * dy / (2.0 * kappa);

    return 0.3 * std::min(dtx, dty);
}