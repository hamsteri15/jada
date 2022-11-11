#pragma once
#include <array>
#include <cstddef>
#include <vector>
#include "algorithms.hpp"

namespace jada {

enum class ConnectivityType { Star, Box };

using idx_t = int;

template <size_t N> using direction = std::array<idx_t, N>;

namespace detail {

template <size_t N, ConnectivityType CT> static constexpr size_t neighbour_count() {

    if constexpr (CT == ConnectivityType::Star) { return 2 * N; }

    size_t n = 1;
    for (size_t i = 0; i < N; ++i) { n *= 3; }
    return n - 1; // neglect all zeroes
}

} // namespace detail

///
///@brief Given N dimensions holds the possible neighbour directions that a
/// block may have based on the
/// connectivity type. For N=2 the neighbours for connectivity type Star
/// are: [1, 0], [-1, 0], [0,1], [0,-1]. For N = 2 and Box connectivity,
/// also the combinations are returned i.e. [1,-1] [-1, 1]...
///@tparam N number of spatial dimensions
///@tparam CT connectivity type Box/Star
template <size_t N, ConnectivityType CT> struct Neighbours {

    static constexpr auto create() {
        if constexpr (CT == ConnectivityType::Star) {
            return star_neighbours();
        } else {
            return box_neighbours();
        }
    }

    static constexpr size_t m_count = detail::neighbour_count<N, CT>();

    static constexpr std::array<direction<N>, m_count> m_neighbours = create();

public:
    ///@brief Get the array of neighbours
    ///@return constexpr auto array of neighbours
    static constexpr auto get() { return m_neighbours; }

    ///@brief Get the total neighbour count
    ///@return constexpr size_t
    static constexpr size_t count() { return m_neighbours.size(); }

    ///@brief Converts a neighbour direction to an idx in the neighbours array
    ///@param neighbour direction of a neighbour
    ///@return constexpr size_t the index of the neihbour if such neigbhour is
    /// found. -1 otherwise.
    static constexpr idx_t idx(direction<N> dir) {

        auto compare = [](direction<N> a, direction<N> b){
            for (size_t i = 0; i < N; ++i){
                if (a[i] != b[i]) {
                    return false;
                }
            }
            return true;
        };

        for (size_t i = 0; i < count(); ++i) {
            if (compare(m_neighbours[i], dir)) { return idx_t(i); }
        }
        return -1;
        // throw std::logic_error("Invalid neighbour");
    }

private:
    ///@brief Computes the star neighbours by permuting a positive and negative
    /// unit vectors pos =  {1,0,0,0 ... N}, neg = {-1, 0, 0, 0 ... N}
    ///@return std::array<direction<N>, 2*N> neighbours which correspond negative
    /// and positive unit vectors of an N-dimensional space (hence the 2*N)
    static constexpr auto star_neighbours() {
        constexpr size_t n_count = detail::neighbour_count<N, CT>();

        direction<N> dir{};
        dir.back() = 1;
        std::array<direction<N>, n_count> all{};
        
        auto change_sign = [](auto a) {
            auto ret(a);
            for (auto& r : ret) { r *= -1; }
            return ret;
        };

        size_t i = 0;
        do {

            all[i]     = dir;
            all[i + N] = change_sign(dir);
            ++i;
            // permutations.push_back(temp);
        } while (next_permutation(dir.begin(), dir.end()));
        
        return all;
    }

    ///@brief Computes the box neighbour directions
    ///@return constexpr auto std::array<direction<N>, 3^N>
    static constexpr auto box_neighbours() {

        constexpr auto n_combinations = detail::neighbour_count<N, CT>();

        std::array<direction<N>, n_combinations> combinations{};
        direction<N> combination{1};


        auto all_zero = [](auto arr){
            for (size_t i = 0; i < std::size(arr); ++i){
                if (arr[i] != idx_t(0)){
                    return false;
                }
            }
            return true;
        };

        size_t j = 0;
        for (size_t i = 0; i < n_combinations + 1; ++i) {

            for (auto& c : combination) {
                if (c != -1) {
                    c -= 1;
                    break;
                } else {
                    c = 1;
                }
            }

            // Neglect the no-op [0,0,0...]
            if (!all_zero(combination)) {
                combinations[j] = combination;
                ++j;
            }
        }
        
        return combinations;
    }
};
} // namespace jada