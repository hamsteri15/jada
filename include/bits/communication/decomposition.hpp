#pragma once

#include "channel.hpp"
#include "divide_equally.hpp"
#include "include/bits/utils.hpp"
#include "topology.hpp"

#include <array>
#include <cstddef>

namespace jada {

/// @brief Given coordinates of a subdomain returns the gird point offset wrt.
/// position zero of the global grid. The offset can be used to convert local
/// indices to global indices by simple addition: global = local + offset.
/// @param coords Coordinates of the local subdomain.
/// @param coord_dims The number of subdomains in each coordinate direction.
/// @param grid_dims The node count in the global grid
/// @return offset to the beginning index of the subdomain at 'coords'
template <class T1, class T2, class T3>
static constexpr auto offset(T1 coords, T2 coord_dims, T3 global_grid_dims) {

    runtime_assert(indices_in_bounds(coords, coord_dims),
                   "Domain coordinates not in bounds.");
    using idx = typename decltype(coords)::value_type;

    auto offset(coords);
    for (size_t i = 0; i < rank(coords); ++i) {
        offset[i] = idx(global_grid_dims[i] / coord_dims[i]) * coords[i];
    }
    return offset;
}

/// @brief Given coordinates of a subdomain returns the local grid point count
/// in that subdomain. In case of uneven splitting of the global_grid_dims to
/// subdomains in a certain direction, the reminding points are added to the
/// last (largest coordinate) subdomain of that correspoding direction. Example:
/// global_grid_dims = {2,3}, coord_dims = {2,2}, the point count of the
/// subdomain at coords= {0,0} is {1,1} and the point count of the subdomain at
/// coords = {0,1} is {1,2}
/// @param coords Coordinates of the local subdomain.
/// @param coord_dims The number of subdomains in each coordinate direction.
/// @param grid_dims The node count in the global grid
/// @return the local dimensions of the subdomain at 'coords'
template <class T1, class T2, class T3>
static constexpr auto
local_dimensions(T1 coords, T2 coord_dims, T3 global_grid_dims) {

    runtime_assert(indices_in_bounds(coords, coord_dims),
                   "Domain coordinates not in bounds.");

    decltype(global_grid_dims) local_dims{};
    using idx = typename decltype(coords)::value_type;

    for (size_t i = 0; i < rank(coords); ++i) {
        local_dims[i] = global_grid_dims[i] / coord_dims[i];

        // Uneven points added to the last subdomain in the corresponding
        // direction
        if (coords[i] == idx(coord_dims[i] - 1)) {
            local_dims[i] += global_grid_dims[i] % coord_dims[i];
        }
    }
    return local_dims;
}

template <size_t N>
Topology<N> decompose(Box<N>                    domain,
                      int                       n_ranks,
                      std::array<bool, N>       periods,
                      std::array<index_type, N> bpad,
                      std::array<index_type, N> epad) {

    auto topo_dims = divide_equally<N>(size_t(n_ranks));

    std::vector<BoxRankPair<N>> boxes;

    int current_rank = 0;

    for (auto md_idx : md_indices(topo_dims)) {

        auto coord = tuple_to_array(md_idx);
        auto begin =
            offset(coord, topo_dims, extent_to_array(domain.get_extent()));
        auto extent = local_dimensions(
            coord, topo_dims, extent_to_array(domain.get_extent()));

        auto end = get_end(begin, extent);

        auto box = Box<N>(begin, end);

        boxes.push_back(BoxRankPair<N>(box, current_rank));

        current_rank++;
    }

    auto ret = Topology<N>(domain, boxes, periods, bpad, epad);
    runtime_assert(ret.is_valid(), "Invalid topology.");

    return ret;
}

} // namespace jada