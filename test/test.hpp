#pragma once

namespace jada {

template <size_t Dir, class Span> void set_linear(Span s) {

    using T = typename Span::value_type;

    auto indices = all_indices(s);
    for (auto idx : indices) {
        auto ii                = std::get<Dir>(idx);
        s(tuple_to_array(idx)) = T(ii);
    }
}
struct d_CD2 : public TiledStencil<d_CD2> {

    static constexpr size_t padding = 1;

    template<class F>
    auto operator()(F f) const { return 0.5 * (f(1) - f(-1)); }
};

template <class Container> bool is_unique(const Container& arr) {

    auto copy = arr;
    std::sort(std::begin(copy), std::end(copy));
    auto it       = std::unique(copy.begin(), copy.end());
    bool isUnique = (it == copy.end());

    return isUnique;
}

} // namespace jada