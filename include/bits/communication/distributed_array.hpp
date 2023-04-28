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
    auto&       local_data() { return m_data; }

    auto begin_padding() const { return m_begin_padding; }
    auto end_padding() const { return m_end_padding; }

    int get_rank() const { return m_rank; }


private:
    int                         m_rank;
    Topology<N>                 m_topology;
    std::array<index_type, N>   m_begin_padding;
    std::array<index_type, N>   m_end_padding;
    std::vector<std::vector<T>> m_data;
};

template <size_t N, class T>
auto make_subspans(const DistributedArray<N, T>& array) {

    using span_t = span_base<const T, N, stdex::layout_stride>;

    std::vector<span_t> ret;

    const auto& data  = array.local_data();
    auto boxes = array.topology().get_boxes(array.get_rank());

    auto bpad = array.begin_padding();
    auto epad = array.end_padding();

    for (size_t i = 0; i < data.size(); ++i) {

        auto unpadded_extent = boxes[i].box.get_extent();
        auto padded_extent = add_padding(unpadded_extent, bpad, epad);


        auto sbegin = bpad;
        auto send = get_end(bpad, extent_to_array(unpadded_extent));

        auto bigspan = make_span(data[i], padded_extent);
        auto sspan = make_subspan(bigspan, sbegin, send);

        ret.push_back(sspan);
    }
    return ret;

}

template <size_t N, class T>
auto make_subspans(DistributedArray<N, T>& array) {

    using span_t = span_base<T, N, stdex::layout_stride>;

    std::vector<span_t> ret;

    auto& data  = array.local_data();
    auto boxes = array.topology().get_boxes(array.get_rank());

    auto bpad = array.begin_padding();
    auto epad = array.end_padding();

    for (size_t i = 0; i < data.size(); ++i) {

        auto unpadded_extent = boxes[i].box.get_extent();
        auto padded_extent = add_padding(unpadded_extent, bpad, epad);


        auto sbegin = bpad;
        auto send = get_end(bpad, extent_to_array(unpadded_extent));

        auto bigspan = make_span(data[i], padded_extent);
        auto sspan = make_subspan(bigspan, sbegin, send);

        ret.push_back(sspan);
    }
    return ret;

}

template <size_t N, class Data>
auto distribute(const Data&               data,
                const Topology<N>&        topo,
                int                       rank,
                std::array<index_type, N> begin_padding,
                std::array<index_type, N> end_padding) {

    using T = typename Data::value_type;

    DistributedArray<N, T> ret(rank, topo, begin_padding, end_padding);

    auto d_array_spans = make_subspans(ret);
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

    auto d_array_spans = make_subspans(array);

    for (size_t i = 0; i < data_spans.size(); ++i) {
        transform(d_array_spans[i], data_spans[i], [](auto r) { return r; });
    }

    return ret;
}

} // namespace jada
