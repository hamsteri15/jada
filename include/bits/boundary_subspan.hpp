#pragma once

#include "subspan.hpp"

namespace jada {

template <class T, size_type N> struct BoundarySubspan {

    BoundarySubspan(T span, std::array<index_type, N> direction)
        : m_span(span)
        , m_direction(direction) {}


    constexpr auto& operator()(index_type idx){
        return m_span(convert_index(idx));
    }
    
    constexpr const auto& operator()(index_type idx) const{
        return m_span(convert_index(idx));
    } 

private:
    T                        m_span;
    std::array<index_type, N> m_direction;

    constexpr auto convert_index(index_type idx) const{
        std::array<index_type, N> new_idx{};

        for (size_t i = 0; i < N; ++i){
            if (m_direction[i] == 1){
                new_idx[i] = idx;
            }
            if (m_direction[i] == -1){
                new_idx[i] = -idx;
            }
        }
        return new_idx;
    }

};

template<class Span, size_t N>
static constexpr auto make_boundary_subspan(Span s, std::array<index_type, N> direction){

    return BoundarySubspan<Span, N>(s, direction);

}


} // namespace jada