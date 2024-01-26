#pragma once

#include "channel.hpp"
#include "gather.hpp"
#include "include/bits/algorithms/algorithms.hpp"
#include "include/bits/core/tuple_extensions.hpp"
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

    size_t local_subdomain_count() const { return m_data.size(); }

    // Note this does not in general equal to number of processes
    size_t global_subdomain_count() const {
        return m_topology.get_boxes().size();
    }

    int get_rank() const { return m_rank; }

    ///
    ///@brief Returns the boxes describing the shapes of data held locally by
    /// this instance of a distributed array.
    ///
    ///@return std::vector<BoxRankPair<N>> A vector of box-rank pairs where the
    /// rank is always m_rank.
    ///
    std::vector<BoxRankPair<N>> local_boxes() const {
        return m_topology.get_boxes(m_rank);
    }

private:
    int                         m_rank;
    Topology<N>                 m_topology;
    std::array<index_type, N>   m_begin_padding;
    std::array<index_type, N>   m_end_padding;
    std::vector<std::vector<T>> m_data;
};

///
///@brief Returns the local element count (without padding) held by the input
/// distributed array.
///
///@param array The input array to query the local element count from.
///@return size_t The element count without padding held by the input array.
///
template <size_t N, class T>
static inline size_t local_element_count(const DistributedArray<N, T>& array) {

    auto   boxes = array.local_boxes();
    size_t size  = 0;
    for (auto box : boxes) {
        auto ext = box.get_extent();
        size += flat_size(ext);
    }
    return size;
}

///
///@brief Returns the global element count (without padding) of the input
/// distributed array.
///
///@param array The input array to query the global element count from.
///@return size_t The global element count without padding.
///
template <size_t N, class T>
static inline size_t global_element_count(const DistributedArray<N, T>& array) {

    return flat_size(array.topology().get_domain().get_extent());
}

///
///@brief Returns the local capacity (with padding) held by the input array
/// distributed array.
///
///@param array The input array to query the local capacity from.
///@return size_t The local capacity with padding held by the input array.
///
template <size_t N, class T>
static inline size_t local_capacity(const DistributedArray<N, T>& array) {

    size_t size = 0;
    for (const auto& v : array.local_data()) { size += v.size(); }
    return size;
}

/// @brief Returns the unpadded subspans to local data held by the input
/// distributed array.
/// @param array The input array to get the subspans to local data.
/// @return The subspans to local data held by the input array.
template <size_t N, class T>
auto make_subspans(const DistributedArray<N, T>& array) {

    using span_t = span_base<const T, N, stdex::layout_stride>;

    std::vector<span_t> ret;

    const auto& data  = array.local_data();
    auto        boxes = array.local_boxes();

    auto bpad = array.begin_padding();
    auto epad = array.end_padding();

    for (size_t i = 0; i < data.size(); ++i) {

        auto unpadded_extent = boxes[i].box.get_extent();
        auto padded_extent   = add_padding(unpadded_extent, bpad, epad);

        auto sbegin = bpad;
        auto send   = get_end(bpad, extent_to_array(unpadded_extent));

        auto bigspan = make_span(data[i], padded_extent);
        auto sspan   = make_subspan(bigspan, sbegin, send);

        ret.push_back(sspan);
    }
    return ret;
}

/// @brief Returns the unpadded subspans to local data held by the input
/// distributed array.
/// @param array The input array to get the subspans to local data.
/// @return The subspans to local data held by the input array.
template <size_t N, class T> auto make_subspans(DistributedArray<N, T>& array) {

    using span_t = span_base<T, N, stdex::layout_stride>;

    std::vector<span_t> ret;

    auto& data  = array.local_data();
    auto  boxes = array.local_boxes();

    auto bpad = array.begin_padding();
    auto epad = array.end_padding();

    for (size_t i = 0; i < data.size(); ++i) {

        auto unpadded_extent = boxes[i].box.get_extent();
        auto padded_extent   = add_padding(unpadded_extent, bpad, epad);

        auto sbegin = bpad;
        auto send   = get_end(bpad, extent_to_array(unpadded_extent));

        auto bigspan = make_span(data[i], padded_extent);
        auto sspan   = make_subspan(bigspan, sbegin, send);

        ret.push_back(sspan);
    }
    return ret;
}

