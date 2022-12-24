#include "jada.hpp"
#include <iostream>
#include <vector>

using namespace jada;

enum Dir { x = 1, y = 0 };

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


auto initial_condition(double dx, double dy) {

    (void) dx;
    (void) dy;
    return [=](auto idx) {
        /* 
        index_type i = std::get<Dir::x>(idx);
        index_type j = std::get<Dir::y>(idx);
        double x = i * dx;
        double y = j * dy;
        return x * 2.0;
        */
        (void) idx;
        return double(0);
        
    };
}
auto boundary_x0(){
    
    return [=](auto f) {
        f(1) = 0.0;
    };
}

auto boundary_x1(){
    
    return [=](auto f) {
        f(1) = 0.0;
    };
}

auto boundary_y0(){

    return [=](auto f) {
        f(1) = 0.0;
    };

}
auto boundary_y1(double dx, double dy){

    return [=](auto f, auto idx) {

        index_type i = std::get<Dir::x>(idx);
        index_type j = std::get<Dir::y>(idx);
        double x = i * dx;
        double y = j * dy;

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

auto D2_dx(auto i_span, auto o_span, double dx, double dy){
    auto bc_0 = boundary_x0();
    auto bc_1 = boundary_x1();
    evaluate_boundary_condition(i_span, bc_0, std::array<index_type,2>{0,-1});
    evaluate_boundary_condition(i_span, bc_1, std::array<index_type,2>{0,1});
    auto indices = all_indices(i_span);
    evaluate<size_t(Dir::x)>(i_span, o_span, CD2(dx), indices);
}

auto D2_dy(auto i_span, auto o_span, double dx, double dy){
    auto bc_0 = boundary_y0();
    auto bc_1 = boundary_y1(dx, dy);
    evaluate_boundary_condition(i_span, bc_0, std::array<index_type,2>{-1,0});
    evaluate_boundary_condition(i_span, bc_1, std::array<index_type,2>{1,0});

    auto indices = all_indices(i_span);
    evaluate<size_t(Dir::y)>(i_span, o_span, CD2(dy), indices);
}


int main() {

    size_t     nx      = 5;
    size_t     ny      = 6;
    size_t     padding = 1;
    std::array<size_t, 2> dims{ny + 2 * padding, nx + 2 * padding};

    double Lx = 1.0;
    double Ly = 1.0;
    double dx = Lx / double(nx);
    double dy = Ly / double(ny);


    std::vector<double> U((nx+2*padding) * (ny + 2*padding), double(0));
    std::vector<double> dU((nx+2*padding) * (ny + 2*padding), double(0));

    auto u_full = make_span(U, dims);
    auto du_full = make_span(dU, dims);


    //This is a subspan which neglects the padding
    auto u      = make_subspan(u_full,
                          std::array<size_t, 2>{padding, padding},
                          std::array<size_t, 2>{ny + padding, nx + padding});

    assign_for_each_index(u, initial_condition(dx, dy));

    //D2_dx(u, du, dx, dy);
    //D2_dy(u, du, dx, dy);
    
    print(du_full);

    //    std::cout << Dir::x << std::endl;
    std::cout << "done" << std::endl;

    return 0;
}