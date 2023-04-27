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

    int get_rank() const { return m_rank; }

    auto unpadded_local_data() const {

        std::vector<std::vector<T>> ret;

        size_t i = 0;
        for (auto box : m_topology.get_boxes(m_rank)) {

            auto ext  = box.get_extent();
            auto pext = add_padding(ext, m_begin_padding, m_end_padding);

            auto data = std::vector<T>(flat_size(ext));

            auto i_span =
                make_subspan(make_span(m_data[i], pext),
                             m_begin_padding,
                             get_end(m_begin_padding, extent_to_array(ext)));

            auto o_span = make_span(data, ext);

            transform(i_span, o_span, [](auto r) { return r; });
            ret.push_back(data);

            ++i;
        }

        return ret;
    }

    // TODO: get rid and work with data insted (create spans on call site)
    auto local_spans() {
        std::vector<span<T, N>> ret;

        size_t i = 0;
        for (auto box : m_topology.get_boxes(m_rank)) {
            auto ext  = box.get_extent();
            auto pext = add_padding(ext, m_begin_padding, m_end_padding);

            ret.push_back(make_span(m_data[i], pext));
            ++i;
        }

        return ret;
    }

    // TODO: get rid and work with data insted (create spans on call site)
    auto local_spans() const {
        std::vector<span<const T, N>> ret;

        size_t i = 0;
        for (auto box : m_topology.get_boxes(m_rank)) {
            auto ext  = box.get_extent();
            auto pext = add_padding(ext, m_begin_padding, m_end_padding);

            ret.push_back(make_span(m_data[i], pext));
            ++i;
        }

        return ret;
    }

    // TODO: get rid and work with data insted (create spans on call site)
    auto unpadded_local_spans() {

        auto spans = local_spans();

        std::vector<span<T, N>> ret;

        auto boxes = m_topology.get_boxes(m_rank);

        for (size_t i = 0; i < spans.size(); ++i) {
            auto begin = m_begin_padding;
            auto end   = get_end(begin, extent_to_array(boxes[i].get_extent()));

            auto ss = make_subspan(spans[i], begin, end);

            ret.push_back(ss);
        }
        return ret;
    }

    // TODO: get rid and work with data insted (create spans on call site)
    auto unpadded_local_spans() const {

        auto spans = local_spans();

        std::vector<span<const T, N>> ret;

        auto boxes = m_topology.get_boxes(m_rank);

        for (size_t i = 0; i < spans.size(); ++i) {
            auto begin = m_begin_padding;
            auto end   = get_end(begin, extent_to_array(boxes[i].get_extent()));

            auto ss = make_subspan(spans[i], begin, end);

            ret.push_back(ss);
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




template <size_t N, class Data>
auto distribute(const Data&               data,
                const Topology<N>&        topo,
                int                       rank,
                std::array<index_type, N> begin_padding,
                std::array<index_type, N> end_padding) {

    using T = typename Data::value_type;

    DistributedArray<N, T> ret(rank, topo, begin_padding, end_padding);

        
    auto d_array_spans = ret.unpadded_local_spans();
    auto data_spans    = make_subspans(data, topo, rank);

    for (size_t i = 0; i < data_spans.size(); ++i) {
        transform(data_spans[i], d_array_spans[i], [](auto r) { return r; });
    }
    
    return ret;
}

template <size_t N, class T> auto reduce(const DistributedArray<N, T>& array) {

    auto global_dims = array.topology().get_domain().get_extent();

    std::vector<T> ret(flat_size(global_dims));

    auto data_spans = make_subspans(ret, array.topology(), array.get_rank());

    auto d_array_spans = array.unpadded_local_spans();

    for (size_t i = 0; i < data_spans.size(); ++i) {
        transform(d_array_spans[i], data_spans[i], [](auto r) { return r; });
    }
    
    return ret;
}

} // namespace jada