/// @brief Serializes the local data of the input distributed array to a flat
/// vector. Note: This function neglects the padding in the output array.
/// @param array The input array to serialize.
/// @return A flat std::vector of the same element type containing the local
/// data the input array holds.
template <size_t N, class T>
static inline std::vector<T>
serialize_local(const DistributedArray<N, T>& array) {

    auto size = local_element_count(array);

    auto spans = make_subspans(array);

    // Offsets in the output array where to begin writing
    std::vector<size_t> offsets = [&]() {
        std::vector<size_t> ret(array.local_subdomain_count(), 0);
        for (size_t i = 1; i < ret.size(); ++i) {
            ret[i] = ret[i - 1] + spans[i - 1].size();
        }

        return ret;
    }();

    std::vector<T> ret(size);

    for (size_t i = 0; i < spans.size(); ++i) {

        T*         begin = &(ret[offsets[i]]);
        auto       ext   = extent(spans[i]);
        span<T, N> to(begin, ext);
        transform(spans[i], to, [](auto r) { return r; });
    }

    return ret;
}

///
///@brief Given an stl-like container of data, creates a distributed array
/// slicing the local portions of the input container based on the input
/// topology and rank.
///
///@param data An stl-like contiguous container from which the local portion is
/// sliced.
///@param topo The topology describing the distribution of the data.
///@param rank The rank which data is put into the local data of the returned
/// distributed array.
///@param begin_padding Padding used for the beginning parts of the local
/// subportions.
///@param end_padding Padding used for the end parts of the local subportions.
///@return DistributedArray of rank N and with same element type as the input
/// data.
///
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

///
///@brief Converts the input distributed array to an std::vector of same element
/// type by first gathering the possibly distributed data from all processes to
/// all processes. Preserves the topology of the input distributed array in the
/// returned vector.
///
///@param array The input array to convert to an std::vector.
///@return std::vector<T> A flat vector of global size of the distributed array
/// where the subportion data is gathered from all caller processes. Each caller
/// process gets the same data.
///
template <size_t N, class T>
std::vector<T> to_vector(const DistributedArray<N, T>& array) {

    auto data = all_gather(serialize_local(array));

    std::vector<T> global(data.size());

    std::vector<size_t> sizes;
    sizes.reserve(array.global_subdomain_count());

    for (int rank = 0; rank <= array.topology().get_max_rank(); ++rank) {

        auto boxes = array.topology().get_boxes(rank);
        for (auto box : boxes) {
            size_t size = flat_size(box.box.get_extent());
            sizes.emplace_back(size);
        }
    }

    std::vector<index_type> offsets(sizes.size(), 0);
    for (size_t i = 1; i < offsets.size(); ++i) {
        offsets[i] = offsets[i - 1] + index_type(sizes[i - 1]);
    }

    auto bigspan =
        make_span(global, array.topology().get_domain().get_extent());

    size_t j = 0;
    for (int rank = 0; rank <= array.topology().get_max_rank(); ++rank) {

        auto boxes = array.topology().get_boxes(rank);
        for (auto box : boxes) {

            auto o_span = make_subspan(bigspan, box.box.begin, box.box.end);
            T*   begin  = data.data() + offsets[j];
            span<T, N> i_span(begin, box.box.get_extent());
            transform(i_span, o_span, [](auto e) { return e; });
            box.rank = array.get_rank();
            ++j;
        }
    }

    return global;
}

/*
//TODO: This should return a Distributed array with all subportions converted to
local subportions. For some reason the topology information is not correctly
handled. template <size_t N, class T> auto reduce(const DistributedArray<N, T>&
array) {

    auto data = to_vector(array);

    auto new_topology = array.topology();

    for (auto& box : new_topology.get_boxes()) { box.rank = array.get_rank(); }

    return distribute(data,
                      new_topology,
                      array.get_rank(),
                      array.begin_padding(),
                      array.end_padding());

    // return global;
}
*/

/// @brief Applies the given function object f to every element of the input
/// array. The algorithm is executed according to policy (not necessarily in order).
/// @param policy the execution policy to use. See execution policy for details.
/// @param arr the input array.
/// @param f function object, to be applied to the result of subspan(md_idx).
template <class ExecutionPolicy, size_t N, class T, class UnaryFunction>
static inline void for_each(ExecutionPolicy&&       policy,
                            DistributedArray<N, T>& arr,
                            UnaryFunction           f) {

    for (auto span : make_subspans(arr)) { for_each(policy, span, f); }
}

/// @brief Applies the given function object f to every element of the input
/// array. Executed in order.
/// @param arr the input array.
/// @param f function object, to be applied to the result of subspan(md_idx).
template <size_t N, class T, class UnaryFunction>
static inline void for_each(DistributedArray<N, T>& arr, UnaryFunction f) {

    for_each(std::execution::seq, arr, f);
}

