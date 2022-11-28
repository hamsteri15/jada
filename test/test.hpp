#pragma once

template<class T>
using vector_t = std::vector<T>;




namespace jada {


template <size_t Dir, class Span> void set_linear(Span s) {
    
    auto op = [=](auto idx){
        using T = typename Span::value_type;
        auto ii                = std::get<Dir>(idx);
        s(tuple_to_array(idx)) = T(ii);
    };
    
    for_each_index(all_indices(s), op);
        
}
struct simpleDiff{
    static constexpr size_t padding = 1;
    template <class F> auto operator()(F f) const { return (f(1) - f(-1)); }
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