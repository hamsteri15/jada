#pragma once

#include <array>
#include <sstream>

#include "include/bits/core/integer_types.hpp"

namespace jada{

template <size_t N> struct TransferInfo {

    int                       sender_rank;
    int                       receiver_rank;
    std::array<index_type, N> sender_begin;
    std::array<index_type, N> receiver_begin;
    std::array<size_type, N>  extent;

    bool operator==(const TransferInfo<N>& rhs) const = default;
    bool operator!=(const TransferInfo<N>& rhs) const = default;

    bool operator<(const TransferInfo<N>& rhs) const {
        // TODO: dont do string comparisons
        std::stringstream s1;
        s1 << *this;
        std::stringstream s2;
        s2 << rhs;

        return s1.str() < s2.str();
    }

    template <size_t L>
    friend std::ostream& operator<<(std::ostream& os, const TransferInfo<L>& t);
};

template <size_t L>
std::ostream& operator<<(std::ostream& os, const TransferInfo<L>& t) {

    os << "Sender: " << t.sender_rank;
    os << " Receiver: " << t.receiver_rank;
    os << " Sender begin: ";
    for (auto e : t.sender_begin) { os << e << " "; }

    os << " Receiver begin: ";
    for (auto e : t.receiver_begin) { os << e << " "; }

    os << " Extent: ";
    for (auto e : t.extent) { os << e << " "; }

    return os;
}


}