/// @brief Applies the given function object f(global_md_idx, value) to the
/// result of indexing every element of every subspan in the array (not
/// necessarily in order). Note! The md_idx given to the function object f is
/// the global index in the array topology. The algorithm is executed according
/// to policy.
/// @param policy the execution policy to use. See execution policy for details.
/// @param arr the input array.
/// @param f binary function object where the first argument is the current
/// (global) multidimensional index, to be applied to the result of
/// subspan(local_md_idx).
template <class ExecutionPolicy, size_t N, class T, class BinaryIndexFunction>
static inline void for_each_indexed(ExecutionPolicy&&       policy,
                                    DistributedArray<N, T>& arr,
                                    BinaryIndexFunction     f) {

    auto boxes    = arr.local_boxes();
    auto subspans = make_subspans(arr);
    for (size_t i = 0; i < subspans.size(); ++i) {
        auto offset = boxes[i].box.begin;
        auto span   = subspans[i];

        auto F = [=](auto md_idx) {
            const auto copy = elementwise_add(md_idx, offset);
            f(copy, span(md_idx));
        };
        detail::md_for_each(policy, all_indices(span), F);
    }
}

/// @brief Applies the given function object f(global_md_idx, value) to the
/// result of indexing every element of every subspan in the array
/// Note! The md_idx given to the function object f is the global index in the
/// array topology. Executed in order.
/// @param arr the input array.
/// @param f binary function object where the first argument is the current
/// (global) multidimensional index, to be applied to the result of
/// subspan(local_md_idx).
template <size_t N, class T, class BinaryIndexFunction>
static inline void for_each_indexed(DistributedArray<N, T>& arr,
                                    BinaryIndexFunction     f) {

    for_each_indexed(std::execution::seq, arr, f);
}

/// @brief Applies the given function to every element (not necessarily in
/// order) of the input distributed array and stores the result in the output
/// distributed array of same extent. Executed according to policy (not
/// necessarily in order).
/// @param policy the execution policy to use. See execution policy for details.
/// @param input the input array.
/// @param output the output array.
/// @param f the unary function object which should return a type corresponding
/// to the value_type of the output array.
template <class ExecutionPolicy,
          size_t N,
          class ET1,
          class ET2,
          class UnaryFunction>
static inline void transform(ExecutionPolicy&&               policy,
                             const DistributedArray<N, ET1>& input,
                             DistributedArray<N, ET2>&       output,
                             UnaryFunction                   f) {

    auto i_subspans = make_subspans(input);
    auto o_subspans = make_subspans(output);

    for (size_t i = 0; i < i_subspans.size(); ++i) {
        auto i_span = i_subspans[i];
        auto o_span = o_subspans[i];
        transform(policy, i_span, o_span, f);
    }
}

/// @brief Applies the given function f to every element of the input
/// distributed array and stores the result in the output distributed array of
/// same extent. Executed in order.
/// @param input the input array.
/// @param output the output array.
/// @param f the unary function object which should return a type corresponding
/// to the value_type of the output array.
template <size_t N, class ET1, class ET2, class UnaryWindowFunction>
static inline void transform(const DistributedArray<N, ET1>& input,
                             DistributedArray<N, ET2>&       output,
                             UnaryWindowFunction             f) {

    transform(std::execution::seq, input, output, f);
}

/// @brief Applies the given function f(global_md_idx, value) to every element
/// of the distributed array and stores the result to the output distributed
/// array of same extent. Note! The md_idx given to the function object f is the
/// global index in the array topology. Executed according to policy (not
/// necessarily in order).
/// @param policy the execution policy to use. See execution policy for details.
/// @param input the input array.
/// @param output the output array.
/// @param f the binary function object which should return a type corresponding
/// to the value_type of output and take a current multidimensional index as the
/// first argument.
template <class ExecutionPolicy,
          size_t N,
          class ET1,
          class ET2,
          class UnaryWindowFunction>
static inline void transform_indexed(ExecutionPolicy&&               policy,
                                     const DistributedArray<N, ET1>& input,
                                     DistributedArray<N, ET2>&       output,
                                     UnaryWindowFunction             f) {

    auto       i_subspans = make_subspans(input);
    auto       o_subspans = make_subspans(output);
    const auto boxes      = input.local_boxes();

    for (size_t i = 0; i < i_subspans.size(); ++i) {
        auto i_span = i_subspans[i];
        auto o_span = o_subspans[i];
        auto offset = boxes[i].box.begin;
        auto F      = [=](auto md_idx) {
            o_span(md_idx) = f(elementwise_add(md_idx, offset), i_span(md_idx));
        };

        detail::md_for_each(policy, all_indices(i_span), F);
    }
}

