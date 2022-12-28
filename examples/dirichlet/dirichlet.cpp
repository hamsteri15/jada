#include "jada.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

using namespace jada;

enum Dir { x = 1, y = 0 };

struct Grid {

    Grid(double Lx,
         double Ly,
         size_t nx,
         size_t ny,
         size_t padding_x,
         size_t padding_y) {
        std::get<Dir::x>(m_L)           = Lx;
        std::get<Dir::y>(m_L)           = Ly;
        std::get<Dir::x>(m_point_count) = nx;
        std::get<Dir::y>(m_point_count) = ny;
        std::get<Dir::x>(m_padding)     = padding_x;
        std::get<Dir::y>(m_padding)     = padding_y;
    }

    auto coord(auto idx_tpl) const {
        std::array<double, 2> ret{};

        index_type i = std::get<Dir::x>(idx_tpl);
        index_type j = std::get<Dir::y>(idx_tpl);
        double     x = 0.5 * delta(Dir::x) + i * delta(Dir::x);
        double     y = 0.5 * delta(Dir::y) + j * delta(Dir::y);

        ret[Dir::x] = x;
        ret[Dir::y] = y;
        return ret;
    }

    auto L(Dir dir) const { return m_L[dir]; }

    auto delta(Dir dir) const { return m_L[dir] / double(m_point_count[dir]); }

    auto extent() const { return m_point_count; }

    auto padding() const { return m_padding; }

    auto padded_extent() const {
        std::array<size_t, 2> ret(m_point_count);
        for (size_t i = 0; i < ret.size(); ++i) { ret[i] += m_padding[i] * 2; }
        return ret;
    }

    size_t padded_size() const {
        size_t size = 1;
        auto   ext  = this->padded_extent();
        for (size_t i = 0; i < 2; ++i) { size *= ext[i]; }
        return size;
    }

    size_t size() const {
        size_t ret = 1;
        auto   ext = this->extent();
        for (size_t i = 0; i < 2; ++i) { ret *= ext[i]; }
        return ret;
    }

private:
    std::array<double, 2> m_L;
    std::array<size_t, 2> m_point_count;
    std::array<size_t, 2> m_padding;
};

struct CD2 {

    CD2(double delta)
        : m_delta(delta) {}

    auto operator()(auto f) const {
        return (f(-1) - 2.0 * f(0) + f(1)) / (m_delta * m_delta);
    }

private:
    double m_delta;
};

template <class field> auto full_span(const field& f, Grid grid) {
    return make_span(f, grid.padded_extent());
}

template <class field> auto full_span(field& f, Grid grid) {
    return make_span(f, grid.padded_extent());
}

template <class field> auto internal_span(const field& f, Grid grid) {
    auto            fspan = full_span(f, grid);
    auto            begin = grid.padding();
    decltype(begin) end(begin);
    for (size_t i = 0; i < end.size(); ++i) { end[i] += grid.extent()[i]; }
    return make_subspan(fspan, begin, end);
}

template <class field> auto internal_span(field& f, Grid grid) {
    auto            fspan = full_span(f, grid);
    auto            begin = grid.padding();
    decltype(begin) end(begin);
    for (size_t i = 0; i < end.size(); ++i) { end[i] += grid.extent()[i]; }
    return make_subspan(fspan, begin, end);
}

auto initial_condition(Grid grid) {

    return [=](auto idx) {
        auto coord = grid.coord(idx);
        (void)coord;
        return double(0);
    };
}

auto dirichlet_boundary_condition(Grid grid, auto u_wall) {

    return [=](auto f, auto idx) {
        double wall_value = u_wall(grid.coord(idx));
        f(1)              = 2.0 * wall_value - f(0);
    };
}

auto boundary_x0(Grid grid) {
    (void)grid;
    auto u_wall = [](auto coord) {
        (void)coord;
        return double(0);
    };
    return dirichlet_boundary_condition(grid, u_wall);
}

auto boundary_x1(Grid grid) { return boundary_x0(grid); }

auto boundary_y0(Grid grid) { return boundary_x0(grid); }
auto boundary_y1(Grid grid) {

    auto u_wall = [](auto coord) {
        auto x = std::get<Dir::x>(coord);
        if (x < double(2.0 / 3.0)) {
            return 75.0 * x;
        } else {
            return 150.0 * (1.0 - x);
        }
    };
    return dirichlet_boundary_condition(grid, u_wall);
}
template <class Span> void print(Span span) {

    if constexpr (rank(span) == 1) {

        for (size_t i = 0; i < span.extent(0); ++i) {
            std::cout << span(i) << " ";
        }
        std::cout << std::endl;
    } else if constexpr (rank(span) == 2) {
        for (size_t i = 0; i < span.extent(0); ++i) {
            for (size_t j = 0; j < span.extent(1); ++j) {
                std::cout << std::setprecision(2) << std::setw(4) << span(i, j)
                          << " ";
            }
            std::cout << std::endl;
        }
    } else {
        throw std::logic_error("Only ranks 1 and 2 spans can be printed");
    }
}

auto get_direction(Dir dir) {

    std::array<index_type, 2> pos{};
    std::array<index_type, 2> neg{};

    pos[dir] = 1;
    neg[dir] = -1;
    return std::make_pair(neg, pos);
}

template <Dir dir> auto get_boundary_conditions(Grid grid) {

    if constexpr (dir == Dir::x) {
        return std::make_pair(boundary_x0(grid), boundary_x1(grid));
    } else {
        return std::make_pair(boundary_y0(grid), boundary_y1(grid));
    }
}

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
            double               kappa,
            double               dt) {

    D2_di<Dir::x>(internal_span(f, grid), internal_span(ddx, grid), grid);

    D2_di<Dir::y>(internal_span(f, grid), internal_span(ddy, grid), grid);

    auto op = [=](double dd_dx, double dd_dy) {
        return (dt / kappa) * (dd_dx + dd_dy);
    };

    std::transform(std::execution::par_unseq,
                   std::begin(ddx),
                   std::end(ddx),
                   std::begin(ddy),
                   std::begin(df),
                   op);

    // new_f = f + new_f;

}

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

auto mag(const std::vector<double>& v) {

    
    auto l = std::transform_reduce(std::execution::par_unseq,
                                   v.begin(),
                                   v.end(),
                                   double(0),
                                   std::plus<double>{},
                                   [](auto d) { return d * d; });
    return std::sqrt(l);
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

double compute_time_step(Grid grid, double kappa) {

    double dx = grid.delta(Dir::x);
    double dy = grid.delta(Dir::y);

    double dtx = dx * dx / (2.0 * kappa);
    double dty = dy * dy / (2.0 * kappa);

    return 0.3 * std::min(dtx, dty);
}

int main() {

    size_t nx      = 50;
    size_t ny      = 50;
    size_t padding = 1;
    double Lx      = 1.0;
    double Ly      = 1.0;

    double kappa = 1;
    Grid   grid(Lx, Ly, nx, ny, padding, padding);

    double dt = compute_time_step(grid, kappa);

    std::vector<double> U(grid.padded_size(), double(0));
    std::vector<double> dU(grid.padded_size(), double(0));
    std::vector<double> ddx(grid.padded_size(), double(0));
    std::vector<double> ddy(grid.padded_size(), double(0));
    std::vector<double> newU(grid.padded_size(), double(0));

    assign_for_each_index(internal_span(U, grid), initial_condition(grid));

    while (1) {

        compute_increment(U, ddx, ddy, dU, grid, kappa, dt);
        
        //newU = U + dU;
        std::transform
        (
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