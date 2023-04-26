#pragma once

#include "channel.hpp"
#include "include/bits/algorithms/algorithms.hpp"
#include "topology.hpp"
namespace jada {

template <size_t N, class T> struct DistributedArray {

    DistributedArray(int                       rank,
                     Topology<N>               topology,
                     std::array<index_type, N> begin_padding,
                     std::array<index_type, N> end_padding)
        : m_rank(rank)
        , m_topology(topology)
        , m_begin_padding(begin_padding)
        , m_end_padding(end_padding) {

        runtime_assert(m_topology.is_valid(), "Not a valid topology");

        for (auto box : m_topology.get_boxes(m_rank)) {
            auto ext  = box.get_extent();
            auto pext = add_padding(ext, m_begin_padding, m_end_padding);
            auto data = std::vector<T>(flat_size(pext), T(0));
            m_data.push_back(data);
        }
    }

    const auto& topology() const { return m_topology; }
    const auto& local_data() const { return m_data; }

    auto local_spans() {
        std::vector<span<T, N>> ret;

        size_t i = 0;
        for (auto box : m_topology.get_boxes(m_rank)) {
            auto ext  = box.get_extent();
            auto pext = add_padding(ext, m_begin_padding, m_end_padding);

            ret.push_back
            (
                make_span(m_data[i], pext)
            );
            ++i;
        }

        return ret;
    }

    auto unpadded_local_data() const {

        std::vector<std::vector<T>> ret;

        size_t i = 0;
        for (auto box : m_topology.get_boxes(m_rank)) {

            auto ext  = box.get_extent();
            auto pext = add_padding(ext, m_begin_padding, m_end_padding);

            auto data = std::vector<T>(flat_size(ext));

            auto i_span = make_subspan(make_span(m_data[i], pext),
                                       m_begin_padding,
                                       get_end(m_begin_padding, extent_to_array(ext)));

            auto o_span = make_span(data, ext);

            transform(i_span, o_span, [](auto r) { return r; });
            ret.push_back(data);

            ++i;
        }

        return ret;
    }

private:
    int                         m_rank;
    Topology<N>                 m_topology;
    std::array<index_type, N>   m_begin_padding;
    std::array<index_type, N>   m_end_padding;
    std::vector<std::vector<T>> m_data;
};

template<size_t N, class Data>
auto distribute(const Data& data, Topology<N> topo, int rank)
{

    using T = typename Data::value_type;

    std::array<index_type, N> bpad{};
    std::array<index_type, N> epad{};

    DistributedArray<N, T> ret(rank, topo, bpad, epad);
    return ret;



}

template <size_t N, class T> auto reduce(const DistributedArray<N, T>& array) {

    auto           global_dims = array.topology().get_domain().get_extent();
    std::vector<T> ret(flat_size(global_dims));

    auto ldata = array.unpadded_local_data();




    return ret;
}

} // namespace jada