/// @brief Applies the given function f(global_md_idx, value) to every element
/// of the distributed array and stores the result to the output distributed
/// array of same extent. Note! The md_idx given to the function object f is the
/// global index in the array topology. Executed in order.
/// @param input the input array.
/// @param output the output array.
/// @param f the binary function object which should return a type corresponding
/// to the value_type of output and take a current multidimensional index as the
/// first argument.
template <size_t N, class ET1, class ET2, class UnaryWindowFunction>
static inline void transform_indexed(const DistributedArray<N, ET1>& input,
                                     DistributedArray<N, ET2>&       output,
                                     UnaryWindowFunction             f) {

    transform_indexed(std::execution::seq, input, output, f);
}

/// @brief Applies the input unary window function to all elements of the input
/// array and stores the result into the output array. A window accessor has the
/// same rank as the distributed arrays. Executed according to policy (not
/// necessarily in order).
/// @param policy the execution policy to use. See execution policy for details.
/// @param input the input array.
/// @param output the output array.
/// @param f the unary window operation. Example: f = [](auto accessor){return
/// accessor(1,0) + accessor(-1,0);};
template <class ExecutionPolicy,
          size_t N,
          class ET1,
          class ET2,
          class UnaryWindowFunction>
static inline void window_transform(ExecutionPolicy&&               policy,
                                    const DistributedArray<N, ET1>& input,
                                    DistributedArray<N, ET2>&       output,
                                    UnaryWindowFunction             f) {

    auto i_subspans = make_subspans(input);
    auto o_subspans = make_subspans(output);

    for (size_t i = 0; i < i_subspans.size(); ++i) {
        auto i_span = i_subspans[i];
        auto o_span = o_subspans[i];
        window_transform(policy, i_span, o_span, f);
    }
}

/// @brief Applies the input unary window function to all elements of the input
/// array and stores the result into the output array. A window accessor has the
/// same rank as the distributed arrays. Executed in order.
/// @param input the input array.
/// @param output the output array.
/// @param f the unary window operation. Example: f = [](auto accessor){return
/// accessor(1,0) + accessor(-1,0);};
template <size_t N, class ET1, class ET2, class UnaryWindowFunction>
static inline void window_transform(const DistributedArray<N, ET1>& input,
                                    DistributedArray<N, ET2>&       output,
                                    UnaryWindowFunction             f) {

    window_transform(std::execution::seq, input, output, f);
}

/// @brief Applies the input unary tile function f to all elements of the input
/// array and stores the result into the output array. A tile accessor is one
/// dimensional. Executed according to policy (not necessarily in order).
/// @tparam Dir the direction (index) along which the tile is created.
/// @param policy the execution policy to use. See execution policy for details.
/// @param input the input array.
/// @param output the output array.
/// @param f the unary tile operation. Example: f = [](auto accessor){return
/// accessor(0) + accessor(1);};
template <size_t Dir,
          class ExecutionPolicy,
          size_t N,
          class ET1,
          class ET2,
          class UnaryTileFunction>
static inline void tile_transform(ExecutionPolicy&&               policy,
                                  const DistributedArray<N, ET1>& input,
                                  DistributedArray<N, ET2>&       output,
                                  UnaryTileFunction               f) {

    auto i_subspans = make_subspans(input);
    auto o_subspans = make_subspans(output);

    for (size_t i = 0; i < i_subspans.size(); ++i) {
        auto i_span = i_subspans[i];
        auto o_span = o_subspans[i];

        tile_transform<Dir>(policy, i_span, o_span, f);
    }
}

/// @brief Applies the input unary tile function f to all elements of the input
/// array and stores the result into the output array. A tile accessor is one
/// dimensional. Executed in order.
/// @tparam Dir the direction (index) along which the tile is created.
/// @param input the input array.
/// @param output the output array.
/// @param f the unary tile operation. Example: f = [](auto accessor){return
/// accessor(0) + accessor(1);};
template <size_t Dir, size_t N, class ET1, class ET2, class UnaryTileFunction>
static inline void tile_transform(const DistributedArray<N, ET1>& input,
                                  DistributedArray<N, ET2>&       output,
                                  UnaryTileFunction               f) {

    tile_transform<Dir>(std::execution::seq, input, output, f);
}


} // namespace jada
