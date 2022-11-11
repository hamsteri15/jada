#pragma once

#include <cstddef>

namespace jada {

// TODO: this should be a concept
template <class Derived> struct TiledStencil {
    static constexpr size_t padding = Derived::padding;
};

} // namespace jada