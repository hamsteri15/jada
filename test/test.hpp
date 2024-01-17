#pragma once
#include <vector>
#include <iostream>

template<class T>
using vector_t = std::vector<T>;


template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
    os << "[";
    if (!vec.empty()) {
        os << vec[0];
        for (size_t i = 1; i < vec.size(); ++i) {
            os << ", " << vec[i];
        }
    }
    os << "]";
    return os;
}

namespace jada {


template <size_t Dir, class Span> void set_linear(Span s) {

    auto op = [=](auto idx, auto& e){
        using T = typename Span::value_type;
        auto ii                = std::get<Dir>(idx);
        e  = T(ii);
    };
    for_each_indexed(s, op);
        
}
struct simpleDiff{
    static constexpr size_t begin_padding = 1;
    static constexpr size_t end_padding = 1;
    template <class F> constexpr auto operator()(F f) const { return (f(1) - f(-1)); }
};




template <class Container> bool is_unique(const Container& arr) {

    auto copy = arr;
    std::sort(std::begin(copy), std::end(copy));
    auto it       = std::unique(copy.begin(), copy.end());
    bool isUnique = (it == copy.end());

    return isUnique;
}


template <class Span> void print(Span span) {

    if constexpr (rank(span) == 1) {

        for (size_t i = 0; i < span.extent(0); ++i) { std::cout << span(i) << " "; }
        std::cout << std::endl;
    } else if constexpr (rank(span) == 2) {
        for (size_t i = 0; i < span.extent(0); ++i) {
            for (size_t j = 0; j < span.extent(1); ++j) { std::cout << span(i, j) << " "; }
            std::cout << std::endl;
        }
    } else {
        throw std::logic_error("Only ranks 1 and 2 spans can be printed");
    }
}




} // namespace jada