#include "jada.hpp"
#include <iostream>
#include <vector>

using namespace jada;

enum Dir { x = 1, y = 0 };

struct Grid{

    Grid(double Lx, double Ly, size_t nx, size_t ny, size_t padding_x, size_t padding_y){
        std::get<Dir::x>(m_L) = Lx;
        std::get<Dir::y>(m_L) = Ly;
        std::get<Dir::x>(m_point_count) = nx;
        std::get<Dir::y>(m_point_count) = ny;
        std::get<Dir::x>(m_padding) = padding_x;
        std::get<Dir::y>(m_padding) = padding_y;
    }

    auto coord(auto idx_tpl) const{
        std::array<double, 2> ret{};

        index_type i = std::get<Dir::x>(idx_tpl);
        index_type j = std::get<Dir::y>(idx_tpl);
        double x = i * delta(Dir::x);
        double y = j * delta(Dir::y);

        ret[Dir::x] = x;
        ret[Dir::y] = y;
        return ret;
    }

    auto L(Dir dir) const{
        return m_L[dir];
    }

    auto delta(Dir dir) const{
        return m_L[dir]/double(m_point_count[dir]);
    }

    auto extent() const{
        return m_point_count;
    }

    auto padding() const{
        return m_padding;
    }

    auto padded_extent() const{
        std::array<size_t, 2> ret(m_point_count);
        for (size_t i = 0; i < ret.size(); ++i){
            ret[i] += m_padding[i] * 2;
        }
        return ret;
    }

    size_t padded_size()const {
        size_t size = 1;
        auto ext = this->padded_extent();
        for (size_t i = 0; i < 2; ++i){
            size *= ext[i];
        }
        return size;
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
        return
        (f(-1) - 2.0 * f(0) + f(1)) / (m_delta * m_delta);
    }

private:
    double m_delta;
};


template<class field>
auto full_span(const field& f, Grid grid){
    return make_span(f, grid.padded_extent());
}

template<class field>
auto full_span(field& f, Grid grid){
    return make_span(f, grid.padded_extent());
}

template<class field>
auto internal_span(const field& f, Grid grid){
    auto fspan = full_span(f, grid);
    auto begin = grid.padding();
    decltype(begin) end(begin);
    for (size_t i = 0; i < end.size(); ++i){
        end[i] += grid.extent()[i];
    } 
    return make_subspan(fspan, begin, end);
}

template<class field>
auto internal_span(field& f, Grid grid){
    auto fspan = full_span(f, grid);
    auto begin = grid.padding();
    decltype(begin) end(begin);
    for (size_t i = 0; i < end.size(); ++i){
        end[i] += grid.extent()[i];
    } 
    return make_subspan(fspan, begin, end);
}




auto initial_condition(Grid grid) {

    return [=](auto idx) {
        auto coord = grid.coord(idx);
        (void) coord;
        return double(0);
        
    };
}
auto boundary_x0(Grid grid){
    (void) grid;
    return [=](auto f) {
        f(1) = 0.0;
    };
}

auto boundary_x1(Grid grid){
    return boundary_x0(grid);
}

auto boundary_y0(Grid grid){
    return boundary_x0(grid);

}
auto boundary_y1(Grid grid){

    return [=](auto f, auto idx) {

        auto coord = grid.coord(idx);
        auto x = std::get<Dir::x>(coord);

        if (x < double(2.0/3.0) ){
            f(1) = 75.0*x;
        }
        else{
            f(1) = 150.0 * (1.0 - x);
        }        
    };

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
                std::cout << span(i, j) << " ";
            }
            std::cout << std::endl;
        }
    } else {
        throw std::logic_error("Only ranks 1 and 2 spans can be printed");
    }
}

auto D2_dx(std::vector<double> f, Grid grid){

    auto df(f);

    auto bc_0 = boundary_x0(grid);
    auto bc_1 = boundary_x1(grid);

    auto i_span = internal_span(f, grid);
    auto o_span = internal_span(df, grid);

    evaluate_boundary_condition(i_span, bc_0, std::array<index_type,2>{0,-1});
    evaluate_boundary_condition(i_span, bc_1, std::array<index_type,2>{0,1});
    evaluate<size_t(Dir::x)>(i_span, o_span, CD2(grid.delta(Dir::x)), all_indices(i_span));

    return df;
}

auto D2_dy(std::vector<double> f, Grid grid){

    auto df(f);

    auto bc_0 = boundary_y0(grid);
    auto bc_1 = boundary_y1(grid);
    auto i_span = internal_span(f, grid);
    auto o_span = internal_span(df, grid);
    evaluate_boundary_condition(i_span, bc_0, std::array<index_type,2>{-1,0});
    evaluate_spatial_boundary_condition(i_span, bc_1, std::array<index_type,2>{1,0});

    evaluate<size_t(Dir::y)>(i_span, o_span, CD2(grid.delta(Dir::y)), all_indices(i_span));
    return df;
}

std::vector<double> operator+(std::vector<double> lhs, std::vector<double> rhs){

    std::vector<double> ret(lhs);
    for (size_t i = 0; i < ret.size(); ++i){
        ret[i] += rhs[i];
    }
    return ret;
}

std::vector<double> operator*(double lhs, std::vector<double> rhs){

    std::vector<double> ret(rhs);
    for (size_t i = 0; i < ret.size(); ++i){
        ret[i] *= lhs;
    }
    return ret;
}

auto nabla(std::vector<double> f, Grid grid){

    auto ddx = D2_dx(f, grid);
    auto ddy = D2_dy(f, grid);

    return ddx + ddy;
}



int main() {

    size_t     nx      = 5;
    size_t     ny      = 6;
    size_t     padding = 1;
    double Lx = 1.0;
    double Ly = 1.0;

    double kappa = 1;
    double T = 0.1;
    double dt = 0.01;

    Grid grid(Lx, Ly, nx, ny, padding, padding);

    std::vector<double> U(grid.padded_size(), double(0));

    assign_for_each_index(internal_span(U, grid), initial_condition(grid));


    double t = 0;
    while(t < T){

        auto dU = (dt/kappa) * nabla(U, grid);
        U = U + dU;             

        t += dt;
    }

    print(full_span(U, grid));

    std::cout << "done" << std::endl;

    return 0;
}