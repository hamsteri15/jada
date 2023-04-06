#pragma once

#include <cstddef>
#include <array>

namespace jada{

    using index_type = int;
    using size_type = std::size_t;


    template<size_type N>
    using md_idx = std::array<index_type, N>;

}