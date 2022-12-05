#pragma once

//#include <range/v3/view/indices.hpp>

#include <cstddef>
#include <vector>

namespace jada{

    using index_type = std::ptrdiff_t;
    using size_type = std::size_t;

}
#include <ranges>
namespace jada {

///
/// @brief Wrapper around ranges::indices
///
/// @tparam T models integer
/// @param begin integer type begin index
/// @param end interger type end index
/// @return one-dimensional view of indices [begin, end)
///
template <class T> static constexpr auto indices(T&& begin, T&& end) {
    // TODO: No idea why the int conversion is required here. If it is removed,
    // everything just breaks.
    return std::ranges::views::iota(int(begin), int(end));
}

} // namespace jada

#include <array>
#include <stdexcept> //std::runtime_error
#include <string>
#include <tuple>
#include <type_traits>

namespace jada {

// Trait to type check all types of a std::tuple
template <template <typename, typename...> class Trait, typename Tuple>
struct all_of;

template <template <typename, typename...> class Trait, typename... Types>
struct all_of<Trait, std::tuple<Types...>> : std::conjunction<Trait<Types>...> {
};

template <template <typename, typename...> class Trait, typename... Types>
struct all_of<Trait, std::pair<Types...>> : std::conjunction<Trait<Types>...> {
};

#ifdef DEBUG
constexpr void runtime_assert(bool condition, const char* msg) {
    if (!condition) throw std::runtime_error(msg);
}
#else
constexpr void runtime_assert([[maybe_unused]] bool        condition,
                              [[maybe_unused]] const char* msg) {}
#endif

/// @brief Converts all tuple elements to an array of elements of the same size
/// @tparam tuple_t the type of the tuple to convert
/// @param tuple the input tuple to convert
/// @return std::array of the tuple elements
template <typename tuple_t> constexpr auto tuple_to_array(tuple_t&& tuple) {

    constexpr auto get_array = [](auto&&... x) constexpr {
        return std::array{std::forward<decltype(x)>(x)...};
    };
    return std::apply(get_array, std::forward<tuple_t>(tuple));
}

} // namespace jada

#include <array>
#include <cstddef>

namespace jada {

template <class T> struct Rank { static constexpr size_t value = T::rank(); };

template <size_t N, class Idx> struct Rank<std::array<Idx, N>> {
    static_assert(std::is_integral<Idx>::value, "Not an integer array.");
    static constexpr size_t value = N;
};

template <class... Idx> struct Rank<std::tuple<Idx...>> {

private:
    using my_type = std::tuple<Idx...>;

public:
    static_assert(all_of<std::is_integral, my_type>::value,
                  "Not an integer tuple.");
    static constexpr size_t value = sizeof...(Idx);
};

template <class... Idx> struct Rank<std::pair<Idx...>> {

private:
    using my_type = std::pair<Idx...>;

public:
    static_assert(all_of<std::is_integral, my_type>::value,
                  "Not an integer pair.");
    static constexpr size_t value = 2;
};

/*
template <class... Idx> struct Rank<ranges::common_tuple<Idx...>> {

private:
    using my_type = std::tuple<Idx...>;

public:
    // static_assert(all_of<std::is_integral, my_type>::value, "Not an integer
tuple."); static constexpr size_t value = sizeof...(Idx);
};
*/

/// @brief Queries the _static_ rank of the input type T
/// @tparam T the type to query the static rank of_
/// @return static rank of the input type T
template <class T> constexpr size_t rank(const T&) { return Rank<T>::value; }

} // namespace jada

#pragma GCC system_header
#ifndef _MDSPAN_SINGLE_HEADER_INCLUDE_GUARD_
#define _MDSPAN_SINGLE_HEADER_INCLUDE_GUARD_
#  include <version>
#  include <type_traits>
#  include <utility>
#include <type_traits> // std::is_void
#include <cstddef> // size_t
#include <type_traits>
#include <utility> // integer_sequence
#include <cstddef>  // size_t
#include <limits>   // numeric_limits
#include <array>
#include <span>
#include <utility> // integer_sequence
#include <cstddef>
#include <cstddef> // size_t
#include <utility> // integer_sequence
#include <array>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <array>
#include <span>
#include<concepts>
#include <tuple> // std::apply
#include <utility> // std::pair
#include <cassert>
#include <vector>

//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/mdarray
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/mdspan
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/default_accessor.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/macros.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/config.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#ifndef __has_include
#  define __has_include(x) 0
#endif

#if __has_include(<version>)
#else
#endif

#ifdef _MSVC_LANG
#define _MDSPAN_CPLUSPLUS _MSVC_LANG
#else
#define _MDSPAN_CPLUSPLUS __cplusplus
#endif

#define MDSPAN_CXX_STD_14 201402L
#define MDSPAN_CXX_STD_17 201703L
#define MDSPAN_CXX_STD_20 202002L

#define MDSPAN_HAS_CXX_14 (_MDSPAN_CPLUSPLUS >= MDSPAN_CXX_STD_14)
#define MDSPAN_HAS_CXX_17 (_MDSPAN_CPLUSPLUS >= MDSPAN_CXX_STD_17)
#define MDSPAN_HAS_CXX_20 (_MDSPAN_CPLUSPLUS >= MDSPAN_CXX_STD_20)

static_assert(_MDSPAN_CPLUSPLUS >= MDSPAN_CXX_STD_14, "mdspan requires C++14 or later.");

#ifndef _MDSPAN_COMPILER_CLANG
#  if defined(__clang__)
#    define _MDSPAN_COMPILER_CLANG __clang__
#  endif
#endif

#if !defined(_MDSPAN_COMPILER_MSVC) && !defined(_MDSPAN_COMPILER_MSVC_CLANG)
#  if defined(_MSC_VER)
#    if !defined(_MDSPAN_COMPILER_CLANG)
#      define _MDSPAN_COMPILER_MSVC _MSC_VER
#    else
#      define _MDSPAN_COMPILER_MSVC_CLANG _MSC_VER
#    endif
#  endif
#endif

#ifndef _MDSPAN_COMPILER_INTEL
#  ifdef __INTEL_COMPILER
#    define _MDSPAN_COMPILER_INTEL __INTEL_COMPILER
#  endif
#endif

#ifndef _MDSPAN_COMPILER_APPLECLANG
#  ifdef __apple_build_version__
#    define _MDSPAN_COMPILER_APPLECLANG __apple_build_version__
#  endif
#endif

#ifndef _MDSPAN_HAS_CUDA
#  if defined(__CUDACC__)
#    define _MDSPAN_HAS_CUDA __CUDACC__
#  endif
#endif

#ifndef _MDSPAN_HAS_HIP
#  if defined(__HIPCC__)
#    define _MDSPAN_HAS_HIP __HIPCC__
#  endif
#endif

#ifndef __has_cpp_attribute
#  define __has_cpp_attribute(x) 0
#endif

#ifndef _MDSPAN_PRESERVE_STANDARD_LAYOUT
// Preserve standard layout by default, but we're not removing the old version
// that turns this off until we're sure this doesn't have an unreasonable cost
// to the compiler or optimizer.
#  define _MDSPAN_PRESERVE_STANDARD_LAYOUT 1
#endif

#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
#  if ((__has_cpp_attribute(no_unique_address) >= 201803L) && \
       (!defined(__NVCC__) || MDSPAN_HAS_CXX_20) && \
       (!defined(_MDSPAN_COMPILER_MSVC) || MDSPAN_HAS_CXX_20))
#    define _MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS 1
#    define _MDSPAN_NO_UNIQUE_ADDRESS [[no_unique_address]]
#  else
#    define _MDSPAN_NO_UNIQUE_ADDRESS
#  endif
#endif

// NVCC older than 11.6 chokes on the no-unique-address-emulation
// so just pretend to use it (to avoid the full blown EBO workaround
// which NVCC also doesn't like ...), and leave the macro empty
#ifndef _MDSPAN_NO_UNIQUE_ADDRESS
#  if defined(__NVCC__)
#    define _MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS 1
#    define _MDSPAN_USE_FAKE_ATTRIBUTE_NO_UNIQUE_ADDRESS
#  endif
#  define _MDSPAN_NO_UNIQUE_ADDRESS
#endif

#ifndef _MDSPAN_USE_CONCEPTS
#  if defined(__cpp_concepts) && __cpp_concepts >= 201507L
#    define _MDSPAN_USE_CONCEPTS 1
#  endif
#endif

#ifndef _MDSPAN_USE_FOLD_EXPRESSIONS
#  if (defined(__cpp_fold_expressions) && __cpp_fold_expressions >= 201603L) \
          || (!defined(__cpp_fold_expressions) && MDSPAN_HAS_CXX_17)
#    define _MDSPAN_USE_FOLD_EXPRESSIONS 1
#  endif
#endif

#ifndef _MDSPAN_USE_INLINE_VARIABLES
#  if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L \
         || (!defined(__cpp_inline_variables) && MDSPAN_HAS_CXX_17)
#    define _MDSPAN_USE_INLINE_VARIABLES 1
#  endif
#endif

#ifndef _MDSPAN_NEEDS_TRAIT_VARIABLE_TEMPLATE_BACKPORTS
#  if (!(defined(__cpp_lib_type_trait_variable_templates) && __cpp_lib_type_trait_variable_templates >= 201510L) \
          || !MDSPAN_HAS_CXX_17)
#    if !(defined(_MDSPAN_COMPILER_APPLECLANG) && MDSPAN_HAS_CXX_17)
#      define _MDSPAN_NEEDS_TRAIT_VARIABLE_TEMPLATE_BACKPORTS 1
#    endif
#  endif
#endif

#ifndef _MDSPAN_USE_VARIABLE_TEMPLATES
#  if (defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304 && MDSPAN_HAS_CXX_17) \
        || (!defined(__cpp_variable_templates) && MDSPAN_HAS_CXX_17)
#    define _MDSPAN_USE_VARIABLE_TEMPLATES 1
#  endif
#endif // _MDSPAN_USE_VARIABLE_TEMPLATES

#ifndef _MDSPAN_USE_CONSTEXPR_14
#  if (defined(__cpp_constexpr) && __cpp_constexpr >= 201304) \
        || (!defined(__cpp_constexpr) && MDSPAN_HAS_CXX_14) \
        && (!(defined(__INTEL_COMPILER) && __INTEL_COMPILER <= 1700))
#    define _MDSPAN_USE_CONSTEXPR_14 1
#  endif
#endif

#ifndef _MDSPAN_USE_INTEGER_SEQUENCE
#  if defined(_MDSPAN_COMPILER_MSVC)
#    if (defined(__cpp_lib_integer_sequence) && __cpp_lib_integer_sequence >= 201304)
#      define _MDSPAN_USE_INTEGER_SEQUENCE 1
#    endif
#  endif
#endif
#ifndef _MDSPAN_USE_INTEGER_SEQUENCE
#  if (defined(__cpp_lib_integer_sequence) && __cpp_lib_integer_sequence >= 201304) \
        || (!defined(__cpp_lib_integer_sequence) && MDSPAN_HAS_CXX_14) \
        /* as far as I can tell, libc++ seems to think this is a C++11 feature... */ \
        || (defined(__GLIBCXX__) && __GLIBCXX__ > 20150422 && __GNUC__ < 5 && !defined(__INTEL_CXX11_MODE__))
     // several compilers lie about integer_sequence working properly unless the C++14 standard is used
#    define _MDSPAN_USE_INTEGER_SEQUENCE 1
#  elif defined(_MDSPAN_COMPILER_APPLECLANG) && MDSPAN_HAS_CXX_14
     // appleclang seems to be missing the __cpp_lib_... macros, but doesn't seem to lie about C++14 making
     // integer_sequence work
#    define _MDSPAN_USE_INTEGER_SEQUENCE 1
#  endif
#endif

#ifndef _MDSPAN_USE_RETURN_TYPE_DEDUCTION
#  if (defined(__cpp_return_type_deduction) && __cpp_return_type_deduction >= 201304) \
          || (!defined(__cpp_return_type_deduction) && MDSPAN_HAS_CXX_14)
#    define _MDSPAN_USE_RETURN_TYPE_DEDUCTION 1
#  endif
#endif

#ifndef _MDSPAN_USE_CLASS_TEMPLATE_ARGUMENT_DEDUCTION
// GCC 10's CTAD seems sufficiently broken to prevent its use.
#  if (defined(_MDSPAN_COMPILER_CLANG) || !defined(__GNUC__) || __GNUC__ >= 11) \
      && ((defined(__cpp_deduction_guides) && __cpp_deduction_guides >= 201703) \
         || (!defined(__cpp_deduction_guides) && MDSPAN_HAS_CXX_17))
#    define _MDSPAN_USE_CLASS_TEMPLATE_ARGUMENT_DEDUCTION 1
#  endif
#endif

#ifndef _MDSPAN_USE_ALIAS_TEMPLATE_ARGUMENT_DEDUCTION
// GCC 10's CTAD seems sufficiently broken to prevent its use.
#  if (defined(_MDSPAN_COMPILER_CLANG) || !defined(__GNUC__) || __GNUC__ >= 11) \
      && ((defined(__cpp_deduction_guides) && __cpp_deduction_guides >= 201907) \
          || (!defined(__cpp_deduction_guides) && MDSPAN_HAS_CXX_20))
#    define _MDSPAN_USE_ALIAS_TEMPLATE_ARGUMENT_DEDUCTION 1
#  endif
#endif

#ifndef _MDSPAN_USE_STANDARD_TRAIT_ALIASES
#  if (defined(__cpp_lib_transformation_trait_aliases) && __cpp_lib_transformation_trait_aliases >= 201304) \
          || (!defined(__cpp_lib_transformation_trait_aliases) && MDSPAN_HAS_CXX_14)
#    define _MDSPAN_USE_STANDARD_TRAIT_ALIASES 1
#  elif defined(_MDSPAN_COMPILER_APPLECLANG) && MDSPAN_HAS_CXX_14
     // appleclang seems to be missing the __cpp_lib_... macros, but doesn't seem to lie about C++14
#    define _MDSPAN_USE_STANDARD_TRAIT_ALIASES 1
#  endif
#endif

#ifndef _MDSPAN_DEFAULTED_CONSTRUCTORS_INHERITANCE_WORKAROUND
#  ifdef __GNUC__
#    if __GNUC__ < 9
#      define _MDSPAN_DEFAULTED_CONSTRUCTORS_INHERITANCE_WORKAROUND 1
#    endif
#  endif
#endif

#ifndef MDSPAN_CONDITIONAL_EXPLICIT
#  if MDSPAN_HAS_CXX_20 && !defined(_MDSPAN_COMPILER_MSVC)
#    define MDSPAN_CONDITIONAL_EXPLICIT(COND) explicit(COND)
#  else
#    define MDSPAN_CONDITIONAL_EXPLICIT(COND)
#  endif
#endif

#ifndef MDSPAN_USE_BRACKET_OPERATOR
#  if defined(__cpp_multidimensional_subscript)
#    define MDSPAN_USE_BRACKET_OPERATOR 1
#  else
#    define MDSPAN_USE_BRACKET_OPERATOR 0
#  endif
#endif

#ifndef MDSPAN_USE_PAREN_OPERATOR
#  if !MDSPAN_USE_BRACKET_OPERATOR
#    define MDSPAN_USE_PAREN_OPERATOR 1
#  else
#    define MDSPAN_USE_PAREN_OPERATOR 0
#  endif
#endif

#if MDSPAN_USE_BRACKET_OPERATOR
#  define __MDSPAN_OP(mds,...) mds[__VA_ARGS__]
// Corentins demo compiler for subscript chokes on empty [] call,
// though I believe the proposal supports it?
#ifdef MDSPAN_NO_EMPTY_BRACKET_OPERATOR
#  define __MDSPAN_OP0(mds) mds.accessor().access(mds.data_handle(),0)
#else
#  define __MDSPAN_OP0(mds) mds[]
#endif
#  define __MDSPAN_OP1(mds, a) mds[a]
#  define __MDSPAN_OP2(mds, a, b) mds[a,b]
#  define __MDSPAN_OP3(mds, a, b, c) mds[a,b,c]
#  define __MDSPAN_OP4(mds, a, b, c, d) mds[a,b,c,d]
#  define __MDSPAN_OP5(mds, a, b, c, d, e) mds[a,b,c,d,e]
#  define __MDSPAN_OP6(mds, a, b, c, d, e, f) mds[a,b,c,d,e,f]
#else
#  define __MDSPAN_OP(mds,...) mds(__VA_ARGS__)
#  define __MDSPAN_OP0(mds) mds()
#  define __MDSPAN_OP1(mds, a) mds(a)
#  define __MDSPAN_OP2(mds, a, b) mds(a,b)
#  define __MDSPAN_OP3(mds, a, b, c) mds(a,b,c)
#  define __MDSPAN_OP4(mds, a, b, c, d) mds(a,b,c,d)
#  define __MDSPAN_OP5(mds, a, b, c, d, e) mds(a,b,c,d,e)
#  define __MDSPAN_OP6(mds, a, b, c, d, e, f) mds(a,b,c,d,e,f)
#endif
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/config.hpp

#ifndef _MDSPAN_HOST_DEVICE
#  if defined(_MDSPAN_HAS_CUDA) || defined(_MDSPAN_HAS_HIP)
#    define _MDSPAN_HOST_DEVICE __host__ __device__
#  else
#    define _MDSPAN_HOST_DEVICE
#  endif
#endif

#ifndef MDSPAN_FORCE_INLINE_FUNCTION
#  ifdef _MDSPAN_COMPILER_MSVC // Microsoft compilers
#    define MDSPAN_FORCE_INLINE_FUNCTION __forceinline _MDSPAN_HOST_DEVICE
#  else
#    define MDSPAN_FORCE_INLINE_FUNCTION __attribute__((always_inline)) _MDSPAN_HOST_DEVICE
#  endif
#endif

#ifndef MDSPAN_INLINE_FUNCTION
#  define MDSPAN_INLINE_FUNCTION inline _MDSPAN_HOST_DEVICE
#endif

// In CUDA defaulted functions do not need host device markup
#ifndef MDSPAN_INLINE_FUNCTION_DEFAULTED
#  define MDSPAN_INLINE_FUNCTION_DEFAULTED
#endif

//==============================================================================
// <editor-fold desc="Preprocessor helpers"> {{{1

#define MDSPAN_PP_COUNT(...) \
  _MDSPAN_PP_INTERNAL_EXPAND_ARGS_PRIVATE( \
    _MDSPAN_PP_INTERNAL_ARGS_AUGMENTER(__VA_ARGS__) \
  )

#define _MDSPAN_PP_INTERNAL_ARGS_AUGMENTER(...) unused, __VA_ARGS__
#define _MDSPAN_PP_INTERNAL_EXPAND(x) x
#define _MDSPAN_PP_INTERNAL_EXPAND_ARGS_PRIVATE(...) \
  _MDSPAN_PP_INTERNAL_EXPAND( \
    _MDSPAN_PP_INTERNAL_COUNT_PRIVATE( \
      __VA_ARGS__, 69, 68, 67, 66, 65, 64, 63, 62, 61, \
      60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49,  \
      48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37,  \
      36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25,  \
      24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13,  \
      12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 \
    ) \
  )
# define _MDSPAN_PP_INTERNAL_COUNT_PRIVATE( \
         _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, \
    _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, \
    _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, \
    _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, \
    _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, \
    _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, \
    _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, \
    _70, count, ...) count \
    /**/

#define MDSPAN_PP_STRINGIFY_IMPL(x) #x
#define MDSPAN_PP_STRINGIFY(x) MDSPAN_PP_STRINGIFY_IMPL(x)

#define MDSPAN_PP_CAT_IMPL(x, y) x ## y
#define MDSPAN_PP_CAT(x, y) MDSPAN_PP_CAT_IMPL(x, y)

#define MDSPAN_PP_EVAL(X, ...) X(__VA_ARGS__)

#define MDSPAN_PP_REMOVE_PARENS_IMPL(...) __VA_ARGS__
#define MDSPAN_PP_REMOVE_PARENS(...) MDSPAN_PP_REMOVE_PARENS_IMPL __VA_ARGS__

// </editor-fold> end Preprocessor helpers }}}1
//==============================================================================

//==============================================================================
// <editor-fold desc="Concept emulation"> {{{1

// These compatibility macros don't help with partial ordering, but they should do the trick
// for what we need to do with concepts in mdspan
#ifdef _MDSPAN_USE_CONCEPTS
#  define MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) > requires REQ
#  define MDSPAN_FUNCTION_REQUIRES(PAREN_PREQUALS, FNAME, PAREN_PARAMS, QUALS, REQ) \
     MDSPAN_PP_REMOVE_PARENS(PAREN_PREQUALS) FNAME PAREN_PARAMS QUALS requires REQ \
     /**/
#else
#  define MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) , typename ::std::enable_if<(REQ), int>::type = 0>
#  define MDSPAN_FUNCTION_REQUIRES(PAREN_PREQUALS, FNAME, PAREN_PARAMS, QUALS, REQ) \
     MDSPAN_TEMPLATE_REQUIRES( \
       class __function_requires_ignored=void, \
       (std::is_void<__function_requires_ignored>::value && REQ) \
     ) MDSPAN_PP_REMOVE_PARENS(PAREN_PREQUALS) FNAME PAREN_PARAMS QUALS \
     /**/
#endif

#if defined(_MDSPAN_COMPILER_MSVC)
#  define MDSPAN_TEMPLATE_REQUIRES(...) \
      MDSPAN_PP_CAT( \
        MDSPAN_PP_CAT(MDSPAN_TEMPLATE_REQUIRES_, MDSPAN_PP_COUNT(__VA_ARGS__))\
        (__VA_ARGS__), \
      ) \
    /**/
#else
#  define MDSPAN_TEMPLATE_REQUIRES(...) \
    MDSPAN_PP_EVAL( \
        MDSPAN_PP_CAT(MDSPAN_TEMPLATE_REQUIRES_, MDSPAN_PP_COUNT(__VA_ARGS__)), \
        __VA_ARGS__ \
    ) \
    /**/
#endif

#define MDSPAN_TEMPLATE_REQUIRES_2(TP1, REQ) \
  template<TP1 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_3(TP1, TP2, REQ) \
  template<TP1, TP2 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_4(TP1, TP2, TP3, REQ) \
  template<TP1, TP2, TP3 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_5(TP1, TP2, TP3, TP4, REQ) \
  template<TP1, TP2, TP3, TP4 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_6(TP1, TP2, TP3, TP4, TP5, REQ) \
  template<TP1, TP2, TP3, TP4, TP5 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_7(TP1, TP2, TP3, TP4, TP5, TP6, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_8(TP1, TP2, TP3, TP4, TP5, TP6, TP7, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6, TP7 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_9(TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_10(TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_11(TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_12(TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_13(TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_14(TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_15(TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_16(TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14, TP15, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14, TP15 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_17(TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14, TP15, TP16, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14, TP15, TP16 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_18(TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14, TP15, TP16, TP17, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14, TP15, TP16, TP17 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_19(TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14, TP15, TP16, TP17, TP18, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14, TP15, TP16, TP17, TP18 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/
#define MDSPAN_TEMPLATE_REQUIRES_20(TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14, TP15, TP16, TP17, TP18, TP19, REQ) \
  template<TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14, TP15, TP16, TP17, TP18, TP19 \
    MDSPAN_CLOSE_ANGLE_REQUIRES(REQ) \
    /**/

#define MDSPAN_INSTANTIATE_ONLY_IF_USED \
  MDSPAN_TEMPLATE_REQUIRES( \
    class __instantiate_only_if_used_tparam=void, \
    ( _MDSPAN_TRAIT(std::is_void, __instantiate_only_if_used_tparam) ) \
  ) \
  /**/

// </editor-fold> end Concept emulation }}}1
//==============================================================================

//==============================================================================
// <editor-fold desc="inline variables"> {{{1

#ifdef _MDSPAN_USE_INLINE_VARIABLES
#  define _MDSPAN_INLINE_VARIABLE inline
#else
#  define _MDSPAN_INLINE_VARIABLE
#endif

// </editor-fold> end inline variables }}}1
//==============================================================================

//==============================================================================
// <editor-fold desc="Return type deduction"> {{{1

#if _MDSPAN_USE_RETURN_TYPE_DEDUCTION
#  define _MDSPAN_DEDUCE_RETURN_TYPE_SINGLE_LINE(SIGNATURE, BODY) \
    auto MDSPAN_PP_REMOVE_PARENS(SIGNATURE) { return MDSPAN_PP_REMOVE_PARENS(BODY); }
#  define _MDSPAN_DEDUCE_DECLTYPE_AUTO_RETURN_TYPE_SINGLE_LINE(SIGNATURE, BODY) \
    decltype(auto) MDSPAN_PP_REMOVE_PARENS(SIGNATURE) { return MDSPAN_PP_REMOVE_PARENS(BODY); }
#else
#  define _MDSPAN_DEDUCE_RETURN_TYPE_SINGLE_LINE(SIGNATURE, BODY) \
    auto MDSPAN_PP_REMOVE_PARENS(SIGNATURE) \
      -> std::remove_cv_t<std::remove_reference_t<decltype(BODY)>> \
    { return MDSPAN_PP_REMOVE_PARENS(BODY); }
#  define _MDSPAN_DEDUCE_DECLTYPE_AUTO_RETURN_TYPE_SINGLE_LINE(SIGNATURE, BODY) \
    auto MDSPAN_PP_REMOVE_PARENS(SIGNATURE) \
      -> decltype(BODY) \
    { return MDSPAN_PP_REMOVE_PARENS(BODY); }

#endif

// </editor-fold> end Return type deduction }}}1
//==============================================================================

//==============================================================================
// <editor-fold desc="fold expressions"> {{{1

struct __mdspan_enable_fold_comma { };

#ifdef _MDSPAN_USE_FOLD_EXPRESSIONS
#  define _MDSPAN_FOLD_AND(...) ((__VA_ARGS__) && ...)
#  define _MDSPAN_FOLD_AND_TEMPLATE(...) ((__VA_ARGS__) && ...)
#  define _MDSPAN_FOLD_OR(...) ((__VA_ARGS__) || ...)
#  define _MDSPAN_FOLD_ASSIGN_LEFT(INIT, ...) (INIT = ... = (__VA_ARGS__))
#  define _MDSPAN_FOLD_ASSIGN_RIGHT(PACK, ...) (PACK = ... = (__VA_ARGS__))
#  define _MDSPAN_FOLD_TIMES_RIGHT(PACK, ...) (PACK * ... * (__VA_ARGS__))
#  define _MDSPAN_FOLD_PLUS_RIGHT(PACK, ...) (PACK + ... + (__VA_ARGS__))
#  define _MDSPAN_FOLD_COMMA(...) ((__VA_ARGS__), ...)
#else

namespace std {

namespace __fold_compatibility_impl {

// We could probably be more clever here, but at the (small) risk of losing some compiler understanding.  For the
// few operations we need, it's not worth generalizing over the operation

#if _MDSPAN_USE_RETURN_TYPE_DEDUCTION

MDSPAN_FORCE_INLINE_FUNCTION
constexpr decltype(auto) __fold_right_and_impl() {
  return true;
}

template <class Arg, class... Args>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr decltype(auto) __fold_right_and_impl(Arg&& arg, Args&&... args) {
  return ((Arg&&)arg) && __fold_compatibility_impl::__fold_right_and_impl((Args&&)args...);
}

MDSPAN_FORCE_INLINE_FUNCTION
constexpr decltype(auto) __fold_right_or_impl() {
  return false;
}

template <class Arg, class... Args>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr auto __fold_right_or_impl(Arg&& arg, Args&&... args) {
  return ((Arg&&)arg) || __fold_compatibility_impl::__fold_right_or_impl((Args&&)args...);
}

template <class Arg1>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr auto __fold_left_assign_impl(Arg1&& arg1) {
  return (Arg1&&)arg1;
}

template <class Arg1, class Arg2, class... Args>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr auto __fold_left_assign_impl(Arg1&& arg1, Arg2&& arg2, Args&&... args) {
  return __fold_compatibility_impl::__fold_left_assign_impl((((Arg1&&)arg1) = ((Arg2&&)arg2)), (Args&&)args...);
}

template <class Arg1>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr auto __fold_right_assign_impl(Arg1&& arg1) {
  return (Arg1&&)arg1;
}

template <class Arg1, class Arg2, class... Args>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr auto __fold_right_assign_impl(Arg1&& arg1, Arg2&& arg2,  Args&&... args) {
  return ((Arg1&&)arg1) = __fold_compatibility_impl::__fold_right_assign_impl((Arg2&&)arg2, (Args&&)args...);
}

template <class Arg1>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr auto __fold_right_plus_impl(Arg1&& arg1) {
  return (Arg1&&)arg1;
}

template <class Arg1, class Arg2, class... Args>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr auto __fold_right_plus_impl(Arg1&& arg1, Arg2&& arg2, Args&&... args) {
  return ((Arg1&&)arg1) + __fold_compatibility_impl::__fold_right_plus_impl((Arg2&&)arg2, (Args&&)args...);
}

template <class Arg1>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr auto __fold_right_times_impl(Arg1&& arg1) {
  return (Arg1&&)arg1;
}

template <class Arg1, class Arg2, class... Args>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr auto __fold_right_times_impl(Arg1&& arg1, Arg2&& arg2, Args&&... args) {
  return ((Arg1&&)arg1) * __fold_compatibility_impl::__fold_right_times_impl((Arg2&&)arg2, (Args&&)args...);
}

#else

//------------------------------------------------------------------------------
// <editor-fold desc="right and"> {{{2

template <class... Args>
struct __fold_right_and_impl_;
template <>
struct __fold_right_and_impl_<> {
  using __rv = bool;
  MDSPAN_FORCE_INLINE_FUNCTION
  static constexpr __rv
  __impl() noexcept {
    return true;
  }
};
template <class Arg, class... Args>
struct __fold_right_and_impl_<Arg, Args...> {
  using __next_t = __fold_right_and_impl_<Args...>;
  using __rv = decltype(std::declval<Arg>() && std::declval<typename __next_t::__rv>());
  MDSPAN_FORCE_INLINE_FUNCTION
  static constexpr __rv
  __impl(Arg&& arg, Args&&... args) noexcept {
    return ((Arg&&)arg) && __next_t::__impl((Args&&)args...);
  }
};

template <class... Args>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr typename __fold_right_and_impl_<Args...>::__rv
__fold_right_and_impl(Args&&... args) {
  return __fold_right_and_impl_<Args...>::__impl((Args&&)args...);
}

// </editor-fold> end right and }}}2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// <editor-fold desc="right or"> {{{2

template <class... Args>
struct __fold_right_or_impl_;
template <>
struct __fold_right_or_impl_<> {
  using __rv = bool;
  MDSPAN_FORCE_INLINE_FUNCTION
  static constexpr __rv
  __impl() noexcept {
    return false;
  }
};
template <class Arg, class... Args>
struct __fold_right_or_impl_<Arg, Args...> {
  using __next_t = __fold_right_or_impl_<Args...>;
  using __rv = decltype(std::declval<Arg>() || std::declval<typename __next_t::__rv>());
  MDSPAN_FORCE_INLINE_FUNCTION
  static constexpr __rv
  __impl(Arg&& arg, Args&&... args) noexcept {
    return ((Arg&&)arg) || __next_t::__impl((Args&&)args...);
  }
};

template <class... Args>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr typename __fold_right_or_impl_<Args...>::__rv
__fold_right_or_impl(Args&&... args) {
  return __fold_right_or_impl_<Args...>::__impl((Args&&)args...);
}

// </editor-fold> end right or }}}2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// <editor-fold desc="right plus"> {{{2

template <class... Args>
struct __fold_right_plus_impl_;
template <class Arg>
struct __fold_right_plus_impl_<Arg> {
  using __rv = Arg&&;
  MDSPAN_FORCE_INLINE_FUNCTION
  static constexpr __rv
  __impl(Arg&& arg) noexcept {
    return (Arg&&)arg;
  }
};
template <class Arg1, class Arg2, class... Args>
struct __fold_right_plus_impl_<Arg1, Arg2, Args...> {
  using __next_t = __fold_right_plus_impl_<Arg2, Args...>;
  using __rv = decltype(std::declval<Arg1>() + std::declval<typename __next_t::__rv>());
  MDSPAN_FORCE_INLINE_FUNCTION
  static constexpr __rv
  __impl(Arg1&& arg, Arg2&& arg2, Args&&... args) noexcept {
    return ((Arg1&&)arg) + __next_t::__impl((Arg2&&)arg2, (Args&&)args...);
  }
};

template <class... Args>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr typename __fold_right_plus_impl_<Args...>::__rv
__fold_right_plus_impl(Args&&... args) {
  return __fold_right_plus_impl_<Args...>::__impl((Args&&)args...);
}

// </editor-fold> end right plus }}}2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// <editor-fold desc="right times"> {{{2

template <class... Args>
struct __fold_right_times_impl_;
template <class Arg>
struct __fold_right_times_impl_<Arg> {
  using __rv = Arg&&;
  MDSPAN_FORCE_INLINE_FUNCTION
  static constexpr __rv
  __impl(Arg&& arg) noexcept {
    return (Arg&&)arg;
  }
};
template <class Arg1, class Arg2, class... Args>
struct __fold_right_times_impl_<Arg1, Arg2, Args...> {
  using __next_t = __fold_right_times_impl_<Arg2, Args...>;
  using __rv = decltype(std::declval<Arg1>() * std::declval<typename __next_t::__rv>());
  MDSPAN_FORCE_INLINE_FUNCTION
  static constexpr __rv
  __impl(Arg1&& arg, Arg2&& arg2, Args&&... args) noexcept {
    return ((Arg1&&)arg) * __next_t::__impl((Arg2&&)arg2, (Args&&)args...);
  }
};

template <class... Args>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr typename __fold_right_times_impl_<Args...>::__rv
__fold_right_times_impl(Args&&... args) {
  return __fold_right_times_impl_<Args...>::__impl((Args&&)args...);
}

// </editor-fold> end right times }}}2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// <editor-fold desc="right assign"> {{{2

template <class... Args>
struct __fold_right_assign_impl_;
template <class Arg>
struct __fold_right_assign_impl_<Arg> {
  using __rv = Arg&&;
  MDSPAN_FORCE_INLINE_FUNCTION
  static constexpr __rv
  __impl(Arg&& arg) noexcept {
    return (Arg&&)arg;
  }
};
template <class Arg1, class Arg2, class... Args>
struct __fold_right_assign_impl_<Arg1, Arg2, Args...> {
  using __next_t = __fold_right_assign_impl_<Arg2, Args...>;
  using __rv = decltype(std::declval<Arg1>() = std::declval<typename __next_t::__rv>());
  MDSPAN_FORCE_INLINE_FUNCTION
  static constexpr __rv
  __impl(Arg1&& arg, Arg2&& arg2, Args&&... args) noexcept {
    return ((Arg1&&)arg) = __next_t::__impl((Arg2&&)arg2, (Args&&)args...);
  }
};

template <class... Args>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr typename __fold_right_assign_impl_<Args...>::__rv
__fold_right_assign_impl(Args&&... args) {
  return __fold_right_assign_impl_<Args...>::__impl((Args&&)args...);
}

// </editor-fold> end right assign }}}2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// <editor-fold desc="left assign"> {{{2

template <class... Args>
struct __fold_left_assign_impl_;
template <class Arg>
struct __fold_left_assign_impl_<Arg> {
  using __rv = Arg&&;
  MDSPAN_FORCE_INLINE_FUNCTION
  static constexpr __rv
  __impl(Arg&& arg) noexcept {
    return (Arg&&)arg;
  }
};
template <class Arg1, class Arg2, class... Args>
struct __fold_left_assign_impl_<Arg1, Arg2, Args...> {
  using __assign_result_t = decltype(std::declval<Arg1>() = std::declval<Arg2>());
  using __next_t = __fold_left_assign_impl_<__assign_result_t, Args...>;
  using __rv = typename __next_t::__rv;
  MDSPAN_FORCE_INLINE_FUNCTION
  static constexpr __rv
  __impl(Arg1&& arg, Arg2&& arg2, Args&&... args) noexcept {
    return __next_t::__impl(((Arg1&&)arg) = (Arg2&&)arg2, (Args&&)args...);
  }
};

template <class... Args>
MDSPAN_FORCE_INLINE_FUNCTION
constexpr typename __fold_left_assign_impl_<Args...>::__rv
__fold_left_assign_impl(Args&&... args) {
  return __fold_left_assign_impl_<Args...>::__impl((Args&&)args...);
}

// </editor-fold> end left assign }}}2
//------------------------------------------------------------------------------

#endif

template <class... Args>
constexpr __mdspan_enable_fold_comma __fold_comma_impl(Args&&... args) noexcept { return { }; }

template <bool... Bs>
struct __bools;

} // __fold_compatibility_impl

} // end namespace std

#  define _MDSPAN_FOLD_AND(...) std::__fold_compatibility_impl::__fold_right_and_impl((__VA_ARGS__)...)
#  define _MDSPAN_FOLD_OR(...) std::__fold_compatibility_impl::__fold_right_or_impl((__VA_ARGS__)...)
#  define _MDSPAN_FOLD_ASSIGN_LEFT(INIT, ...) std::__fold_compatibility_impl::__fold_left_assign_impl(INIT, (__VA_ARGS__)...)
#  define _MDSPAN_FOLD_ASSIGN_RIGHT(PACK, ...) std::__fold_compatibility_impl::__fold_right_assign_impl((PACK)..., __VA_ARGS__)
#  define _MDSPAN_FOLD_TIMES_RIGHT(PACK, ...) std::__fold_compatibility_impl::__fold_right_times_impl((PACK)..., __VA_ARGS__)
#  define _MDSPAN_FOLD_PLUS_RIGHT(PACK, ...) std::__fold_compatibility_impl::__fold_right_plus_impl((PACK)..., __VA_ARGS__)
#  define _MDSPAN_FOLD_COMMA(...) std::__fold_compatibility_impl::__fold_comma_impl((__VA_ARGS__)...)

#  define _MDSPAN_FOLD_AND_TEMPLATE(...) \
  _MDSPAN_TRAIT(std::is_same, __fold_compatibility_impl::__bools<(__VA_ARGS__)..., true>, __fold_compatibility_impl::__bools<true, (__VA_ARGS__)...>)

#endif

// </editor-fold> end fold expressions }}}1
//==============================================================================

//==============================================================================
// <editor-fold desc="Variable template compatibility"> {{{1

#if _MDSPAN_USE_VARIABLE_TEMPLATES
#  define _MDSPAN_TRAIT(TRAIT, ...) TRAIT##_v<__VA_ARGS__>
#else
#  define _MDSPAN_TRAIT(TRAIT, ...) TRAIT<__VA_ARGS__>::value
#endif

// </editor-fold> end Variable template compatibility }}}1
//==============================================================================

//==============================================================================
// <editor-fold desc="Pre-C++14 constexpr"> {{{1

#if _MDSPAN_USE_CONSTEXPR_14
#  define _MDSPAN_CONSTEXPR_14 constexpr
// Workaround for a bug (I think?) in EDG frontends
#  ifdef __EDG__
#    define _MDSPAN_CONSTEXPR_14_DEFAULTED
#  else
#    define _MDSPAN_CONSTEXPR_14_DEFAULTED constexpr
#  endif
#else
#  define _MDSPAN_CONSTEXPR_14
#  define _MDSPAN_CONSTEXPR_14_DEFAULTED
#endif

// </editor-fold> end Pre-C++14 constexpr }}}1
//==============================================================================
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/macros.hpp

namespace std {
namespace experimental {

template <class ElementType>
struct default_accessor {

  using offset_policy = default_accessor;
  using element_type = ElementType;
  using reference = ElementType&;
  using data_handle_type = ElementType*;

  MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr default_accessor() noexcept = default;

  MDSPAN_TEMPLATE_REQUIRES(
    class OtherElementType,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, OtherElementType(*)[], element_type(*)[])
    )
  )
  MDSPAN_INLINE_FUNCTION
  constexpr default_accessor(default_accessor<OtherElementType>) noexcept {}

  MDSPAN_INLINE_FUNCTION
  constexpr data_handle_type
  offset(data_handle_type p, size_t i) const noexcept {
    return p + i;
  }

  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr reference access(data_handle_type p, size_t i) const noexcept {
    return p[i];
  }

};

} // end namespace experimental
} // end namespace std
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/default_accessor.hpp
//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/full_extent_t.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

namespace std {
namespace experimental {

struct full_extent_t { explicit full_extent_t() = default; };

_MDSPAN_INLINE_VARIABLE constexpr auto full_extent = full_extent_t{ };

} // end namespace experimental
} // namespace std
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/full_extent_t.hpp
//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/mdspan.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/layout_right.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/trait_backports.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#ifndef MDSPAN_INCLUDE_EXPERIMENTAL_BITS_TRAIT_BACKPORTS_HPP_
#define MDSPAN_INCLUDE_EXPERIMENTAL_BITS_TRAIT_BACKPORTS_HPP_

//==============================================================================
// <editor-fold desc="Variable template trait backports (e.g., is_void_v)"> {{{1

#ifdef _MDSPAN_NEEDS_TRAIT_VARIABLE_TEMPLATE_BACKPORTS

#if _MDSPAN_USE_VARIABLE_TEMPLATES
namespace std {

#define _MDSPAN_BACKPORT_TRAIT(TRAIT) \
  template <class... Args> _MDSPAN_INLINE_VARIABLE constexpr auto TRAIT##_v = TRAIT<Args...>::value;

_MDSPAN_BACKPORT_TRAIT(is_assignable)
_MDSPAN_BACKPORT_TRAIT(is_constructible)
_MDSPAN_BACKPORT_TRAIT(is_convertible)
_MDSPAN_BACKPORT_TRAIT(is_default_constructible)
_MDSPAN_BACKPORT_TRAIT(is_trivially_destructible)
_MDSPAN_BACKPORT_TRAIT(is_same)
_MDSPAN_BACKPORT_TRAIT(is_empty)
_MDSPAN_BACKPORT_TRAIT(is_void)

#undef _MDSPAN_BACKPORT_TRAIT

} // end namespace std

#endif // _MDSPAN_USE_VARIABLE_TEMPLATES

#endif // _MDSPAN_NEEDS_TRAIT_VARIABLE_TEMPLATE_BACKPORTS

// </editor-fold> end Variable template trait backports (e.g., is_void_v) }}}1
//==============================================================================

//==============================================================================
// <editor-fold desc="integer sequence (ugh...)"> {{{1

#if !defined(_MDSPAN_USE_INTEGER_SEQUENCE) || !_MDSPAN_USE_INTEGER_SEQUENCE

namespace std {

template <class T, T... Vals>
struct integer_sequence {
  static constexpr std::size_t size() noexcept { return sizeof...(Vals); }
  using value_type = T;
};

template <std::size_t... Vals>
using index_sequence = std::integer_sequence<std::size_t, Vals...>;

namespace __detail {

template <class T, T N, T I, class Result>
struct __make_int_seq_impl;

template <class T, T N, T... Vals>
struct __make_int_seq_impl<T, N, N, integer_sequence<T, Vals...>>
{
  using type = integer_sequence<T, Vals...>;
};

template <class T, T N, T I, T... Vals>
struct __make_int_seq_impl<
  T, N, I, integer_sequence<T, Vals...>
> : __make_int_seq_impl<T, N, I+1, integer_sequence<T, Vals..., I>>
{ };

} // end namespace __detail

template <class T, T N>
using make_integer_sequence = typename __detail::__make_int_seq_impl<T, N, 0, integer_sequence<T>>::type;

template <std::size_t N>
using make_index_sequence = typename __detail::__make_int_seq_impl<size_t, N, 0, integer_sequence<size_t>>::type;

template <class... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;

} // end namespace std

#endif

// </editor-fold> end integer sequence (ugh...) }}}1
//==============================================================================

//==============================================================================
// <editor-fold desc="standard trait aliases"> {{{1

#if !defined(_MDSPAN_USE_STANDARD_TRAIT_ALIASES) || !_MDSPAN_USE_STANDARD_TRAIT_ALIASES

namespace std {

#define _MDSPAN_BACKPORT_TRAIT_ALIAS(TRAIT) \
  template <class... Args> using TRAIT##_t = typename TRAIT<Args...>::type;

_MDSPAN_BACKPORT_TRAIT_ALIAS(remove_cv)
_MDSPAN_BACKPORT_TRAIT_ALIAS(remove_reference)

template <bool _B, class _T=void>
using enable_if_t = typename enable_if<_B, _T>::type;

#undef _MDSPAN_BACKPORT_TRAIT_ALIAS

} // end namespace std

#endif

// </editor-fold> end standard trait aliases }}}1
//==============================================================================

#endif //MDSPAN_INCLUDE_EXPERIMENTAL_BITS_TRAIT_BACKPORTS_HPP_
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/trait_backports.hpp
//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/extents.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/static_array.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/dynamic_extent.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

namespace std {
namespace experimental {

_MDSPAN_INLINE_VARIABLE constexpr auto dynamic_extent = std::numeric_limits<size_t>::max();

} // end namespace experimental
} // namespace std

//==============================================================================================================
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/dynamic_extent.hpp
//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/maybe_static_value.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/no_unique_address.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

namespace std {
namespace experimental {
namespace detail {

//==============================================================================

template <class _T, size_t _Disambiguator = 0, class _Enable = void>
struct __no_unique_address_emulation {
  using __stored_type = _T;
  _T __v;
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T const &__ref() const noexcept {
    return __v;
  }
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 _T &__ref() noexcept {
    return __v;
  }
};

// Empty case
// This doesn't work if _T is final, of course, but we're not using anything
// like that currently. That kind of thing could be added pretty easily though
template <class _T, size_t _Disambiguator>
struct __no_unique_address_emulation<
    _T, _Disambiguator,
    enable_if_t<_MDSPAN_TRAIT(is_empty, _T) &&
                // If the type isn't trivially destructible, its destructor
                // won't be called at the right time, so don't use this
                // specialization
                _MDSPAN_TRAIT(is_trivially_destructible, _T)>> : 
#ifdef _MDSPAN_COMPILER_MSVC
    // MSVC doesn't allow you to access public static member functions of a type
    // when you *happen* to privately inherit from that type.
    protected
#else
    // But we still want this to be private if possible so that we don't accidentally 
    // access members of _T directly rather than calling __ref() first, which wouldn't
    // work if _T happens to be stateful and thus we're using the unspecialized definition
    // of __no_unique_address_emulation above.
    private
#endif
    _T {
  using __stored_type = _T;
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T const &__ref() const noexcept {
    return *static_cast<_T const *>(this);
  }
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 _T &__ref() noexcept {
    return *static_cast<_T *>(this);
  }

  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __no_unique_address_emulation() noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __no_unique_address_emulation(
      __no_unique_address_emulation const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __no_unique_address_emulation(
      __no_unique_address_emulation &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __no_unique_address_emulation &
  operator=(__no_unique_address_emulation const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __no_unique_address_emulation &
  operator=(__no_unique_address_emulation &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__no_unique_address_emulation() noexcept = default;

  // Explicitly make this not a reference so that the copy or move
  // constructor still gets called.
  MDSPAN_INLINE_FUNCTION
  explicit constexpr __no_unique_address_emulation(_T const& __v) noexcept : _T(__v) {}
  MDSPAN_INLINE_FUNCTION
  explicit constexpr __no_unique_address_emulation(_T&& __v) noexcept : _T(::std::move(__v)) {}
};

//==============================================================================

} // end namespace detail
} // end namespace experimental
} // end namespace std
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/no_unique_address.hpp
#endif

// This is only needed for the non-standard-layout version of partially
// static array.
// Needs to be after the includes above to work with the single header generator
#if !_MDSPAN_PRESERVE_STANDARD_LAYOUT
namespace std {
namespace experimental {

//==============================================================================

namespace detail {

// static case
template <class _dynamic_t, class static_t, _static_t __v,
          _static_t __is_dynamic_sentinal = dynamic_extent,
          size_t __array_entry_index = 0>
struct __maybe_static_value {
  static constexpr _static_t __static_value = __v;
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _dynamic_t __value() const noexcept {
    return static_cast<_dynamic_t>(__v);
  }
  template <class _U>
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14
  __mdspan_enable_fold_comma
  __set_value(_U&& /*__rhs*/) noexcept {
    // Should we assert that the value matches the static value here?
    return {};
  }

  //--------------------------------------------------------------------------

  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __maybe_static_value() noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __maybe_static_value(__maybe_static_value const&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __maybe_static_value(__maybe_static_value&&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __maybe_static_value& operator=(__maybe_static_value const&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __maybe_static_value& operator=(__maybe_static_value&&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__maybe_static_value() noexcept = default;

  MDSPAN_INLINE_FUNCTION
  constexpr explicit __maybe_static_value(_dynamic_t const&) noexcept {
    // Should we assert that the value matches the static value here?
  }

  //--------------------------------------------------------------------------

};

// dynamic case
template <class _dynamic_t, class _static_t, _static_t __is_dynamic_sentinal, size_t __array_entry_index>
struct __maybe_static_value<_dynamic_t, _static_t, __is_dynamic_sentinal, __is_dynamic_sentinal,
                            __array_entry_index>
#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    : __no_unique_address_emulation<_T>
#endif
{
  static constexpr _static_t __static_value = __is_dynamic_sentinal;
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
  _MDSPAN_NO_UNIQUE_ADDRESS _dynamic_t __v = {};
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _dynamic_t __value() const noexcept {
    return __v;
  }
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 _dynamic_t &__ref() noexcept {
    return __v;
  }
  template <class _U>
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14
  __mdspan_enable_fold_comma
  __set_value(_U&& __rhs) noexcept {
    __v = (_U &&)rhs;
    return {};
  }
#else
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _dynamic_t __value() const noexcept {
    return this->__no_unique_address_emulation<_dynamic_t>::__ref();
  }
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 _dynamic_t &__ref() noexcept {
    return this->__no_unique_address_emulation<_dynamic_t>::__ref();
  }
  template <class _U>
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14
  __mdspan_enable_fold_comma
  __set_value(_U&& __rhs) noexcept {
    this->__no_unique_address_emulation<_dynamic_t>::__ref() = (_U &&)__rhs;
    return {};
  }
#endif
};

} // namespace detail

//==============================================================================

} // end namespace experimental
} // end namespace std

#endif // !_MDSPAN_PRESERVE_STANDARD_LAYOUT
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/maybe_static_value.hpp
//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/standard_layout_static_array.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/compressed_pair.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
#endif

namespace std {
namespace experimental {
namespace detail {

// For no unique address emulation, this is the case taken when neither are empty.
// For real `[[no_unique_address]]`, this case is always taken.
template <class _T, class _U, class _Enable = void> struct __compressed_pair {
  _MDSPAN_NO_UNIQUE_ADDRESS _T __t_val;
  _MDSPAN_NO_UNIQUE_ADDRESS _U __u_val;
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 _T &__first() noexcept { return __t_val; }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T const &__first() const noexcept {
    return __t_val;
  }
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 _U &__second() noexcept { return __u_val; }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _U const &__second() const noexcept {
    return __u_val;
  }

  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __compressed_pair() noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __compressed_pair(__compressed_pair const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __compressed_pair(__compressed_pair &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __compressed_pair &
  operator=(__compressed_pair const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __compressed_pair &
  operator=(__compressed_pair &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__compressed_pair() noexcept = default;
  template <class _TLike, class _ULike>
  MDSPAN_INLINE_FUNCTION constexpr __compressed_pair(_TLike &&__t, _ULike &&__u)
      : __t_val((_TLike &&) __t), __u_val((_ULike &&) __u) {}
};

#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)

// First empty.
template <class _T, class _U>
struct __compressed_pair<
    _T, _U,
    enable_if_t<_MDSPAN_TRAIT(is_empty, _T) && !_MDSPAN_TRAIT(is_empty, _U)>>
    : private _T {
  _U __u_val;
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 _T &__first() noexcept {
    return *static_cast<_T *>(this);
  }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T const &__first() const noexcept {
    return *static_cast<_T const *>(this);
  }
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 _U &__second() noexcept { return __u_val; }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _U const &__second() const noexcept {
    return __u_val;
  }

  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __compressed_pair() noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __compressed_pair(__compressed_pair const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __compressed_pair(__compressed_pair &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __compressed_pair &
  operator=(__compressed_pair const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __compressed_pair &
  operator=(__compressed_pair &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__compressed_pair() noexcept = default;
  template <class _TLike, class _ULike>
  MDSPAN_INLINE_FUNCTION constexpr __compressed_pair(_TLike &&__t, _ULike &&__u)
      : _T((_TLike &&) __t), __u_val((_ULike &&) __u) {}
};

// Second empty.
template <class _T, class _U>
struct __compressed_pair<
    _T, _U,
    enable_if_t<!_MDSPAN_TRAIT(is_empty, _T) && _MDSPAN_TRAIT(is_empty, _U)>>
    : private _U {
  _T __t_val;
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 _T &__first() noexcept { return __t_val; }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T const &__first() const noexcept {
    return __t_val;
  }
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 _U &__second() noexcept {
    return *static_cast<_U *>(this);
  }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _U const &__second() const noexcept {
    return *static_cast<_U const *>(this);
  }

  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __compressed_pair() noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __compressed_pair(__compressed_pair const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __compressed_pair(__compressed_pair &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __compressed_pair &
  operator=(__compressed_pair const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __compressed_pair &
  operator=(__compressed_pair &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__compressed_pair() noexcept = default;

  template <class _TLike, class _ULike>
  MDSPAN_INLINE_FUNCTION constexpr __compressed_pair(_TLike &&__t, _ULike &&__u)
      : _U((_ULike &&) __u), __t_val((_TLike &&) __t) {}
};

// Both empty.
template <class _T, class _U>
struct __compressed_pair<
    _T, _U,
    enable_if_t<_MDSPAN_TRAIT(is_empty, _T) && _MDSPAN_TRAIT(is_empty, _U)>>
    // We need to use the __no_unique_address_emulation wrapper here to avoid
    // base class ambiguities.
#ifdef _MDSPAN_COMPILER_MSVC
// MSVC doesn't allow you to access public static member functions of a type
// when you *happen* to privately inherit from that type.
    : protected __no_unique_address_emulation<_T, 0>,
      protected __no_unique_address_emulation<_U, 1>
#else
    : private __no_unique_address_emulation<_T, 0>,
      private __no_unique_address_emulation<_U, 1>
#endif
{
  using __first_base_t = __no_unique_address_emulation<_T, 0>;
  using __second_base_t = __no_unique_address_emulation<_U, 1>;

  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 _T &__first() noexcept {
    return this->__first_base_t::__ref();
  }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T const &__first() const noexcept {
    return this->__first_base_t::__ref();
  }
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 _U &__second() noexcept {
    return this->__second_base_t::__ref();
  }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _U const &__second() const noexcept {
    return this->__second_base_t::__ref();
  }

  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __compressed_pair() noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __compressed_pair(__compressed_pair const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __compressed_pair(__compressed_pair &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __compressed_pair &
  operator=(__compressed_pair const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __compressed_pair &
  operator=(__compressed_pair &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__compressed_pair() noexcept = default;
  template <class _TLike, class _ULike>
  MDSPAN_INLINE_FUNCTION constexpr __compressed_pair(_TLike &&__t, _ULike &&__u) noexcept
    : __first_base_t(_T((_TLike &&) __t)),
      __second_base_t(_U((_ULike &&) __u))
  { }
};

#endif // !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)

} // end namespace detail
} // end namespace experimental
} // end namespace std
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/compressed_pair.hpp

#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
#endif

#ifdef __cpp_lib_span
#endif

namespace std {
namespace experimental {
namespace detail {

//==============================================================================

_MDSPAN_INLINE_VARIABLE constexpr struct
    __construct_psa_from_dynamic_exts_values_tag_t {
} __construct_psa_from_dynamic_exts_values_tag = {};

_MDSPAN_INLINE_VARIABLE constexpr struct
    __construct_psa_from_all_exts_values_tag_t {
} __construct_psa_from_all_exts_values_tag = {};

struct __construct_psa_from_all_exts_array_tag_t {};
template <size_t _N = 0> struct __construct_psa_from_dynamic_exts_array_tag_t {};

//==============================================================================

template <size_t _I, class _T> using __repeated_with_idxs = _T;

//==============================================================================

#if _MDSPAN_PRESERVE_STANDARD_LAYOUT

/**
 *  PSA = "partially static array"
 *
 * @tparam _T
 * @tparam _ValsSeq
 * @tparam __sentinal
 */
template <class _Tag, class _T, class _static_t, class _ValsSeq, _static_t __sentinal = static_cast<_static_t>(dynamic_extent),
          class _IdxsSeq = make_index_sequence<_ValsSeq::size()>>
struct __standard_layout_psa;

//==============================================================================
// Static case
template <class _Tag, class _T, class _static_t, _static_t __value, _static_t... __values_or_sentinals,
          _static_t __sentinal, size_t _Idx, size_t... _Idxs>
struct __standard_layout_psa<
    _Tag, _T, _static_t, integer_sequence<_static_t, __value, __values_or_sentinals...>,
    __sentinal, integer_sequence<size_t, _Idx, _Idxs...>>
#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    : private __no_unique_address_emulation<__standard_layout_psa<
          _Tag, _T, _static_t, integer_sequence<_static_t, __values_or_sentinals...>, __sentinal,
          integer_sequence<size_t, _Idxs...>>>
#endif
{

  //--------------------------------------------------------------------------

  using __next_t =
      __standard_layout_psa<_Tag, _T, _static_t,
                            integer_sequence<_static_t, __values_or_sentinals...>,
                            __sentinal, integer_sequence<size_t, _Idxs...>>;

#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
  _MDSPAN_NO_UNIQUE_ADDRESS __next_t __next_;
#else
  using __base_t = __no_unique_address_emulation<__next_t>;
#endif

  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 __next_t &__next() noexcept {
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    return __next_;
#else
    return this->__base_t::__ref();
#endif
  }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr __next_t const &__next() const noexcept {
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    return __next_;
#else
    return this->__base_t::__ref();
#endif
  }

  static constexpr auto __size = sizeof...(_Idxs) + 1;
  static constexpr auto __size_dynamic = __next_t::__size_dynamic;

  //--------------------------------------------------------------------------

  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa() noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa(__standard_layout_psa const &) noexcept =
      default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa(__standard_layout_psa &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __standard_layout_psa &
  operator=(__standard_layout_psa const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __standard_layout_psa &
  operator=(__standard_layout_psa &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__standard_layout_psa() noexcept = default;

  //--------------------------------------------------------------------------

  MDSPAN_INLINE_FUNCTION
  constexpr __standard_layout_psa(
      __construct_psa_from_all_exts_values_tag_t, _T const & /*__val*/,
      __repeated_with_idxs<_Idxs, _T> const &... __vals) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      : __next_{
#else
      : __base_t(__base_t{__next_t(
#endif
          __construct_psa_from_all_exts_values_tag, __vals...
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        }
#else
        )})
#endif
  { }

  template <class... _Ts>
  MDSPAN_INLINE_FUNCTION constexpr __standard_layout_psa(
      __construct_psa_from_dynamic_exts_values_tag_t,
      _Ts const &... __vals) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      : __next_{
#else
      : __base_t(__base_t{__next_t(
#endif
          __construct_psa_from_dynamic_exts_values_tag, __vals...
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        }
#else
        )})
#endif
  { }

  template <class _U, size_t _N>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      array<_U, _N> const &__vals) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      : __next_{
#else
      : __base_t(__base_t{__next_t(
#endif
          __vals
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        }
#else
        )})
#endif
  { }

  template <class _U, size_t _NStatic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_all_exts_array_tag_t const & __tag,
      array<_U, _NStatic> const &__vals) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      : __next_{
#else
      : __base_t(__base_t{__next_t(
#endif
          __tag, __vals
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        }
#else
        )})
#endif
  { }

  template <class _U, size_t _IDynamic, size_t _NDynamic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_dynamic_exts_array_tag_t<_IDynamic> __tag,
      array<_U, _NDynamic> const &__vals) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      : __next_{
#else
      : __base_t(__base_t{__next_t(
#endif
          __tag, __vals
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        }
#else
        )})
#endif
  { }

#ifdef __cpp_lib_span
  template <class _U, size_t _N>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      span<_U, _N> const &__vals) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      : __next_{
#else
      : __base_t(__base_t{__next_t(
#endif
          __vals
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        }
#else
        )})
#endif
  { }

  template <class _U, size_t _NStatic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_all_exts_array_tag_t const & __tag,
      span<_U, _NStatic> const &__vals) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      : __next_{
#else
      : __base_t(__base_t{__next_t(
#endif
          __tag, __vals
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        }
#else
        )})
#endif
  { }

  template <class _U, size_t _IDynamic, size_t _NDynamic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_dynamic_exts_array_tag_t<_IDynamic> __tag,
      span<_U, _NDynamic> const &__vals) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      : __next_{
#else
      : __base_t(__base_t{__next_t(
#endif
          __tag, __vals
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        }
#else
        )})
#endif
  { }
#endif

  template <class _UTag, class _U, class _static_U, class _UValsSeq, _static_U __u_sentinal,
            class _IdxsSeq>
  MDSPAN_INLINE_FUNCTION constexpr __standard_layout_psa(
      __standard_layout_psa<_UTag, _U, _static_U, _UValsSeq, __u_sentinal, _IdxsSeq> const
          &__rhs) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      : __next_{
#else
      : __base_t(__base_t{__next_t(
#endif
          __rhs.__next()
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        }
#else
        )})
#endif
  { }

  //--------------------------------------------------------------------------

  // See https://godbolt.org/z/_KSDNX for a summary-by-example of why this is
  // necessary. We're using inheritance here instead of an alias template
  // because we have to deduce __values_or_sentinals in several places, and
  // alias templates don't permit that in this context.
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr __standard_layout_psa const &__enable_psa_conversion() const
      noexcept {
    return *this;
  }

  template <size_t _I, enable_if_t<_I != _Idx, int> = 0>
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get_n() const noexcept {
    return __next().template __get_n<_I>();
  }
  template <size_t _I, enable_if_t<_I == _Idx, int> = 1>
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get_n() const noexcept {
    return __value;
  }
  template <size_t _I, enable_if_t<_I != _Idx, int> = 0>
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 void
  __set_n(_T const &__rhs) noexcept {
    __next().__set_value(__rhs);
  }
  template <size_t _I, enable_if_t<_I == _Idx, int> = 1>
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 void
  __set_n(_T const &) noexcept {
    // Don't assert here because that would break constexpr. This better
    // not change anything, though
  }
  template <size_t _I, enable_if_t<_I == _Idx, _static_t> = __sentinal>
  MDSPAN_FORCE_INLINE_FUNCTION static constexpr _static_t __get_static_n() noexcept {
    return __value;
  }
  template <size_t _I, enable_if_t<_I != _Idx, _static_t> __default = __sentinal>
  MDSPAN_FORCE_INLINE_FUNCTION static constexpr _static_t __get_static_n() noexcept {
    return __next_t::template __get_static_n<_I, __default>();
  }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get(size_t __n) const noexcept {
    return __value * (_T(_Idx == __n)) + __next().__get(__n);
  }

  //--------------------------------------------------------------------------
};

//==============================================================================

// Dynamic case, __next_t may or may not be empty
template <class _Tag, class _T, class _static_t, _static_t __sentinal, _static_t... __values_or_sentinals,
          size_t _Idx, size_t... _Idxs>
struct __standard_layout_psa<
    _Tag, _T, _static_t, integer_sequence<_static_t, __sentinal, __values_or_sentinals...>,
    __sentinal, integer_sequence<size_t, _Idx, _Idxs...>> {
  //--------------------------------------------------------------------------

  using __next_t =
      __standard_layout_psa<_Tag, _T, _static_t,
                            integer_sequence<_static_t, __values_or_sentinals...>,
                            __sentinal, integer_sequence<size_t, _Idxs...>>;

  using __value_pair_t = __compressed_pair<_T, __next_t>;
  __value_pair_t __value_pair;
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 __next_t &__next() noexcept {
    return __value_pair.__second();
  }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr __next_t const &__next() const noexcept {
    return __value_pair.__second();
  }

  static constexpr auto __size = sizeof...(_Idxs) + 1;
  static constexpr auto __size_dynamic = 1 + __next_t::__size_dynamic;

  //--------------------------------------------------------------------------

  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa() noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa(__standard_layout_psa const &) noexcept =
      default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa(__standard_layout_psa &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __standard_layout_psa &
  operator=(__standard_layout_psa const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __standard_layout_psa &
  operator=(__standard_layout_psa &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__standard_layout_psa() noexcept = default;

  //--------------------------------------------------------------------------

  MDSPAN_INLINE_FUNCTION
  constexpr __standard_layout_psa(
      __construct_psa_from_all_exts_values_tag_t, _T const &__val,
      __repeated_with_idxs<_Idxs, _T> const &... __vals) noexcept
      : __value_pair(__val,
                     __next_t(__construct_psa_from_all_exts_values_tag,
                              __vals...)) {}

  template <class... _Ts>
  MDSPAN_INLINE_FUNCTION constexpr __standard_layout_psa(
      __construct_psa_from_dynamic_exts_values_tag_t, _T const &__val,
      _Ts const &... __vals) noexcept
      : __value_pair(__val,
                     __next_t(__construct_psa_from_dynamic_exts_values_tag,
                              __vals...)) {}

  template <class _U, size_t _N>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      array<_U, _N> const &__vals) noexcept
      : __value_pair(::std::get<_Idx>(__vals), __vals) {}

  template <class _U, size_t _NStatic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_all_exts_array_tag_t __tag,
      array<_U, _NStatic> const &__vals) noexcept
      : __value_pair(
            ::std::get<_Idx>(__vals),
            __next_t(__tag,
                     __vals)) {}

  template <class _U, size_t _IDynamic, size_t _NDynamic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_dynamic_exts_array_tag_t<_IDynamic>,
      array<_U, _NDynamic> const &__vals) noexcept
      : __value_pair(
            ::std::get<_IDynamic>(__vals),
            __next_t(__construct_psa_from_dynamic_exts_array_tag_t<_IDynamic + 1>{},
                     __vals)) {}

#ifdef __cpp_lib_span
  template <class _U, size_t _N>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      span<_U, _N> const &__vals) noexcept
      : __value_pair(__vals[_Idx], __vals) {}

  template <class _U, size_t _NStatic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_all_exts_array_tag_t __tag,
      span<_U, _NStatic> const &__vals) noexcept
      : __value_pair(
            __vals[_Idx],
            __next_t(__tag,
                     __vals)) {}

  template <class _U, size_t _IDynamic, size_t _NDynamic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_dynamic_exts_array_tag_t<_IDynamic>,
      span<_U, _NDynamic> const &__vals) noexcept
      : __value_pair(
            __vals[_IDynamic],
            __next_t(__construct_psa_from_dynamic_exts_array_tag_t<_IDynamic + 1>{},
                     __vals)) {}
#endif

  template <class _UTag, class _U, class _static_U, class _UValsSeq, _static_U __u_sentinal,
            class _UIdxsSeq>
  MDSPAN_INLINE_FUNCTION constexpr __standard_layout_psa(
      __standard_layout_psa<_UTag, _U, _static_U, _UValsSeq, __u_sentinal, _UIdxsSeq> const
          &__rhs) noexcept
      : __value_pair(__rhs.template __get_n<_Idx>(), __rhs.__next()) {}

  //--------------------------------------------------------------------------

  // See comment in the previous partial specialization for why this is
  // necessary.  Or just trust me that it's messy.
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr __standard_layout_psa const &__enable_psa_conversion() const
      noexcept {
    return *this;
  }

  template <size_t _I, enable_if_t<_I != _Idx, int> = 0>
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get_n() const noexcept {
    return __next().template __get_n<_I>();
  }
  template <size_t _I, enable_if_t<_I == _Idx, int> = 1>
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get_n() const noexcept {
    return __value_pair.__first();
  }
  template <size_t _I, enable_if_t<_I != _Idx, int> = 0>
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 void
  __set_n(_T const &__rhs) noexcept {
    __next().__set_value(__rhs);
  }
  template <size_t _I, enable_if_t<_I == _Idx, int> = 1>
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 void
  __set_n(_T const &__rhs) noexcept {
    __value_pair.__first() = __rhs;
  }
  template <size_t _I, enable_if_t<_I == _Idx, _static_t> __default = __sentinal>
  MDSPAN_FORCE_INLINE_FUNCTION static constexpr _static_t __get_static_n() noexcept {
    return __default;
  }
  template <size_t _I, enable_if_t<_I != _Idx, _static_t> __default = __sentinal>
  MDSPAN_FORCE_INLINE_FUNCTION static constexpr _static_t __get_static_n() noexcept {
    return __next_t::template __get_static_n<_I, __default>();
  }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get(size_t __n) const noexcept {
    return __value_pair.__first() * (_T(_Idx == __n)) + __next().__get(__n);
  }

  //--------------------------------------------------------------------------
};

// empty/terminal case
template <class _Tag, class _T, class _static_t, _static_t __sentinal>
struct __standard_layout_psa<_Tag, _T, _static_t, integer_sequence<_static_t>, __sentinal,
                             integer_sequence<size_t>> {
  //--------------------------------------------------------------------------

  static constexpr auto __size = 0;
  static constexpr auto __size_dynamic = 0;

  //--------------------------------------------------------------------------

  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa() noexcept
#if defined(__clang__) || defined(_MDSPAN_DEFAULTED_CONSTRUCTORS_INHERITANCE_WORKAROUND)
  // As far as I can tell, there appears to be a bug in clang that's causing
  // this to be non-constexpr when it's defaulted.
  { }
#else
   = default;
#endif
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa(__standard_layout_psa const &) noexcept =
      default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa(__standard_layout_psa &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __standard_layout_psa &
  operator=(__standard_layout_psa const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __standard_layout_psa &
  operator=(__standard_layout_psa &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__standard_layout_psa() noexcept = default;

  MDSPAN_INLINE_FUNCTION
  constexpr __standard_layout_psa(
      __construct_psa_from_all_exts_values_tag_t) noexcept {}

  template <class... _Ts>
  MDSPAN_INLINE_FUNCTION constexpr __standard_layout_psa(
      __construct_psa_from_dynamic_exts_values_tag_t) noexcept {}

  template <class _U, size_t _N>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      array<_U, _N> const &) noexcept {}

  template <class _U, size_t _NStatic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_all_exts_array_tag_t,
      array<_U, _NStatic> const &) noexcept {}

  template <class _U, size_t _IDynamic, size_t _NDynamic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_dynamic_exts_array_tag_t<_IDynamic>,
      array<_U, _NDynamic> const &) noexcept {}

#ifdef __cpp_lib_span
  template <class _U, size_t _N>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      span<_U, _N> const &) noexcept {}

  template <class _U, size_t _NStatic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_all_exts_array_tag_t,
      span<_U, _NStatic> const &) noexcept {}

  template <class _U, size_t _IDynamic, size_t _NDynamic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_dynamic_exts_array_tag_t<_IDynamic>,
      span<_U, _NDynamic> const &) noexcept {}
#endif

  template <class _UTag, class _U, class _static_U, class _UValsSeq, _static_U __u_sentinal,
            class _UIdxsSeq>
  MDSPAN_INLINE_FUNCTION constexpr __standard_layout_psa(
      __standard_layout_psa<_UTag, _U, _static_U, _UValsSeq, __u_sentinal, _UIdxsSeq> const&) noexcept {}

  // See comment in the previous partial specialization for why this is
  // necessary.  Or just trust me that it's messy.
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr __standard_layout_psa const &__enable_psa_conversion() const
      noexcept {
    return *this;
  }

  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get(size_t /*n*/) const noexcept {
    return 0;
  }
};

// Same thing, but with a disambiguator so that same-base issues doesn't cause
// a loss of standard-layout-ness.
template <class _Tag, class T, class _static_t, _static_t... __values_or_sentinals>
struct __partially_static_sizes_tagged
    : __standard_layout_psa<
          _Tag, T, _static_t,
          integer_sequence<_static_t, __values_or_sentinals...>> {
  using __tag_t = _Tag;
  using __psa_impl_t = __standard_layout_psa<
      _Tag, T, _static_t, integer_sequence<_static_t, __values_or_sentinals...>>;
  using __psa_impl_t::__psa_impl_t;
#ifdef _MDSPAN_DEFAULTED_CONSTRUCTORS_INHERITANCE_WORKAROUND
  MDSPAN_INLINE_FUNCTION
#endif
  constexpr __partially_static_sizes_tagged() noexcept
#ifdef _MDSPAN_DEFAULTED_CONSTRUCTORS_INHERITANCE_WORKAROUND
    : __psa_impl_t() { }
#else
    = default;
#endif
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __partially_static_sizes_tagged(
      __partially_static_sizes_tagged const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __partially_static_sizes_tagged(
      __partially_static_sizes_tagged &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __partially_static_sizes_tagged &
  operator=(__partially_static_sizes_tagged const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __partially_static_sizes_tagged &
  operator=(__partially_static_sizes_tagged &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__partially_static_sizes_tagged() noexcept = default;

  template <class _UTag>
  MDSPAN_FORCE_INLINE_FUNCTION constexpr explicit __partially_static_sizes_tagged(
    __partially_static_sizes_tagged<_UTag, T, _static_t, __values_or_sentinals...> const& __vals
  ) noexcept : __psa_impl_t(__vals.__enable_psa_conversion()) { }
};

struct __no_tag {};
template <class T, class _static_t, _static_t... __values_or_sentinals>
struct __partially_static_sizes
    : __partially_static_sizes_tagged<__no_tag, T, _static_t, __values_or_sentinals...> {
private:
  using __base_t =
      __partially_static_sizes_tagged<__no_tag, T, _static_t, __values_or_sentinals...>;
  template <class _UTag>
  MDSPAN_FORCE_INLINE_FUNCTION constexpr __partially_static_sizes(
    __partially_static_sizes_tagged<_UTag, T, _static_t, __values_or_sentinals...>&& __vals
  ) noexcept : __base_t(::std::move(__vals)) { }
public:
  using __base_t::__base_t;

#ifdef _MDSPAN_DEFAULTED_CONSTRUCTORS_INHERITANCE_WORKAROUND
  MDSPAN_INLINE_FUNCTION
  constexpr __partially_static_sizes() noexcept : __base_t() { }
#endif
  template <class _UTag>
  MDSPAN_FORCE_INLINE_FUNCTION constexpr __partially_static_sizes_tagged<
      _UTag, T, _static_t, __values_or_sentinals...>
  __with_tag() const noexcept {
    return __partially_static_sizes_tagged<_UTag, T, _static_t, __values_or_sentinals...>(*this);
  }
};

#endif // _MDSPAN_PRESERVE_STATIC_LAYOUT

} // end namespace detail
} // end namespace experimental
} // end namespace std
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/standard_layout_static_array.hpp
//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/type_list.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

namespace std {
namespace experimental {

//==============================================================================

namespace detail {

template <class... _Ts> struct __type_list { static constexpr auto __size = sizeof...(_Ts); };

// Implementation of type_list at() that's heavily optimized for small typelists
template <size_t, class> struct __type_at;
template <size_t, class _Seq, class=make_index_sequence<_Seq::__size>> struct __type_at_large_impl;

template <size_t _I, size_t _Idx, class _T>
struct __type_at_entry { };

template <class _Result>
struct __type_at_assign_op_ignore_rest {
  template <class _T>
  __type_at_assign_op_ignore_rest<_Result> operator=(_T&&);
  using type = _Result;
};

struct __type_at_assign_op_impl {
  template <size_t _I, size_t _Idx, class _T>
  __type_at_assign_op_impl operator=(__type_at_entry<_I, _Idx, _T>&&);
  template <size_t _I, class _T>
  __type_at_assign_op_ignore_rest<_T> operator=(__type_at_entry<_I, _I, _T>&&);
};

template <size_t _I, class... _Ts, size_t... _Idxs>
struct __type_at_large_impl<_I, __type_list<_Ts...>, integer_sequence<size_t, _Idxs...>>
  : decltype(
      _MDSPAN_FOLD_ASSIGN_LEFT(__type_at_assign_op_impl{}, /* = ... = */ __type_at_entry<_I, _Idxs, _Ts>{})
    )
{ };

template <size_t _I, class... _Ts>
struct __type_at<_I, __type_list<_Ts...>>
    : __type_at_large_impl<_I, __type_list<_Ts...>>
{ };

template <class _T0, class... _Ts>
struct __type_at<0, __type_list<_T0, _Ts...>> {
  using type = _T0;
};

template <class _T0, class _T1, class... _Ts>
struct __type_at<1, __type_list<_T0, _T1, _Ts...>> {
  using type = _T1;
};

template <class _T0, class _T1, class _T2, class... _Ts>
struct __type_at<2, __type_list<_T0, _T1, _T2, _Ts...>> {
  using type = _T2;
};

template <class _T0, class _T1, class _T2, class _T3, class... _Ts>
struct __type_at<3, __type_list<_T0, _T1, _T2, _T3, _Ts...>> {
  using type = _T3;
};

} // namespace detail

//==============================================================================

} // end namespace experimental
} // end namespace std

//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/type_list.hpp

// Needs to be after the includes above to work with the single header generator
#if !_MDSPAN_PRESERVE_STANDARD_LAYOUT

namespace std {
namespace experimental {
namespace detail {

//==============================================================================

template <class _T, _T _Val, bool _Mask> struct __mask_element {};

template <class _T, _T... _Result>
struct __mask_sequence_assign_op {
  template <_T _V>
  __mask_sequence_assign_op<_T, _Result..., _V>
  operator=(__mask_element<_T, _V, true>&&);
  template <_T _V>
  __mask_sequence_assign_op<_T, _Result...>
  operator=(__mask_element<_T, _V, false>&&);
  using __result = integer_sequence<_T, _Result...>;
};

template <class _Seq, class _Mask>
struct __mask_sequence;

template <class _T, _T... _Vals, bool... _Masks>
struct __mask_sequence<integer_sequence<_T, _Vals...>, integer_sequence<bool, _Masks...>>
{
  using type = typename decltype(
    _MDSPAN_FOLD_ASSIGN_LEFT(
      __mask_sequence_assign_op<_T>{}, /* = ... = */ __mask_element<_T, _Vals, _Masks>{}
    )
  )::__result;
};

//==============================================================================

template <class _T, class _static_t, class _Vals, _static_t __sentinal,
          class _Idxs, class _IdxsDynamic, class _IdxsDynamicIdxs>
class __partially_static_array_impl;

template <
  class _T, class _static_t,
  _static_t... __values_or_sentinals, _static_t __sentinal,
  size_t... _Idxs,
  size_t... _IdxsDynamic,
  size_t... _IdxsDynamicIdxs
>
class __partially_static_array_impl<
  _T,
  _static_t,
  integer_sequence<_static_t, __values_or_sentinals...>,
  __sentinal,
  integer_sequence<size_t, _Idxs...>,
  integer_sequence<size_t, _IdxsDynamic...>,
  integer_sequence<size_t, _IdxsDynamicIdxs...>
>
    : private __maybe_static_value<_T, _static_t, __values_or_sentinals, __sentinal,
                                   _Idxs>... {
private:

  template <size_t _N>
  using __base_n = typename __type_at<_N,
    __type_list<__maybe_static_value<_T, _static_t, __values_or_sentinals, __sentinal, _Idxs>...>
  >::type;

public:

  static constexpr auto __size = sizeof...(_Idxs);
  static constexpr auto __size_dynamic =
    _MDSPAN_FOLD_PLUS_RIGHT(static_cast<int>((__values_or_sentinals == __sentinal)), /* + ... + */ 0);

  //--------------------------------------------------------------------------

  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __partially_static_array_impl() = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __partially_static_array_impl(
      __partially_static_array_impl const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __partially_static_array_impl(
      __partially_static_array_impl &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __partially_static_array_impl &
  operator=(__partially_static_array_impl const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __partially_static_array_impl &
  operator=(__partially_static_array_impl &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__partially_static_array_impl() noexcept = default;

  MDSPAN_INLINE_FUNCTION
  constexpr __partially_static_array_impl(
      __construct_psa_from_all_exts_values_tag_t,
      __repeated_with_idxs<_Idxs, _T> const &... __vals) noexcept
      : __base_n<_Idxs>(__base_n<_Idxs>{{__vals}})... {}

  MDSPAN_INLINE_FUNCTION
  constexpr __partially_static_array_impl(
      __construct_psa_from_dynamic_exts_values_tag_t,
      __repeated_with_idxs<_IdxsDynamicIdxs, _T> const &... __vals) noexcept
      : __base_n<_IdxsDynamic>(__base_n<_IdxsDynamic>{{__vals}})... {}

  MDSPAN_INLINE_FUNCTION constexpr explicit __partially_static_array_impl(
    array<_T, sizeof...(_Idxs)> const& __vals) noexcept
    : __partially_static_array_impl(
        __construct_psa_from_all_exts_values_tag,
        ::std::get<_Idxs>(__vals)...) {}

  // clang-format off
  MDSPAN_FUNCTION_REQUIRES(
    (MDSPAN_INLINE_FUNCTION constexpr explicit),
    __partially_static_array_impl,
    (array<_T, __size_dynamic> const &__vals), noexcept,
    /* requires */
      (sizeof...(_Idxs) != __size_dynamic)
  ): __partially_static_array_impl(
       __construct_psa_from_dynamic_exts_values_tag,
       ::std::get<_IdxsDynamicIdxs>(__vals)...) {}
  // clang-format on

  template <class _U, class _static_u, class _UValsSeq, _static_u __u_sentinal, class _UIdxsSeq,
            class _UIdxsDynamicSeq, class _UIdxsDynamicIdxsSeq>
  MDSPAN_INLINE_FUNCTION constexpr __partially_static_array_impl(
    __partially_static_array_impl<
      _U, _static_u, _UValsSeq, __u_sentinal, _UIdxsSeq,
     _UIdxsDynamicSeq, _UIdxsDynamicIdxsSeq> const &__rhs) noexcept
    : __partially_static_array_impl(
        __construct_psa_from_all_exts_values_tag,
        __rhs.template __get_n<_Idxs>()...) {}

  //--------------------------------------------------------------------------

  // See comment in the previous partial specialization for why this is
  // necessary.  Or just trust me that it's messy.
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr __partially_static_array_impl const &__enable_psa_conversion() const
  noexcept {
      return *this;
  }

  template <size_t _I>
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get_n() const noexcept {
    return static_cast<__base_n<_I> const*>(this)->__value();
  }

  template <class _U, size_t _I>
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 void __set_n(_U&& __rhs) noexcept {
    static_cast<__base_n<_I>*>(this)->__set_value((_U&&)__rhs);
  }

  template <size_t _I, _static_t __default = __sentinal>
  MDSPAN_FORCE_INLINE_FUNCTION static constexpr _static_t
  __get_static_n() noexcept {
    return __base_n<_I>::__static_value == __sentinal ?
      __default : __base_n<_I>::__static_value;
  }

  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T
  __get(size_t __n) const noexcept {
    return _MDSPAN_FOLD_PLUS_RIGHT(
      (_T(_Idxs == __n) * __get_n<_Idxs>()), /* + ... + */ _T(0)
    );
  }

};

//==============================================================================

template <class _T, class _static_t, class _ValSeq, _static_t __sentinal, class _Idxs = make_index_sequence<_ValSeq::size()>>
struct __partially_static_array_impl_maker;

template <
  class _T, class _static_t,  _static_t... _Vals, _static_t __sentinal, size_t... _Idxs
>
struct __partially_static_array_impl_maker<
  _T, _static_t, integer_sequence<_static_t, _Vals...>, __sentinal, integer_sequence<size_t, _Idxs...>
>
{
  using __dynamic_idxs = typename __mask_sequence<
    integer_sequence<size_t, _Idxs...>,
    integer_sequence<bool, (_Vals == __sentinal)...>
  >::type;
  using __impl_base =
    __partially_static_array_impl<_T, _static_t,
      integer_sequence<_static_t, _Vals...>,
      __sentinal, integer_sequence<size_t, _Idxs...>,
      __dynamic_idxs,
      make_index_sequence<__dynamic_idxs::size()>
    >;
};

template <class _T, class _static_t, class _ValsSeq, _static_t __sentinal = dynamic_extent>
class __partially_static_array_with_sentinal
  : public __partially_static_array_impl_maker<_T, _static_t, _ValsSeq, __sentinal>::__impl_base
{
private:
  using __base_t = typename __partially_static_array_impl_maker<_T, _static_t, _ValsSeq, __sentinal>::__impl_base;
public:
  using __base_t::__base_t;
};

//==============================================================================

template <class T, class _static_t, _static_t... __values_or_sentinals>
struct __partially_static_sizes :
  __partially_static_array_with_sentinal<
    T, _static_t, ::std::integer_sequence<_static_t, __values_or_sentinals...>>
{
private:
  using __base_t = __partially_static_array_with_sentinal<
    T, _static_t, ::std::integer_sequence<_static_t, __values_or_sentinals...>>;
public:
  using __base_t::__base_t;
  template <class _UTag>
  MDSPAN_FORCE_INLINE_FUNCTION constexpr __partially_static_sizes<T, _static_t, __values_or_sentinals...>
  __with_tag() const noexcept {
    return *this;
  }
};

// Tags are needed for the standard layout version, but not here
template <class T, class _static_t, _static_t... __values_or_sentinals>
using __partially_static_sizes_tagged = __partially_static_sizes<T, _static_t, __values_or_sentinals...>;

} // end namespace detail
} // end namespace experimental
} // end namespace std

#endif // !_MDSPAN_PRESERVE_STANDARD_LAYOUT
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/static_array.hpp

#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
#endif

namespace std {
namespace experimental {

namespace detail {

template<size_t ... Extents>
struct _count_dynamic_extents;

template<size_t E, size_t ... Extents>
struct _count_dynamic_extents<E,Extents...> {
  static constexpr size_t val = (E==dynamic_extent?1:0) + _count_dynamic_extents<Extents...>::val;
};

template<>
struct _count_dynamic_extents<> {
  static constexpr size_t val = 0;
};

template <size_t... Extents, size_t... OtherExtents>
static constexpr std::false_type _check_compatible_extents(
  std::false_type, std::integer_sequence<size_t, Extents...>, std::integer_sequence<size_t, OtherExtents...>
) noexcept { return { }; }

template <size_t... Extents, size_t... OtherExtents>
static std::integral_constant<
  bool,
  _MDSPAN_FOLD_AND(
    (
      Extents == dynamic_extent
        || OtherExtents == dynamic_extent
        || Extents == OtherExtents
    ) /* && ... */
  )
>
_check_compatible_extents(
  std::true_type, std::integer_sequence<size_t, Extents...>, std::integer_sequence<size_t, OtherExtents...>
) noexcept { return { }; }

struct __extents_tag { };

} // end namespace detail

template <class ThisIndexType, size_t... Extents>
class extents
#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
  : private detail::__no_unique_address_emulation<
      detail::__partially_static_sizes_tagged<detail::__extents_tag, ThisIndexType , size_t, Extents...>>
#endif
{
public:

  using rank_type = size_t;
  using index_type = ThisIndexType;
  static_assert(std::is_integral<index_type>::value && !std::is_same<index_type, bool>::value,
                "std::extents requires a signed or unsigned integer as index_type parameter");
  using size_type = make_unsigned_t<index_type>;

// internal typedefs which for technical reasons are public
  using __storage_t = detail::__partially_static_sizes_tagged<detail::__extents_tag, index_type, size_t, Extents...>;

#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
  _MDSPAN_NO_UNIQUE_ADDRESS __storage_t __storage_;
#else
  using __base_t = detail::__no_unique_address_emulation<__storage_t>;
#endif

// private members dealing with the way we internally store dynamic extents
 private:

  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14
  __storage_t& __storage() noexcept {
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    return __storage_;
#else
    return this->__base_t::__ref();
#endif
  }
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr __storage_t const& __storage() const noexcept {
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    return __storage_;
#else
    return this->__base_t::__ref();
#endif
  }

  template <size_t... Idxs>
  MDSPAN_FORCE_INLINE_FUNCTION
  static constexpr
  std::size_t _static_extent_impl(size_t n, std::integer_sequence<size_t, Idxs...>) noexcept {
    return _MDSPAN_FOLD_PLUS_RIGHT(((Idxs == n) ? Extents : 0), /* + ... + */ 0);
  }

  template <class, size_t...>
  friend class extents;

  template <class OtherIndexType, size_t... OtherExtents, size_t... Idxs>
  MDSPAN_INLINE_FUNCTION
  constexpr bool _eq_impl(std::experimental::extents<OtherIndexType, OtherExtents...>, false_type, index_sequence<Idxs...>) const noexcept { return false; }
  template <class OtherIndexType, size_t... OtherExtents, size_t... Idxs>
  MDSPAN_INLINE_FUNCTION
  constexpr bool _eq_impl(
    std::experimental::extents<OtherIndexType, OtherExtents...> other,
    true_type, index_sequence<Idxs...>
  ) const noexcept {
    return _MDSPAN_FOLD_AND(
      (__storage().template __get_n<Idxs>() == other.__storage().template __get_n<Idxs>()) /* && ... */
    );
  }

  template <class OtherIndexType, size_t... OtherExtents, size_t... Idxs>
  MDSPAN_INLINE_FUNCTION
  constexpr bool _not_eq_impl(std::experimental::extents<OtherIndexType, OtherExtents...>, false_type, index_sequence<Idxs...>) const noexcept { return true; }
  template <class OtherIndexType, size_t... OtherExtents, size_t... Idxs>
  MDSPAN_INLINE_FUNCTION
  constexpr bool _not_eq_impl(
    std::experimental::extents<OtherIndexType, OtherExtents...> other,
    true_type, index_sequence<Idxs...>
  ) const noexcept {
    return _MDSPAN_FOLD_OR(
      (__storage().template __get_n<Idxs>() != other.__storage().template __get_n<Idxs>()) /* || ... */
    );
  }

#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
  MDSPAN_INLINE_FUNCTION constexpr explicit
  extents(__base_t&& __b) noexcept
    : __base_t(::std::move(__b))
  { }
#endif

// public interface:
public:
  /* Defined above for use in the private code
  using rank_type = size_t;
  using index_type = ThisIndexType;
  */

  MDSPAN_INLINE_FUNCTION
  static constexpr rank_type rank() noexcept { return sizeof...(Extents); }
  MDSPAN_INLINE_FUNCTION
  static constexpr rank_type rank_dynamic() noexcept { return _MDSPAN_FOLD_PLUS_RIGHT((rank_type(Extents == dynamic_extent)), /* + ... + */ 0); }

  //--------------------------------------------------------------------------------
  // Constructors, Destructors, and Assignment

  // Default constructor
  MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr extents() noexcept = default;

  // Converting constructor
  MDSPAN_TEMPLATE_REQUIRES(
    class OtherIndexType, size_t... OtherExtents,
    /* requires */ (
      /* multi-stage check to protect from invalid pack expansion when sizes don't match? */
      decltype(detail::_check_compatible_extents(
        std::integral_constant<bool, sizeof...(Extents) == sizeof...(OtherExtents)>{},
        std::integer_sequence<size_t, Extents...>{},
        std::integer_sequence<size_t, OtherExtents...>{}
      ))::value
    )
  )
  MDSPAN_INLINE_FUNCTION
  MDSPAN_CONDITIONAL_EXPLICIT(
    (((Extents != dynamic_extent) && (OtherExtents == dynamic_extent)) || ...) ||
    (std::numeric_limits<index_type>::max() < std::numeric_limits<OtherIndexType>::max()))
  constexpr extents(const extents<OtherIndexType, OtherExtents...>& __other)
    noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    : __storage_{
#else
    : __base_t(__base_t{__storage_t{
#endif
        __other.__storage().__enable_psa_conversion()
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      }
#else
      }})
#endif
  {
    /* TODO: precondition check
     * other.extent(r) equals Er for each r for which Er is a static extent, and
     * either
     *   - sizeof...(OtherExtents) is zero, or
     *   - other.extent(r) is a representable value of type index_type for all rank index r of other
     */
  }

#ifdef __NVCC__
    MDSPAN_TEMPLATE_REQUIRES(
    class... Integral,
    /* requires */ (
      // TODO: check whether the other version works with newest NVCC, doesn't with 11.4
      // NVCC seems to pick up rank_dynamic from the wrong extents type???
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_convertible, Integral, index_type) /* && ... */) &&
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_nothrow_constructible, index_type, Integral) /* && ... */) &&
      // NVCC chokes on the fold thingy here so wrote the workaround
      ((sizeof...(Integral) == detail::_count_dynamic_extents<Extents...>::val) ||
       (sizeof...(Integral) == sizeof...(Extents)))
      )
    )
#else
    MDSPAN_TEMPLATE_REQUIRES(
    class... Integral,
    /* requires */ (
       _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_convertible, Integral, index_type) /* && ... */) &&
       _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_nothrow_constructible, index_type, Integral) /* && ... */) &&
       ((sizeof...(Integral) == rank_dynamic()) || (sizeof...(Integral) == rank()))
      )
    )
#endif
  MDSPAN_INLINE_FUNCTION
  explicit constexpr extents(Integral... exts) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    : __storage_{
#else
    : __base_t(__base_t{typename __base_t::__stored_type{
#endif
      std::conditional_t<sizeof...(Integral)==rank_dynamic(),
        detail::__construct_psa_from_dynamic_exts_values_tag_t,
        detail::__construct_psa_from_all_exts_values_tag_t>(),
        static_cast<index_type>(exts)...
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      }
#else
      }})
#endif
  {
    /* TODO: precondition check
     * If sizeof...(IndexTypes) != rank_dynamic() is true, exts_arr[r] equals Er for each r for which Er is a static extent, and
     * either
     *   - sizeof...(exts) == 0 is true, or
     *   - each element of exts is nonnegative and is a representable value of type index_type.
     */
  }

    // TODO: check whether this works with newest NVCC, doesn't with 11.4
#ifdef __NVCC__
  // NVCC seems to pick up rank_dynamic from the wrong extents type???
  // NVCC chokes on the fold thingy here so wrote the workaround
  MDSPAN_TEMPLATE_REQUIRES(
    class IndexType, size_t N,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, IndexType, index_type) &&
      _MDSPAN_TRAIT(is_nothrow_constructible, index_type, IndexType) &&
      ((N == detail::_count_dynamic_extents<Extents...>::val) ||
       (N == sizeof...(Extents)))
    )
  )
#else
    MDSPAN_TEMPLATE_REQUIRES(
        class IndexType, size_t N,
        /* requires */ (
          _MDSPAN_TRAIT(is_convertible, IndexType, index_type) &&
          _MDSPAN_TRAIT(is_nothrow_constructible, index_type, IndexType) &&
          (N == rank() || N == rank_dynamic())
    )
  )
#endif
  MDSPAN_CONDITIONAL_EXPLICIT(N != rank_dynamic())
  MDSPAN_INLINE_FUNCTION
  constexpr
  extents(std::array<IndexType, N> const& exts) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    : __storage_{
#else
    : __base_t(__base_t{typename __base_t::__stored_type{
#endif
      std::conditional_t<N==rank_dynamic(),
        detail::__construct_psa_from_dynamic_exts_array_tag_t<0>,
        detail::__construct_psa_from_all_exts_array_tag_t>(),
      std::array<IndexType,N>{exts}
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      }
#else
      }})
#endif
  {
    /* TODO: precondition check
     * If N != rank_dynamic() is true, exts[r] equals Er for each r for which Er is a static extent, and
     * either
     *   - N is zero, or
     *   - exts[r] is nonnegative and is a representable value of type index_type for all rank index r
     */
  }

#ifdef __cpp_lib_span
  // TODO: check whether the below works with newest NVCC, doesn't with 11.4
#ifdef __NVCC__
  // NVCC seems to pick up rank_dynamic from the wrong extents type???
  // NVCC chokes on the fold thingy here so wrote the workaround
  MDSPAN_TEMPLATE_REQUIRES(
    class IndexType, size_t N,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, IndexType, index_type) &&
      _MDSPAN_TRAIT(is_nothrow_constructible, index_type, IndexType) &&
      ((N == detail::_count_dynamic_extents<Extents...>::val) ||
       (N == sizeof...(Extents)))
    )
  )
#else
    MDSPAN_TEMPLATE_REQUIRES(
        class IndexType, size_t N,
        /* requires */ (
          _MDSPAN_TRAIT(is_convertible, IndexType, index_type) &&
          _MDSPAN_TRAIT(is_nothrow_constructible, index_type, IndexType) &&
          (N == rank() || N == rank_dynamic())
    )
  )
#endif
  MDSPAN_CONDITIONAL_EXPLICIT(N != rank_dynamic())
  MDSPAN_INLINE_FUNCTION
  constexpr
  extents(std::span<IndexType, N> exts) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    : __storage_{
#else
    : __base_t(__base_t{typename __base_t::__stored_type{
#endif
      std::conditional_t<N==rank_dynamic(),
        detail::__construct_psa_from_dynamic_exts_array_tag_t<0>,
        detail::__construct_psa_from_all_exts_array_tag_t>(),
      exts
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      }
#else
      }})
#endif
  {
    /* TODO: precondition check
     * If N != rank_dynamic() is true, exts[r] equals Er for each r for which Er is a static extent, and
     * either
     *   - N is zero, or
     *   - exts[r] is nonnegative and is a representable value of type index_type for all rank index r
     */
  }
#endif

  // Need this constructor for some submdspan implementation stuff
  // for the layout_stride case where I use an extents object for strides
  MDSPAN_INLINE_FUNCTION
  constexpr explicit
  extents(__storage_t const& sto ) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    : __storage_{
#else
    : __base_t(__base_t{
#endif
        sto
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      }
#else
      })
#endif
  { }

  //--------------------------------------------------------------------------------

  MDSPAN_INLINE_FUNCTION
  static constexpr
  size_t static_extent(size_t n) noexcept {
    // Can't do assert here since that breaks true constexpr ness
    // assert(n<rank());
    return _static_extent_impl(n, std::make_integer_sequence<size_t, sizeof...(Extents)>{});
  }

  MDSPAN_INLINE_FUNCTION
  constexpr
  index_type extent(size_t n) const noexcept {
    // Can't do assert here since that breaks true constexpr ness
    // assert(n<rank());
    return __storage().__get(n);
  }

  //--------------------------------------------------------------------------------

  template<class OtherIndexType, size_t... RHS>
  MDSPAN_INLINE_FUNCTION
  friend constexpr bool operator==(extents const& lhs, extents<OtherIndexType, RHS...> const& rhs) noexcept {
    return lhs._eq_impl(
      rhs, std::integral_constant<bool, (sizeof...(RHS) == rank())>{},
      make_index_sequence<sizeof...(RHS)>{}
    );
  }

#if !(MDSPAN_HAS_CXX_20)
  template<class OtherIndexType, size_t... RHS>
  MDSPAN_INLINE_FUNCTION
  friend constexpr bool operator!=(extents const& lhs, extents<OtherIndexType, RHS...> const& rhs) noexcept {
    return lhs._not_eq_impl(
      rhs, std::integral_constant<bool, (sizeof...(RHS) == rank())>{},
      make_index_sequence<sizeof...(RHS)>{}
    );
  }
#endif

  // End of public interface

public:  // (but not really)

  MDSPAN_INLINE_FUNCTION static constexpr
  extents __make_extents_impl(detail::__partially_static_sizes<index_type, size_t,Extents...>&& __bs) noexcept {
    // This effectively amounts to a sideways cast that can be done in a constexpr
    // context, but we have to do it to handle the case where the extents and the
    // strides could accidentally end up with the same types in their hierarchies
    // somehow (which would cause layout_stride::mapping to not be standard_layout)
    return extents(
#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      __base_t{
#endif
        ::std::move(__bs.template __with_tag<detail::__extents_tag>())
#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      }
#endif
    );
  }

  template <size_t N>
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr
  index_type __extent() const noexcept {
    return __storage().template __get_n<N>();
  }

  template <size_t N, size_t Default=dynamic_extent>
  MDSPAN_INLINE_FUNCTION
  static constexpr
  index_type __static_extent() noexcept {
    return __storage_t::template __get_static_n<N, Default>();
  }

};

namespace detail {

template <class IndexType, size_t Rank, class Extents = ::std::experimental::extents<IndexType>>
struct __make_dextents;

template <class IndexType, size_t Rank, size_t... ExtentsPack>
struct __make_dextents<IndexType, Rank, ::std::experimental::extents<IndexType, ExtentsPack...>> {
  using type = typename __make_dextents<IndexType, Rank - 1,
    ::std::experimental::extents<IndexType, ::std::experimental::dynamic_extent, ExtentsPack...>>::type;
};

template <class IndexType, size_t... ExtentsPack>
struct __make_dextents<IndexType, 0, ::std::experimental::extents<IndexType, ExtentsPack...>> {
  using type = ::std::experimental::extents<IndexType, ExtentsPack...>;
};

} // end namespace detail

template <class IndexType, size_t Rank>
using dextents = typename detail::__make_dextents<IndexType, Rank>::type;

#if defined(_MDSPAN_USE_CLASS_TEMPLATE_ARGUMENT_DEDUCTION)
template <class... IndexTypes>
extents(IndexTypes...)
  -> extents<size_t, size_t((IndexTypes(), ::std::experimental::dynamic_extent))...>;
#endif

namespace detail {

template <class T>
struct __is_extents : ::std::false_type {};

template <class IndexType, size_t... ExtentsPack>
struct __is_extents<::std::experimental::extents<IndexType, ExtentsPack...>> : ::std::true_type {};

template <class T>
static constexpr bool __is_extents_v = __is_extents<T>::value;

template <typename Extents>
struct __extents_to_partially_static_sizes;

template <class IndexType, size_t... ExtentsPack>
struct __extents_to_partially_static_sizes<::std::experimental::extents<IndexType, ExtentsPack...>> {
  using type = detail::__partially_static_sizes<
          typename ::std::experimental::extents<IndexType, ExtentsPack...>::index_type, size_t, 
          ExtentsPack...>;
};

template <typename Extents>
using __extents_to_partially_static_sizes_t = typename __extents_to_partially_static_sizes<Extents>::type;

} // end namespace detail
} // end namespace experimental
} // end namespace std
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/extents.hpp
//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/layout_stride.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
#endif

#ifdef __cpp_lib_span
#endif
#if defined(_MDSPAN_USE_CONCEPTS) && MDSPAN_HAS_CXX_20
#endif

namespace std {
namespace experimental {

struct layout_left {
  template<class Extents>
    class mapping;
};
struct layout_right {
  template<class Extents>
    class mapping;
};

namespace detail {
  template<class Layout, class Mapping>
  constexpr bool __is_mapping_of =
    is_same<typename Layout::template mapping<typename Mapping::extents_type>, Mapping>::value;

#if defined(_MDSPAN_USE_CONCEPTS) && MDSPAN_HAS_CXX_20
  template<class M>
  concept __layout_mapping_alike = requires {
    requires __is_extents<typename M::extents_type>::value;
    { M::is_always_strided() } -> same_as<bool>;
    { M::is_always_exhaustive() } -> same_as<bool>;
    { M::is_always_unique() } -> same_as<bool>;
    bool_constant<M::is_always_strided()>::value;
    bool_constant<M::is_always_exhaustive()>::value;
    bool_constant<M::is_always_unique()>::value;
  };
#endif
} // namespace detail

struct layout_stride {
  template <class Extents>
  class mapping
#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    : private detail::__no_unique_address_emulation<
        detail::__compressed_pair<
          Extents,
          std::array<typename Extents::index_type, Extents::rank()>
        >
      >
#endif
  {
  public:
    using extents_type = Extents;
    using index_type = typename extents_type::index_type;
    using size_type = typename extents_type::size_type;
    using rank_type = typename extents_type::rank_type;
    using layout_type = layout_stride;

    // This could be a `requires`, but I think it's better and clearer as a `static_assert`.
    static_assert(detail::__is_extents_v<Extents>, "std::experimental::layout_stride::mapping must be instantiated with a specialization of std::experimental::extents.");

  private:

    //----------------------------------------------------------------------------

    using __strides_storage_t = array<index_type, extents_type::rank()>;//::std::experimental::dextents<index_type, extents_type::rank()>;
    using __member_pair_t = detail::__compressed_pair<extents_type, __strides_storage_t>;

#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    _MDSPAN_NO_UNIQUE_ADDRESS __member_pair_t __members;
#else
    using __base_t = detail::__no_unique_address_emulation<__member_pair_t>;
#endif

    MDSPAN_FORCE_INLINE_FUNCTION constexpr __strides_storage_t const&
    __strides_storage() const noexcept {
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      return __members.__second();
#else
      return this->__base_t::__ref().__second();
#endif
    }
    MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 __strides_storage_t&
    __strides_storage() noexcept {
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      return __members.__second();
#else
      return this->__base_t::__ref().__second();
#endif
    }

    //----------------------------------------------------------------------------

    template <class>
    friend class mapping;

    //----------------------------------------------------------------------------

    // Workaround for non-deducibility of the index sequence template parameter if it's given at the top level
    template <class>
    struct __deduction_workaround;

    template <size_t... Idxs>
    struct __deduction_workaround<index_sequence<Idxs...>>
    {
      template <class OtherExtents>
      MDSPAN_INLINE_FUNCTION
      static constexpr bool _eq_impl(mapping const& self, mapping<OtherExtents> const& other) noexcept {
        return _MDSPAN_FOLD_AND((self.stride(Idxs) == other.stride(Idxs)) /* && ... */);
      }
      template <class OtherExtents>
      MDSPAN_INLINE_FUNCTION
      static constexpr bool _not_eq_impl(mapping const& self, mapping<OtherExtents> const& other) noexcept {
        return _MDSPAN_FOLD_OR((self.stride(Idxs) != other.stride(Idxs)) /* || ... */);
      }

      template <class... Integral>
      MDSPAN_FORCE_INLINE_FUNCTION
      static constexpr size_t _call_op_impl(mapping const& self, Integral... idxs) noexcept {
        return _MDSPAN_FOLD_PLUS_RIGHT((idxs * self.stride(Idxs)), /* + ... + */ 0);
      }

      MDSPAN_INLINE_FUNCTION
      static constexpr size_t _req_span_size_impl(mapping const& self) noexcept {
        // assumes no negative strides; not sure if I'm allowed to assume that or not
        return __impl::_call_op_impl(self, (self.extents().template __extent<Idxs>() - 1)...) + 1;
      }

      template<class OtherMapping>
      MDSPAN_INLINE_FUNCTION
      static constexpr const __strides_storage_t fill_strides(const OtherMapping& map) {
        return __strides_storage_t{static_cast<index_type>(map.stride(Idxs))...};
      }

      MDSPAN_INLINE_FUNCTION
      static constexpr const __strides_storage_t& fill_strides(const __strides_storage_t& s) {
        return s;
      }

      template<class IntegralType>
      MDSPAN_INLINE_FUNCTION
      static constexpr const __strides_storage_t fill_strides(const array<IntegralType,extents_type::rank()>& s) {
        return __strides_storage_t{static_cast<index_type>(s[Idxs])...};
      }

#ifdef __cpp_lib_span
      template<class IntegralType>
      MDSPAN_INLINE_FUNCTION
      static constexpr const __strides_storage_t fill_strides(const span<IntegralType,extents_type::rank()>& s) {
        return __strides_storage_t{static_cast<index_type>(s[Idxs])...};
      }
#endif

      MDSPAN_INLINE_FUNCTION
      static constexpr const __strides_storage_t fill_strides(
        detail::__extents_to_partially_static_sizes_t<
          ::std::experimental::dextents<index_type, extents_type::rank()>>&& s) {
        return __strides_storage_t{static_cast<index_type>(s.template __get_n<Idxs>())...};
      }

      template<size_t K>
      MDSPAN_INLINE_FUNCTION
      static constexpr size_t __return_zero() { return 0; }

      template<class Mapping>
      MDSPAN_INLINE_FUNCTION
      static constexpr typename Mapping::index_type
        __OFFSET(const Mapping& m) { return m(__return_zero<Idxs>()...); }
    };

    // Can't use defaulted parameter in the __deduction_workaround template because of a bug in MSVC warning C4348.
    using __impl = __deduction_workaround<make_index_sequence<Extents::rank()>>;

    //----------------------------------------------------------------------------

#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    MDSPAN_INLINE_FUNCTION constexpr explicit
    mapping(__member_pair_t&& __m) : __members(::std::move(__m)) {}
#else
    MDSPAN_INLINE_FUNCTION constexpr explicit
    mapping(__base_t&& __b) : __base_t(::std::move(__b)) {}
#endif

  public: // but not really
    MDSPAN_INLINE_FUNCTION
    static constexpr mapping
    __make_mapping(
      detail::__extents_to_partially_static_sizes_t<Extents>&& __exts,
      detail::__extents_to_partially_static_sizes_t<
        ::std::experimental::dextents<index_type, Extents::rank()>>&& __strs
    ) noexcept {
      // call the private constructor we created for this purpose
      return mapping(
#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        __base_t{
#endif
          __member_pair_t(
            extents_type::__make_extents_impl(::std::move(__exts)),
            __strides_storage_t{__impl::fill_strides(::std::move(__strs))}
          )
#if !defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        }
#endif
      );
    }
    //----------------------------------------------------------------------------

  public:

    //--------------------------------------------------------------------------------

    MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mapping() noexcept = default;
    MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mapping(mapping const&) noexcept = default;

    MDSPAN_TEMPLATE_REQUIRES(
      class IntegralTypes,
      /* requires */ (
        // MSVC 19.32 does not like using index_type here, requires the typename Extents::index_type
        // error C2641: cannot deduce template arguments for 'std::experimental::layout_stride::mapping'
        _MDSPAN_TRAIT(is_convertible, const remove_const_t<IntegralTypes>&, typename Extents::index_type) &&
        _MDSPAN_TRAIT(is_nothrow_constructible, typename Extents::index_type, const remove_const_t<IntegralTypes>&)
      )
    )
    MDSPAN_INLINE_FUNCTION
    constexpr
    mapping(
      extents_type const& e,
      array<IntegralTypes, extents_type::rank()> const& s
    ) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      : __members{
#else
      : __base_t(__base_t{__member_pair_t(
#endif
          e, __strides_storage_t(__impl::fill_strides(s))
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        }
#else
        )})
#endif
    {
      /*
       * TODO: check preconditions
       * - s[i] > 0 is true for all i in the range [0, rank_ ).
       * - REQUIRED-SPAN-SIZE(e, s) is a representable value of type index_type ([basic.fundamental]).
       * - If rank_ is greater than 0, then there exists a permutation P of the integers in the
       *   range [0, rank_), such that s[ pi ] >= s[ pi − 1 ] * e.extent( pi − 1 ) is true for
       *   all i in the range [1, rank_ ), where pi is the ith element of P.
       */
    }

#ifdef __cpp_lib_span
    MDSPAN_TEMPLATE_REQUIRES(
      class IntegralTypes,
      /* requires */ (
        // MSVC 19.32 does not like using index_type here, requires the typename Extents::index_type
        // error C2641: cannot deduce template arguments for 'std::experimental::layout_stride::mapping'
        _MDSPAN_TRAIT(is_convertible, const remove_const_t<IntegralTypes>&, typename Extents::index_type) &&
        _MDSPAN_TRAIT(is_nothrow_constructible, typename Extents::index_type, const remove_const_t<IntegralTypes>&)
      )
    )
    MDSPAN_INLINE_FUNCTION
    constexpr
    mapping(
      extents_type const& e,
      span<IntegralTypes, extents_type::rank()> const& s
    ) noexcept
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      : __members{
#else
      : __base_t(__base_t{__member_pair_t(
#endif
          e, __strides_storage_t(__impl::fill_strides(s))
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        }
#else
        )})
#endif
    {
      /*
       * TODO: check preconditions
       * - s[i] > 0 is true for all i in the range [0, rank_ ).
       * - REQUIRED-SPAN-SIZE(e, s) is a representable value of type index_type ([basic.fundamental]).
       * - If rank_ is greater than 0, then there exists a permutation P of the integers in the
       *   range [0, rank_), such that s[ pi ] >= s[ pi − 1 ] * e.extent( pi − 1 ) is true for
       *   all i in the range [1, rank_ ), where pi is the ith element of P.
       */
    }
#endif // __cpp_lib_span

#if !(defined(_MDSPAN_USE_CONCEPTS) && MDSPAN_HAS_CXX_20)
    MDSPAN_TEMPLATE_REQUIRES(
      class StridedLayoutMapping,
      /* requires */ (
        _MDSPAN_TRAIT(is_constructible, extents_type, typename StridedLayoutMapping::extents_type) &&
        detail::__is_mapping_of<typename StridedLayoutMapping::layout_type, StridedLayoutMapping> &&
        StridedLayoutMapping::is_always_unique() &&
        StridedLayoutMapping::is_always_strided()
      )
    )
#else
    template<class StridedLayoutMapping>
    requires(
         detail::__layout_mapping_alike<StridedLayoutMapping> &&
         _MDSPAN_TRAIT(is_constructible, extents_type, typename StridedLayoutMapping::extents_type) &&
         StridedLayoutMapping::is_always_unique() &&
         StridedLayoutMapping::is_always_strided()
    )
#endif
    MDSPAN_CONDITIONAL_EXPLICIT(
      (!is_convertible<typename StridedLayoutMapping::extents_type, extents_type>::value) &&
      (detail::__is_mapping_of<layout_left, StridedLayoutMapping> ||
       detail::__is_mapping_of<layout_right, StridedLayoutMapping> ||
       detail::__is_mapping_of<layout_stride, StridedLayoutMapping>)
    ) // needs two () due to comma
    MDSPAN_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14
    mapping(StridedLayoutMapping const& other) noexcept // NOLINT(google-explicit-constructor)
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      : __members{
#else
      : __base_t(__base_t{__member_pair_t(
#endif
          other.extents(), __strides_storage_t(__impl::fill_strides(other))
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
        }
#else
        )})
#endif
    {
      /*
       * TODO: check preconditions
       * - other.stride(i) > 0 is true for all i in the range [0, rank_ ).
       * - other.required_span_size() is a representable value of type index_type ([basic.fundamental]).
       * - OFFSET(other) == 0
       */
    }

    //--------------------------------------------------------------------------------

    MDSPAN_INLINE_FUNCTION_DEFAULTED _MDSPAN_CONSTEXPR_14_DEFAULTED
    mapping& operator=(mapping const&) noexcept = default;

    MDSPAN_INLINE_FUNCTION constexpr const extents_type& extents() const noexcept {
#if defined(_MDSPAN_USE_ATTRIBUTE_NO_UNIQUE_ADDRESS)
      return __members.__first();
#else
      return this->__base_t::__ref().__first();
#endif
    };

    MDSPAN_INLINE_FUNCTION
    constexpr array< index_type, extents_type::rank() > strides() const noexcept {
      return __strides_storage();
    }

    MDSPAN_INLINE_FUNCTION
    constexpr index_type required_span_size() const noexcept {
      index_type span_size = 1;
      for(unsigned r = 0; r < extents_type::rank(); r++) {
        // Return early if any of the extents are zero
        if(extents().extent(r)==0) return 0;
        span_size = std::max(span_size, static_cast<index_type>(extents().extent(r) * __strides_storage()[r]));
      }
      return span_size;
    }

    MDSPAN_TEMPLATE_REQUIRES(
      class... Indices,
      /* requires */ (
        sizeof...(Indices) == Extents::rank() &&
        _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_convertible, Indices, index_type) /*&& ...*/ ) &&
        _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_nothrow_constructible, index_type, Indices) /*&& ...*/)
      )
    )
    MDSPAN_FORCE_INLINE_FUNCTION
    constexpr size_t operator()(Indices... idxs) const noexcept {
      return __impl::_call_op_impl(*this, static_cast<index_type>(idxs)...);
    }

    MDSPAN_INLINE_FUNCTION static constexpr bool is_always_unique() noexcept { return true; }
    MDSPAN_INLINE_FUNCTION static constexpr bool is_always_exhaustive() noexcept {
      return false;
    }
    MDSPAN_INLINE_FUNCTION static constexpr bool is_always_strided() noexcept { return true; }

    MDSPAN_INLINE_FUNCTION static constexpr bool is_unique() noexcept { return true; }
    MDSPAN_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 bool is_exhaustive() const noexcept {
// TODO @testing test layout_stride is_exhaustive()
// FIXME CUDA
#ifdef __CUDA_ARCH__
      return false;
#else
      auto rem = array<size_t, Extents::rank()>{ };
      std::iota(rem.begin(), rem.end(), size_t(0));
      auto next_idx_iter = std::find_if(
        rem.begin(), rem.end(),
        [&](size_t i) { return this->stride(i) == 1;  }
      );
      if(next_idx_iter != rem.end()) {
        size_t prev_stride_times_prev_extent =
          this->extents().extent(*next_idx_iter) * this->stride(*next_idx_iter);
        // "remove" the index
        constexpr auto removed_index_sentinel = static_cast<size_t>(-1);
        *next_idx_iter = removed_index_sentinel;
        size_t found_count = 1;
        while (found_count != Extents::rank()) {
          next_idx_iter = std::find_if(
            rem.begin(), rem.end(),
            [&](size_t i) {
              return i != removed_index_sentinel
                && static_cast<size_t>(this->extents().extent(i)) == prev_stride_times_prev_extent;
            }
          );
          if (next_idx_iter != rem.end()) {
            // "remove" the index
            *next_idx_iter = removed_index_sentinel;
            ++found_count;
            prev_stride_times_prev_extent = stride(*next_idx_iter) * this->extents().extent(*next_idx_iter);
          } else { break; }
        }
        return found_count == Extents::rank();
      }
      return false;
#endif
    }
    MDSPAN_INLINE_FUNCTION static constexpr bool is_strided() noexcept { return true; }

    MDSPAN_INLINE_FUNCTION
    constexpr index_type stride(rank_type r) const noexcept {
      return __strides_storage()[r];
    }

#if !(defined(_MDSPAN_USE_CONCEPTS) && MDSPAN_HAS_CXX_20)
    MDSPAN_TEMPLATE_REQUIRES(
      class StridedLayoutMapping,
      /* requires */ (
        detail::__is_mapping_of<typename StridedLayoutMapping::layout_type, StridedLayoutMapping> &&
        (extents_type::rank() == StridedLayoutMapping::extents_type::rank()) &&
        StridedLayoutMapping::is_always_strided()
      )
    )
#else
    template<class StridedLayoutMapping>
    requires(
         detail::__layout_mapping_alike<StridedLayoutMapping> &&
         (extents_type::rank() == StridedLayoutMapping::extents_type::rank()) &&
         StridedLayoutMapping::is_always_strided()
    )
#endif
    MDSPAN_INLINE_FUNCTION
    friend constexpr bool operator==(const mapping& x, const StridedLayoutMapping& y) noexcept {
      bool strides_match = true;
      for(rank_type r = 0; r < extents_type::rank(); r++)
        strides_match = strides_match && (x.stride(r) == y.stride(r));
      return (x.extents() == y.extents()) &&
             (__impl::__OFFSET(y)== static_cast<typename StridedLayoutMapping::index_type>(0)) &&
             strides_match;
    }

    // This one is not technically part of the proposal. Just here to make implementation a bit more optimal hopefully
    MDSPAN_TEMPLATE_REQUIRES(
      class OtherExtents,
      /* requires */ (
        (extents_type::rank() == OtherExtents::rank())
      )
    )
    MDSPAN_INLINE_FUNCTION
    friend constexpr bool operator==(mapping const& lhs, mapping<OtherExtents> const& rhs) noexcept {
      return __impl::_eq_impl(lhs, rhs);
    }

#if !MDSPAN_HAS_CXX_20
    MDSPAN_TEMPLATE_REQUIRES(
      class StridedLayoutMapping,
      /* requires */ (
        detail::__is_mapping_of<typename StridedLayoutMapping::layout_type, StridedLayoutMapping> &&
        (extents_type::rank() == StridedLayoutMapping::extents_type::rank()) &&
        StridedLayoutMapping::is_always_strided()
      )
    )
    MDSPAN_INLINE_FUNCTION
    friend constexpr bool operator!=(const mapping& x, const StridedLayoutMapping& y) noexcept {
      return not (x == y);
    }

    MDSPAN_TEMPLATE_REQUIRES(
      class OtherExtents,
      /* requires */ (
        (extents_type::rank() == OtherExtents::rank())
      )
    )
    MDSPAN_INLINE_FUNCTION
    friend constexpr bool operator!=(mapping const& lhs, mapping<OtherExtents> const& rhs) noexcept {
      return __impl::_not_eq_impl(lhs, rhs);
    }
#endif

  };
};

} // end namespace experimental
} // end namespace std
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/layout_stride.hpp

namespace std {
namespace experimental {

//==============================================================================
template <class Extents>
class layout_right::mapping {
  public:
    using extents_type = Extents;
    using index_type = typename extents_type::index_type;
    using size_type = typename extents_type::size_type;
    using rank_type = typename extents_type::rank_type;
    using layout_type = layout_right;
  private:

    static_assert(detail::__is_extents_v<extents_type>, "std::experimental::layout_right::mapping must be instantiated with a specialization of std::experimental::extents.");

    template <class>
    friend class mapping;

    // i0+(i1 + E(1)*(i2 + E(2)*i3))
    template <size_t r, size_t Rank>
    struct __rank_count {};

    template <size_t r, size_t Rank, class I, class... Indices>
    _MDSPAN_HOST_DEVICE
    constexpr index_type __compute_offset(
      index_type offset, __rank_count<r,Rank>, const I& i, Indices... idx) const {
      return __compute_offset(offset * __extents.template __extent<r>() + i,__rank_count<r+1,Rank>(),  idx...);
    }

    template<class I, class ... Indices>
    _MDSPAN_HOST_DEVICE
    constexpr index_type __compute_offset(
      __rank_count<0,extents_type::rank()>, const I& i, Indices... idx) const {
      return __compute_offset(i,__rank_count<1,extents_type::rank()>(),idx...);
    }

    _MDSPAN_HOST_DEVICE
    constexpr index_type __compute_offset(size_t offset, __rank_count<extents_type::rank(), extents_type::rank()>) const {
      return static_cast<index_type>(offset);
    }

    _MDSPAN_HOST_DEVICE
    constexpr index_type __compute_offset(__rank_count<0,0>) const { return 0; }

  public:

    //--------------------------------------------------------------------------------

    MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mapping() noexcept = default;
    MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mapping(mapping const&) noexcept = default;

    _MDSPAN_HOST_DEVICE
    constexpr mapping(extents_type const& __exts) noexcept
      :__extents(__exts)
    { }

    MDSPAN_TEMPLATE_REQUIRES(
      class OtherExtents,
      /* requires */ (
        _MDSPAN_TRAIT(is_constructible, extents_type, OtherExtents)
      )
    )
    MDSPAN_CONDITIONAL_EXPLICIT((!is_convertible<OtherExtents, extents_type>::value)) // needs two () due to comma
    MDSPAN_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14
    mapping(mapping<OtherExtents> const& other) noexcept // NOLINT(google-explicit-constructor)
      :__extents(other.extents())
    {
       /*
        * TODO: check precondition
        * other.required_span_size() is a representable value of type index_type
        */
    }

    MDSPAN_TEMPLATE_REQUIRES(
      class OtherExtents,
      /* requires */ (
        _MDSPAN_TRAIT(is_constructible, extents_type, OtherExtents) &&
        (extents_type::rank() <= 1)
      )
    )
    MDSPAN_CONDITIONAL_EXPLICIT((!is_convertible<OtherExtents, extents_type>::value)) // needs two () due to comma
    MDSPAN_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14
    mapping(layout_left::mapping<OtherExtents> const& other) noexcept // NOLINT(google-explicit-constructor)
      :__extents(other.extents())
    {
       /*
        * TODO: check precondition
        * other.required_span_size() is a representable value of type index_type
        */
    }

    MDSPAN_TEMPLATE_REQUIRES(
      class OtherExtents,
      /* requires */ (
        _MDSPAN_TRAIT(is_constructible, extents_type, OtherExtents)
      )
    )
    MDSPAN_CONDITIONAL_EXPLICIT((extents_type::rank() > 0))
    MDSPAN_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14
    mapping(layout_stride::mapping<OtherExtents> const& other) // NOLINT(google-explicit-constructor)
      :__extents(other.extents())
    {
       /*
        * TODO: check precondition
        * other.required_span_size() is a representable value of type index_type
        */
       #ifndef __CUDA_ARCH__
       size_t stride = 1;
       for(rank_type r=__extents.rank(); r>0; r--) {
         if(stride != other.stride(r-1))
           throw std::runtime_error("Assigning layout_stride to layout_right with invalid strides.");
         stride *= __extents.extent(r-1);
       }
       #endif
    }

    MDSPAN_INLINE_FUNCTION_DEFAULTED _MDSPAN_CONSTEXPR_14_DEFAULTED mapping& operator=(mapping const&) noexcept = default;

    MDSPAN_INLINE_FUNCTION
    constexpr const extents_type& extents() const noexcept {
      return __extents;
    }

    MDSPAN_INLINE_FUNCTION
    constexpr index_type required_span_size() const noexcept {
      index_type value = 1;
      for(rank_type r=0; r != extents_type::rank(); ++r) value*=__extents.extent(r);
      return value;
    }

    //--------------------------------------------------------------------------------

    MDSPAN_TEMPLATE_REQUIRES(
      class... Indices,
      /* requires */ (
        (sizeof...(Indices) == extents_type::rank()) &&
        _MDSPAN_FOLD_AND(
           (_MDSPAN_TRAIT(is_convertible, Indices, index_type) &&
            _MDSPAN_TRAIT(is_nothrow_constructible, index_type, Indices))
        )
      )
    )
    _MDSPAN_HOST_DEVICE
    constexpr index_type operator()(Indices... idxs) const noexcept {
      return __compute_offset(__rank_count<0, extents_type::rank()>(), idxs...);
    }

    MDSPAN_INLINE_FUNCTION static constexpr bool is_always_unique() noexcept { return true; }
    MDSPAN_INLINE_FUNCTION static constexpr bool is_always_exhaustive() noexcept { return true; }
    MDSPAN_INLINE_FUNCTION static constexpr bool is_always_strided() noexcept { return true; }
    MDSPAN_INLINE_FUNCTION constexpr bool is_unique() const noexcept { return true; }
    MDSPAN_INLINE_FUNCTION constexpr bool is_exhaustive() const noexcept { return true; }
    MDSPAN_INLINE_FUNCTION constexpr bool is_strided() const noexcept { return true; }

    MDSPAN_INLINE_FUNCTION
    constexpr index_type stride(rank_type i) const noexcept {
      index_type value = 1;
      for(rank_type r=extents_type::rank()-1; r>i; r--) value*=__extents.extent(r);
      return value;
    }

    template<class OtherExtents>
    MDSPAN_INLINE_FUNCTION
    friend constexpr bool operator==(mapping const& lhs, mapping<OtherExtents> const& rhs) noexcept {
      return lhs.extents() == rhs.extents();
    }

    // In C++ 20 the not equal exists if equal is found
#if MDSPAN_HAS_CXX_20
    template<class OtherExtents>
    MDSPAN_INLINE_FUNCTION
    friend constexpr bool operator!=(mapping const& lhs, mapping<OtherExtents> const& rhs) noexcept {
      return lhs.extents() != rhs.extents();
    }
#endif

    // Not really public, but currently needed to implement fully constexpr useable submdspan:
    template<size_t N, class SizeType, size_t ... E, size_t ... Idx>
    constexpr index_type __get_stride(std::experimental::extents<SizeType, E...>,integer_sequence<size_t, Idx...>) const {
      return _MDSPAN_FOLD_TIMES_RIGHT((Idx>N? __extents.template __extent<Idx>():1),1);
    }
    template<size_t N>
    constexpr index_type __stride() const noexcept {
      return __get_stride<N>(__extents, make_index_sequence<extents_type::rank()>());
    }

private:
   _MDSPAN_NO_UNIQUE_ADDRESS extents_type __extents{};

};

} // end namespace experimental
} // end namespace std

//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/layout_right.hpp

namespace std {
namespace experimental {

template <
  class ElementType,
  class Extents,
  class LayoutPolicy = layout_right,
  class AccessorPolicy = default_accessor<ElementType>
>
class mdspan
{
private:
  static_assert(detail::__is_extents_v<Extents>, "std::experimental::mdspan's Extents template parameter must be a specialization of std::experimental::extents.");

  // Workaround for non-deducibility of the index sequence template parameter if it's given at the top level
  template <class>
  struct __deduction_workaround;

  template <size_t... Idxs>
  struct __deduction_workaround<index_sequence<Idxs...>>
  {
    MDSPAN_FORCE_INLINE_FUNCTION static constexpr
    size_t __size(mdspan const& __self) noexcept {
      return _MDSPAN_FOLD_TIMES_RIGHT((__self.__mapping_ref().extents().template __extent<Idxs>()), /* * ... * */ size_t(1));
    }
    MDSPAN_FORCE_INLINE_FUNCTION static constexpr
    bool __empty(mdspan const& __self) noexcept {
      return (__self.rank()>0) && _MDSPAN_FOLD_OR((__self.__mapping_ref().extents().template __extent<Idxs>()==index_type(0)));
    }
    template <class ReferenceType, class SizeType, size_t N>
    MDSPAN_FORCE_INLINE_FUNCTION static constexpr
    ReferenceType __callop(mdspan const& __self, const array<SizeType, N>& indices) noexcept {
      return __self.__accessor_ref().access(__self.__ptr_ref(), __self.__mapping_ref()(indices[Idxs]...));
    }
  };

public:

  //--------------------------------------------------------------------------------
  // Domain and codomain types

  using extents_type = Extents;
  using layout_type = LayoutPolicy;
  using accessor_type = AccessorPolicy;
  using mapping_type = typename layout_type::template mapping<extents_type>;
  using element_type = ElementType;
  using value_type = remove_cv_t<element_type>;
  using index_type = typename extents_type::index_type;
  using size_type = typename extents_type::size_type;
  using rank_type = typename extents_type::rank_type;
  using data_handle_type = typename accessor_type::data_handle_type;
  using reference = typename accessor_type::reference;

  MDSPAN_INLINE_FUNCTION static constexpr size_t rank() noexcept { return extents_type::rank(); }
  MDSPAN_INLINE_FUNCTION static constexpr size_t rank_dynamic() noexcept { return extents_type::rank_dynamic(); }
  MDSPAN_INLINE_FUNCTION static constexpr size_t static_extent(size_t r) noexcept { return extents_type::static_extent(r); }
  MDSPAN_INLINE_FUNCTION constexpr index_type extent(size_t r) const noexcept { return __mapping_ref().extents().extent(r); };

private:

  // Can't use defaulted parameter in the __deduction_workaround template because of a bug in MSVC warning C4348.
  using __impl = __deduction_workaround<make_index_sequence<extents_type::rank()>>;

  using __map_acc_pair_t = detail::__compressed_pair<mapping_type, accessor_type>;

public:

  //--------------------------------------------------------------------------------
  // [mdspan.basic.cons], mdspan constructors, assignment, and destructor

#if !MDSPAN_HAS_CXX_20
  MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mdspan() = default;
#else
  MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mdspan()
    requires(
       (rank_dynamic() > 0) &&
       _MDSPAN_TRAIT(is_default_constructible, data_handle_type) &&
       _MDSPAN_TRAIT(is_default_constructible, mapping_type) &&
       _MDSPAN_TRAIT(is_default_constructible, accessor_type)
     ) = default;
#endif
  MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mdspan(const mdspan&) = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mdspan(mdspan&&) = default;

  MDSPAN_TEMPLATE_REQUIRES(
    class... SizeTypes,
    /* requires */ (
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_convertible, SizeTypes, index_type) /* && ... */) &&
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_nothrow_constructible, index_type, SizeTypes) /* && ... */) &&
      ((sizeof...(SizeTypes) == rank()) || (sizeof...(SizeTypes) == rank_dynamic())) &&
      _MDSPAN_TRAIT(is_constructible, mapping_type, extents_type) &&
      _MDSPAN_TRAIT(is_default_constructible, accessor_type)
    )
  )
  MDSPAN_INLINE_FUNCTION
  explicit constexpr mdspan(data_handle_type p, SizeTypes... dynamic_extents)
    // TODO @proposal-bug shouldn't I be allowed to do `move(p)` here?
    : __members(std::move(p), __map_acc_pair_t(mapping_type(extents_type(static_cast<index_type>(std::move(dynamic_extents))...)), accessor_type()))
  { }

  MDSPAN_TEMPLATE_REQUIRES(
    class SizeType, size_t N,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, SizeType, index_type) &&
      _MDSPAN_TRAIT(is_nothrow_constructible, index_type, SizeType) &&
      ((N == rank()) || (N == rank_dynamic())) &&
      _MDSPAN_TRAIT(is_constructible, mapping_type, extents_type) &&
      _MDSPAN_TRAIT(is_default_constructible, accessor_type)
    )
  )
  MDSPAN_CONDITIONAL_EXPLICIT(N != rank_dynamic())
  MDSPAN_INLINE_FUNCTION
  constexpr mdspan(data_handle_type p, const array<SizeType, N>& dynamic_extents)
    : __members(std::move(p), __map_acc_pair_t(mapping_type(extents_type(dynamic_extents)), accessor_type()))
  { }

#ifdef __cpp_lib_span
  MDSPAN_TEMPLATE_REQUIRES(
    class SizeType, size_t N,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, SizeType, index_type) &&
      _MDSPAN_TRAIT(is_nothrow_constructible, index_type, SizeType) &&
      ((N == rank()) || (N == rank_dynamic())) &&
      _MDSPAN_TRAIT(is_constructible, mapping_type, extents_type) &&
      _MDSPAN_TRAIT(is_default_constructible, accessor_type)
    )
  )
  MDSPAN_CONDITIONAL_EXPLICIT(N != rank_dynamic())
  MDSPAN_INLINE_FUNCTION
  constexpr mdspan(data_handle_type p, span<SizeType, N> dynamic_extents)
    : __members(std::move(p), __map_acc_pair_t(mapping_type(extents_type(as_const(dynamic_extents))), accessor_type()))
  { }
#endif

  MDSPAN_FUNCTION_REQUIRES(
    (MDSPAN_INLINE_FUNCTION constexpr),
    mdspan, (data_handle_type p, const extents_type& exts), ,
    /* requires */ (_MDSPAN_TRAIT(is_default_constructible, accessor_type) &&
                    _MDSPAN_TRAIT(is_constructible, mapping_type, extents_type))
  ) : __members(std::move(p), __map_acc_pair_t(mapping_type(exts), accessor_type()))
  { }

  MDSPAN_FUNCTION_REQUIRES(
    (MDSPAN_INLINE_FUNCTION constexpr),
    mdspan, (data_handle_type p, const mapping_type& m), ,
    /* requires */ (_MDSPAN_TRAIT(is_default_constructible, accessor_type))
  ) : __members(std::move(p), __map_acc_pair_t(m, accessor_type()))
  { }

  MDSPAN_INLINE_FUNCTION
  constexpr mdspan(data_handle_type p, const mapping_type& m, const accessor_type& a)
    : __members(std::move(p), __map_acc_pair_t(m, a))
  { }

  MDSPAN_TEMPLATE_REQUIRES(
    class OtherElementType, class OtherExtents, class OtherLayoutPolicy, class OtherAccessor,
    /* requires */ (
      _MDSPAN_TRAIT(is_constructible, mapping_type, typename OtherLayoutPolicy::template mapping<OtherExtents>) &&
      _MDSPAN_TRAIT(is_constructible, accessor_type, OtherAccessor)
    )
  )
  MDSPAN_INLINE_FUNCTION
  constexpr mdspan(const mdspan<OtherElementType, OtherExtents, OtherLayoutPolicy, OtherAccessor>& other)
    : __members(other.__ptr_ref(), __map_acc_pair_t(other.__mapping_ref(), other.__accessor_ref()))
  {
      static_assert(_MDSPAN_TRAIT(is_constructible, data_handle_type, typename OtherAccessor::data_handle_type),"Incompatible data_handle_type for mdspan construction");
      static_assert(_MDSPAN_TRAIT(is_constructible, extents_type, OtherExtents),"Incompatible extents for mdspan construction");
      /*
       * TODO: Check precondition
       * For each rank index r of extents_type, static_extent(r) == dynamic_extent || static_extent(r) == other.extent(r) is true.
       */
  }

  /* Might need this on NVIDIA?
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~mdspan() = default;
  */

  MDSPAN_INLINE_FUNCTION_DEFAULTED _MDSPAN_CONSTEXPR_14_DEFAULTED mdspan& operator=(const mdspan&) = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED _MDSPAN_CONSTEXPR_14_DEFAULTED mdspan& operator=(mdspan&&) = default;

  //--------------------------------------------------------------------------------
  // [mdspan.basic.mapping], mdspan mapping domain multidimensional index to access codomain element

  #if MDSPAN_USE_BRACKET_OPERATOR
  MDSPAN_TEMPLATE_REQUIRES(
    class... SizeTypes,
    /* requires */ (
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_convertible, SizeTypes, index_type) /* && ... */) &&
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_nothrow_constructible, index_type, SizeTypes) /* && ... */) &&
      (rank() == sizeof...(SizeTypes))
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr reference operator[](SizeTypes... indices) const
  {
    return __accessor_ref().access(__ptr_ref(), __mapping_ref()(static_cast<index_type>(std::move(indices))...));
  }
  #endif

  MDSPAN_TEMPLATE_REQUIRES(
    class SizeType,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, SizeType, index_type) &&
      _MDSPAN_TRAIT(is_nothrow_constructible, index_type, SizeType)
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr reference operator[](const array<SizeType, rank()>& indices) const
  {
    return __impl::template __callop<reference>(*this, indices);
  }

  #ifdef __cpp_lib_span
  MDSPAN_TEMPLATE_REQUIRES(
    class SizeType,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, SizeType, index_type) &&
      _MDSPAN_TRAIT(is_nothrow_constructible, index_type, SizeType)
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr reference operator[](span<SizeType, rank()> indices) const
  {
    return __impl::template __callop<reference>(*this, indices);
  }
  #endif // __cpp_lib_span

  #if !MDSPAN_USE_BRACKET_OPERATOR
  MDSPAN_TEMPLATE_REQUIRES(
    class Index,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, Index, index_type) &&
      _MDSPAN_TRAIT(is_nothrow_constructible, index_type, Index) &&
      extents_type::rank() == 1
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr reference operator[](Index idx) const
  {
    return __accessor_ref().access(__ptr_ref(), __mapping_ref()(static_cast<index_type>(std::move(idx))));
  }
  #endif

  #if MDSPAN_USE_PAREN_OPERATOR
  MDSPAN_TEMPLATE_REQUIRES(
    class... SizeTypes,
    /* requires */ (
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_convertible, SizeTypes, index_type) /* && ... */) &&
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_nothrow_constructible, index_type, SizeTypes) /* && ... */) &&
      extents_type::rank() == sizeof...(SizeTypes)
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr reference operator()(SizeTypes... indices) const
  {
    return __accessor_ref().access(__ptr_ref(), __mapping_ref()(static_cast<index_type>(std::move(indices))...));
  }

  MDSPAN_TEMPLATE_REQUIRES(
    class SizeType,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, SizeType, index_type) &&
      _MDSPAN_TRAIT(is_nothrow_constructible, index_type, SizeType)
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr reference operator()(const array<SizeType, rank()>& indices) const
  {
    return __impl::template __callop<reference>(*this, indices);
  }

  #ifdef __cpp_lib_span
  MDSPAN_TEMPLATE_REQUIRES(
    class SizeType,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, SizeType, index_type) &&
      _MDSPAN_TRAIT(is_nothrow_constructible, index_type, SizeType)
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr reference operator()(span<SizeType, rank()> indices) const
  {
    return __impl::template __callop<reference>(*this, indices);
  }
  #endif // __cpp_lib_span
  #endif // MDSPAN_USE_PAREN_OPERATOR

  MDSPAN_INLINE_FUNCTION constexpr size_t size() const noexcept {
    return __impl::__size(*this);
  };

  MDSPAN_INLINE_FUNCTION constexpr bool empty() const noexcept {
    return __impl::__empty(*this);
  };

  MDSPAN_INLINE_FUNCTION
  friend constexpr void swap(mdspan& x, mdspan& y) noexcept {
    swap(x.__ptr_ref(), y.__ptr_ref());
    swap(x.__mapping_ref(), y.__mapping_ref());
    swap(x.__accessor_ref(), y.__accessor_ref());
  }

  //--------------------------------------------------------------------------------
  // [mdspan.basic.domobs], mdspan observers of the domain multidimensional index space

  MDSPAN_INLINE_FUNCTION constexpr const extents_type& extents() const noexcept { return __mapping_ref().extents(); };
  MDSPAN_INLINE_FUNCTION constexpr const data_handle_type& data_handle() const noexcept { return __ptr_ref(); };
  MDSPAN_INLINE_FUNCTION constexpr const mapping_type& mapping() const noexcept { return __mapping_ref(); };
  MDSPAN_INLINE_FUNCTION constexpr const accessor_type& accessor() const noexcept { return __accessor_ref(); };

  //--------------------------------------------------------------------------------
  // [mdspan.basic.obs], mdspan observers of the mapping

  MDSPAN_INLINE_FUNCTION static constexpr bool is_always_unique() noexcept { return mapping_type::is_always_unique(); };
  MDSPAN_INLINE_FUNCTION static constexpr bool is_always_exhaustive() noexcept { return mapping_type::is_always_exhaustive(); };
  MDSPAN_INLINE_FUNCTION static constexpr bool is_always_strided() noexcept { return mapping_type::is_always_strided(); };

  MDSPAN_INLINE_FUNCTION constexpr bool is_unique() const noexcept { return __mapping_ref().is_unique(); };
  MDSPAN_INLINE_FUNCTION constexpr bool is_exhaustive() const noexcept { return __mapping_ref().is_exhaustive(); };
  MDSPAN_INLINE_FUNCTION constexpr bool is_strided() const noexcept { return __mapping_ref().is_strided(); };
  MDSPAN_INLINE_FUNCTION constexpr index_type stride(size_t r) const { return __mapping_ref().stride(r); };

private:

  detail::__compressed_pair<data_handle_type, __map_acc_pair_t> __members{};

  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 data_handle_type& __ptr_ref() noexcept { return __members.__first(); }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr data_handle_type const& __ptr_ref() const noexcept { return __members.__first(); }
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 mapping_type& __mapping_ref() noexcept { return __members.__second().__first(); }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr mapping_type const& __mapping_ref() const noexcept { return __members.__second().__first(); }
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 accessor_type& __accessor_ref() noexcept { return __members.__second().__second(); }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr accessor_type const& __accessor_ref() const noexcept { return __members.__second().__second(); }

  template <class, class, class, class>
  friend class mdspan;

};

#if defined(_MDSPAN_USE_CLASS_TEMPLATE_ARGUMENT_DEDUCTION)
MDSPAN_TEMPLATE_REQUIRES(
  class ElementType, class... SizeTypes,
  /* requires */ _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_integral, SizeTypes) /* && ... */) &&
  (sizeof...(SizeTypes) > 0)
)
explicit mdspan(ElementType*, SizeTypes...)
  -> mdspan<ElementType, ::std::experimental::dextents<size_t, sizeof...(SizeTypes)>>;

MDSPAN_TEMPLATE_REQUIRES(
  class Pointer,
  (_MDSPAN_TRAIT(is_pointer, std::remove_reference_t<Pointer>))
)
mdspan(Pointer&&) -> mdspan<std::remove_pointer_t<std::remove_reference_t<Pointer>>, extents<size_t>>;

MDSPAN_TEMPLATE_REQUIRES(
  class CArray,
  (_MDSPAN_TRAIT(is_array, CArray) && (rank_v<CArray> == 1))
)
mdspan(CArray&) -> mdspan<std::remove_all_extents_t<CArray>, extents<size_t, ::std::extent_v<CArray,0>>>;

template <class ElementType, class SizeType, size_t N>
mdspan(ElementType*, const ::std::array<SizeType, N>&)
  -> mdspan<ElementType, ::std::experimental::dextents<size_t, N>>;

#ifdef __cpp_lib_span
template <class ElementType, class SizeType, size_t N>
mdspan(ElementType*, ::std::span<SizeType, N>)
  -> mdspan<ElementType, ::std::experimental::dextents<size_t, N>>;
#endif

// This one is necessary because all the constructors take `data_handle_type`s, not
// `ElementType*`s, and `data_handle_type` is taken from `accessor_type::data_handle_type`, which
// seems to throw off automatic deduction guides.
template <class ElementType, class SizeType, size_t... ExtentsPack>
mdspan(ElementType*, const extents<SizeType, ExtentsPack...>&)
  -> mdspan<ElementType, ::std::experimental::extents<SizeType, ExtentsPack...>>;

template <class ElementType, class MappingType>
mdspan(ElementType*, const MappingType&)
  -> mdspan<ElementType, typename MappingType::extents_type, typename MappingType::layout_type>;

template <class MappingType, class AccessorType>
mdspan(const typename AccessorType::data_handle_type, const MappingType&, const AccessorType&)
  -> mdspan<typename AccessorType::element_type, typename MappingType::extents_type, typename MappingType::layout_type, AccessorType>;
#endif

} // end namespace experimental
} // end namespace std
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/mdspan.hpp
//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/layout_left.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

namespace std {
namespace experimental {

//==============================================================================

template <class Extents>
class layout_left::mapping {
  public:
    using extents_type = Extents;
    using index_type = typename extents_type::index_type;
    using size_type = typename extents_type::size_type;
    using rank_type = typename extents_type::rank_type;
    using layout_type = layout_left;
  private:

    static_assert(detail::__is_extents_v<extents_type>, "std::experimental::layout_left::mapping must be instantiated with a specialization of std::experimental::extents.");

    template <class>
    friend class mapping;

    // i0+(i1 + E(1)*(i2 + E(2)*i3))
    template <size_t r, size_t Rank>
    struct __rank_count {};

    template <size_t r, size_t Rank, class I, class... Indices>
    _MDSPAN_HOST_DEVICE
    constexpr index_type __compute_offset(
      __rank_count<r,Rank>, const I& i, Indices... idx) const {
      return __compute_offset(__rank_count<r+1,Rank>(), idx...) *
                 __extents.template __extent<r>() + i;
    }

    template<class I>
    _MDSPAN_HOST_DEVICE
    constexpr index_type __compute_offset(
      __rank_count<extents_type::rank()-1,extents_type::rank()>, const I& i) const {
      return i;
    }

    _MDSPAN_HOST_DEVICE
    constexpr index_type __compute_offset(__rank_count<0,0>) const { return 0; }

  public:

    //--------------------------------------------------------------------------------

    MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mapping() noexcept = default;
    MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mapping(mapping const&) noexcept = default;

    _MDSPAN_HOST_DEVICE
    constexpr mapping(extents_type const& __exts) noexcept
      :__extents(__exts)
    { }

    MDSPAN_TEMPLATE_REQUIRES(
      class OtherExtents,
      /* requires */ (
        _MDSPAN_TRAIT(is_constructible, extents_type, OtherExtents)
      )
    )
    MDSPAN_CONDITIONAL_EXPLICIT((!is_convertible<OtherExtents, extents_type>::value)) // needs two () due to comma
    MDSPAN_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14
    mapping(mapping<OtherExtents> const& other) noexcept // NOLINT(google-explicit-constructor)
      :__extents(other.extents())
    {
       /*
        * TODO: check precondition
        * other.required_span_size() is a representable value of type index_type
        */
    }

    MDSPAN_TEMPLATE_REQUIRES(
      class OtherExtents,
      /* requires */ (
        _MDSPAN_TRAIT(is_constructible, extents_type, OtherExtents) &&
        (extents_type::rank() <= 1)
      )
    )
    MDSPAN_CONDITIONAL_EXPLICIT((!is_convertible<OtherExtents, extents_type>::value)) // needs two () due to comma
    MDSPAN_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14
    mapping(layout_right::mapping<OtherExtents> const& other) noexcept // NOLINT(google-explicit-constructor)
      :__extents(other.extents())
    {
       /*
        * TODO: check precondition
        * other.required_span_size() is a representable value of type index_type
        */
    }

    MDSPAN_TEMPLATE_REQUIRES(
      class OtherExtents,
      /* requires */ (
        _MDSPAN_TRAIT(is_constructible, extents_type, OtherExtents)
      )
    )
    MDSPAN_CONDITIONAL_EXPLICIT((extents_type::rank() > 0))
    MDSPAN_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14
    mapping(layout_stride::mapping<OtherExtents> const& other) // NOLINT(google-explicit-constructor)
      :__extents(other.extents())
    {
       /*
        * TODO: check precondition
        * other.required_span_size() is a representable value of type index_type
        */
       #ifndef __CUDA_ARCH__
       size_t stride = 1;
       for(rank_type r=0; r<__extents.rank(); r++) {
         if(stride != other.stride(r))
           throw std::runtime_error("Assigning layout_stride to layout_left with invalid strides.");
         stride *= __extents.extent(r);
       }
       #endif
    }

    MDSPAN_INLINE_FUNCTION_DEFAULTED _MDSPAN_CONSTEXPR_14_DEFAULTED mapping& operator=(mapping const&) noexcept = default;

    MDSPAN_INLINE_FUNCTION
    constexpr const extents_type& extents() const noexcept {
      return __extents;
    }

    MDSPAN_INLINE_FUNCTION
    constexpr index_type required_span_size() const noexcept {
      index_type value = 1;
      for(rank_type r=0; r<extents_type::rank(); r++) value*=__extents.extent(r);
      return value;
    }

    //--------------------------------------------------------------------------------

    MDSPAN_TEMPLATE_REQUIRES(
      class... Indices,
      /* requires */ (
        (sizeof...(Indices) == extents_type::rank()) &&
        _MDSPAN_FOLD_AND(
           (_MDSPAN_TRAIT(is_convertible, Indices, index_type) &&
            _MDSPAN_TRAIT(is_nothrow_constructible, index_type, Indices))
        )
      )
    )
    _MDSPAN_HOST_DEVICE
    constexpr index_type operator()(Indices... idxs) const noexcept {
      return __compute_offset(__rank_count<0, extents_type::rank()>(), idxs...);
    }

    MDSPAN_INLINE_FUNCTION static constexpr bool is_always_unique() noexcept { return true; }
    MDSPAN_INLINE_FUNCTION static constexpr bool is_always_exhaustive() noexcept { return true; }
    MDSPAN_INLINE_FUNCTION static constexpr bool is_always_strided() noexcept { return true; }

    MDSPAN_INLINE_FUNCTION constexpr bool is_unique() const noexcept { return true; }
    MDSPAN_INLINE_FUNCTION constexpr bool is_exhaustive() const noexcept { return true; }
    MDSPAN_INLINE_FUNCTION constexpr bool is_strided() const noexcept { return true; }

    MDSPAN_INLINE_FUNCTION
    constexpr index_type stride(rank_type i) const noexcept {
      index_type value = 1;
      for(rank_type r=0; r<i; r++) value*=__extents.extent(r);
      return value;
    }

    template<class OtherExtents>
    MDSPAN_INLINE_FUNCTION
    friend constexpr bool operator==(mapping const& lhs, mapping<OtherExtents> const& rhs) noexcept {
      return lhs.extents() == rhs.extents();
    }

    // In C++ 20 the not equal exists if equal is found
#if MDSPAN_HAS_CXX_20
    template<class OtherExtents>
    MDSPAN_INLINE_FUNCTION
    friend constexpr bool operator!=(mapping const& lhs, mapping<OtherExtents> const& rhs) noexcept {
      return lhs.extents() != rhs.extents();
    }
#endif

    // Not really public, but currently needed to implement fully constexpr useable submdspan:
    template<size_t N, class SizeType, size_t ... E, size_t ... Idx>
    constexpr index_type __get_stride(std::experimental::extents<SizeType, E...>,integer_sequence<size_t, Idx...>) const {
      return _MDSPAN_FOLD_TIMES_RIGHT((Idx<N? __extents.template __extent<Idx>():1),1);
    }
    template<size_t N>
    constexpr index_type __stride() const noexcept {
      return __get_stride<N>(__extents, make_index_sequence<extents_type::rank()>());
    }

private:
   _MDSPAN_NO_UNIQUE_ADDRESS extents_type __extents{};

};

} // end namespace experimental
} // end namespace std

//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/layout_left.hpp
//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/submdspan.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

namespace std {
namespace experimental {

namespace detail {

template <size_t OldExtent, size_t OldStaticStride, class T>
struct __slice_wrap {
  T slice;
  size_t old_extent;
  size_t old_stride;
};

//--------------------------------------------------------------------------------

template <size_t OldExtent, size_t OldStaticStride>
MDSPAN_INLINE_FUNCTION constexpr
__slice_wrap<OldExtent, OldStaticStride, size_t>
__wrap_slice(size_t val, size_t ext, size_t stride) { return { val, ext, stride }; }

template <size_t OldExtent, size_t OldStaticStride, class IntegerType, IntegerType Value0>
MDSPAN_INLINE_FUNCTION constexpr
__slice_wrap<OldExtent, OldStaticStride, std::integral_constant<IntegerType, Value0>>
__wrap_slice(size_t val, size_t ext, std::integral_constant<IntegerType, Value0> stride)
{
#if MDSPAN_HAS_CXX_17
  if constexpr (std::is_signed_v<IntegerType>) {
    static_assert(Value0 >= IntegerType(0), "Invalid slice specifier");
  }
#endif // MDSPAN_HAS_CXX_17

  return { val, ext, stride };
}

template <size_t OldExtent, size_t OldStaticStride>
MDSPAN_INLINE_FUNCTION constexpr
__slice_wrap<OldExtent, OldStaticStride, full_extent_t>
__wrap_slice(full_extent_t val, size_t ext, size_t stride) { return { val, ext, stride }; }

// TODO generalize this to anything that works with std::get<0> and std::get<1>
template <size_t OldExtent, size_t OldStaticStride>
MDSPAN_INLINE_FUNCTION constexpr
__slice_wrap<OldExtent, OldStaticStride, std::tuple<size_t, size_t>>
__wrap_slice(std::tuple<size_t, size_t> const& val, size_t ext, size_t stride)
{
  return { val, ext, stride };
}

template <size_t OldExtent, size_t OldStaticStride,
	  class IntegerType0, IntegerType0 Value0,
	  class IntegerType1, IntegerType1 Value1>
MDSPAN_INLINE_FUNCTION constexpr
  __slice_wrap<OldExtent, OldStaticStride,
               std::tuple<std::integral_constant<IntegerType0, Value0>,
                          std::integral_constant<IntegerType1, Value1>>>
__wrap_slice(std::tuple<std::integral_constant<IntegerType0, Value0>, std::integral_constant<IntegerType1, Value1>> const& val, size_t ext, size_t stride)
{
  static_assert(Value1 >= Value0, "Invalid slice tuple");
  return { val, ext, stride };
}

//--------------------------------------------------------------------------------

// a layout right remains a layout right if it is indexed by 0 or more scalars,
// then optionally a pair and finally 0 or more all
template <
  // what we encountered until now preserves the layout right
  bool result=true,
  // we only encountered 0 or more scalars, no pair or all
  bool encountered_only_scalar=true
>
struct preserve_layout_right_analysis : integral_constant<bool, result> {
  using layout_type_if_preserved = layout_right;
  using encounter_pair = preserve_layout_right_analysis<
    // if we encounter a pair, the layout remains a layout right only if it was one before
    // and that only scalars were encountered until now
    result && encountered_only_scalar,
    // if we encounter a pair, we didn't encounter scalars only
    false
  >;
  using encounter_all = preserve_layout_right_analysis<
    // if we encounter a all, the layout remains a layout right if it was one before
    result,
    // if we encounter a all, we didn't encounter scalars only
    false
  >;
  using encounter_scalar = preserve_layout_right_analysis<
    // if we encounter a scalar, the layout remains a layout right only if it was one before
    // and that only scalars were encountered until now
    result && encountered_only_scalar,
    // if we encounter a scalar, the fact that we encountered scalars only doesn't change
    encountered_only_scalar
  >;
};

// a layout left remains a layout left if it is indexed by 0 or more all,
// then optionally a pair and finally 0 or more scalars
template <
  bool result=true,
  bool encountered_only_all=true
>
struct preserve_layout_left_analysis : integral_constant<bool, result> {
  using layout_type_if_preserved = layout_left;
  using encounter_pair = preserve_layout_left_analysis<
    // if we encounter a pair, the layout remains a layout left only if it was one before
    // and that only all were encountered until now
    result && encountered_only_all,
    // if we encounter a pair, we didn't encounter all only
    false
  >;
  using encounter_all = preserve_layout_left_analysis<
    // if we encounter a all, the layout remains a layout left only if it was one before
    // and that only all were encountered until now
    result && encountered_only_all,
    // if we encounter a all, the fact that we encountered scalars all doesn't change
    encountered_only_all
  >;
  using encounter_scalar = preserve_layout_left_analysis<
    // if we encounter a scalar, the layout remains a layout left if it was one before
    result,
    // if we encounter a scalar, we didn't encounter scalars only
    false
  >;
};

struct ignore_layout_preservation : std::integral_constant<bool, false> {
  using layout_type_if_preserved = void;
  using encounter_pair = ignore_layout_preservation;
  using encounter_all = ignore_layout_preservation;
  using encounter_scalar = ignore_layout_preservation;
};

template <class Layout>
struct preserve_layout_analysis
  : ignore_layout_preservation { };
template <>
struct preserve_layout_analysis<layout_right>
  : preserve_layout_right_analysis<> { };
template <>
struct preserve_layout_analysis<layout_left>
  : preserve_layout_left_analysis<> { };

//--------------------------------------------------------------------------------

template <
  class _IndexT,
  class _PreserveLayoutAnalysis,
  class _OffsetsArray=__partially_static_sizes<_IndexT, size_t>,
  class _ExtsArray=__partially_static_sizes<_IndexT, size_t>,
  class _StridesArray=__partially_static_sizes<_IndexT, size_t>,
  class = make_index_sequence<_OffsetsArray::__size>,
  class = make_index_sequence<_ExtsArray::__size>,
  class = make_index_sequence<_StridesArray::__size>
>
struct __assign_op_slice_handler;

/* clang-format: off */
template <
  class _IndexT,
  class _PreserveLayoutAnalysis,
  size_t... _Offsets,
  size_t... _Exts,
  size_t... _Strides,
  size_t... _OffsetIdxs,
  size_t... _ExtIdxs,
  size_t... _StrideIdxs>
struct __assign_op_slice_handler<
  _IndexT,
  _PreserveLayoutAnalysis,
  __partially_static_sizes<_IndexT, size_t, _Offsets...>,
  __partially_static_sizes<_IndexT, size_t, _Exts...>,
  __partially_static_sizes<_IndexT, size_t, _Strides...>,
  integer_sequence<size_t, _OffsetIdxs...>,
  integer_sequence<size_t, _ExtIdxs...>,
  integer_sequence<size_t, _StrideIdxs...>>
{
  // TODO remove this for better compiler performance
  static_assert(
    _MDSPAN_FOLD_AND((_Strides == dynamic_extent || _Strides > 0) /* && ... */),
    " "
  );
  static_assert(
    _MDSPAN_FOLD_AND((_Offsets == dynamic_extent || _Offsets >= 0) /* && ... */),
    " "
  );

  using __offsets_storage_t = __partially_static_sizes<_IndexT, size_t, _Offsets...>;
  using __extents_storage_t = __partially_static_sizes<_IndexT, size_t, _Exts...>;
  using __strides_storage_t = __partially_static_sizes<_IndexT, size_t, _Strides...>;
  __offsets_storage_t __offsets;
  __extents_storage_t __exts;
  __strides_storage_t __strides;

#ifdef __INTEL_COMPILER
#if __INTEL_COMPILER <= 1800
  MDSPAN_INLINE_FUNCTION constexpr __assign_op_slice_handler(__assign_op_slice_handler&& __other) noexcept
    : __offsets(::std::move(__other.__offsets)), __exts(::std::move(__other.__exts)), __strides(::std::move(__other.__strides))
  { }
  MDSPAN_INLINE_FUNCTION constexpr __assign_op_slice_handler(
    __offsets_storage_t&& __o,
    __extents_storage_t&& __e,
    __strides_storage_t&& __s
  ) noexcept
    : __offsets(::std::move(__o)), __exts(::std::move(__e)), __strides(::std::move(__s))
  { }
#endif
#endif

// Don't define this unless we need it; they have a cost to compile
#ifndef _MDSPAN_USE_RETURN_TYPE_DEDUCTION
  using __extents_type = ::std::experimental::extents<_IndexT, _Exts...>;
#endif

  // For size_t slice, skip the extent and stride, but add an offset corresponding to the value
  template <size_t _OldStaticExtent, size_t _OldStaticStride>
  MDSPAN_FORCE_INLINE_FUNCTION // NOLINT (misc-unconventional-assign-operator)
  _MDSPAN_CONSTEXPR_14 auto
  operator=(__slice_wrap<_OldStaticExtent, _OldStaticStride, size_t>&& __slice) noexcept
    -> __assign_op_slice_handler<
         _IndexT,
         typename _PreserveLayoutAnalysis::encounter_scalar,
         __partially_static_sizes<_IndexT, size_t, _Offsets..., dynamic_extent>,
         __partially_static_sizes<_IndexT, size_t, _Exts...>,
         __partially_static_sizes<_IndexT, size_t, _Strides...>/* intentional space here to work around ICC bug*/> {
    return {
      __partially_static_sizes<_IndexT, size_t, _Offsets..., dynamic_extent>(
        __construct_psa_from_all_exts_values_tag,
        __offsets.template __get_n<_OffsetIdxs>()..., __slice.slice),
      ::std::move(__exts),
      ::std::move(__strides)
    };
  }

  // Treat integral_constant slice like size_t slice, but with a compile-time offset.
  // The result's extents_type can't take advantage of that,
  // but it might help for specialized layouts.
  template <size_t _OldStaticExtent, size_t _OldStaticStride, class IntegerType, IntegerType Value0>
  MDSPAN_FORCE_INLINE_FUNCTION // NOLINT (misc-unconventional-assign-operator)
  _MDSPAN_CONSTEXPR_14 auto
  operator=(__slice_wrap<_OldStaticExtent, _OldStaticStride, std::integral_constant<IntegerType, Value0>>&&) noexcept
    -> __assign_op_slice_handler<
         _IndexT,
         typename _PreserveLayoutAnalysis::encounter_scalar,
         __partially_static_sizes<_IndexT, size_t, _Offsets..., Value0>,
         __partially_static_sizes<_IndexT, size_t, _Exts...>,
         __partially_static_sizes<_IndexT, size_t, _Strides...>/* intentional space here to work around ICC bug*/> {
#if MDSPAN_HAS_CXX_17
    if constexpr (std::is_signed_v<IntegerType>) {
      static_assert(Value0 >= IntegerType(0), "Invalid slice specifier");
    }
#endif // MDSPAN_HAS_CXX_17
    return {
      __partially_static_sizes<_IndexT, size_t, _Offsets..., Value0>(
        __construct_psa_from_all_exts_values_tag,
        __offsets.template __get_n<_OffsetIdxs>()..., size_t(Value0)),
      ::std::move(__exts),
      ::std::move(__strides)
    };
  }

  // For a std::full_extent, offset 0 and old extent
  template <size_t _OldStaticExtent, size_t _OldStaticStride>
  MDSPAN_FORCE_INLINE_FUNCTION // NOLINT (misc-unconventional-assign-operator)
  _MDSPAN_CONSTEXPR_14 auto
  operator=(__slice_wrap<_OldStaticExtent, _OldStaticStride, full_extent_t>&& __slice) noexcept
    -> __assign_op_slice_handler<
         _IndexT,
         typename _PreserveLayoutAnalysis::encounter_all,
         __partially_static_sizes<_IndexT, size_t, _Offsets..., 0>,
         __partially_static_sizes<_IndexT, size_t, _Exts..., _OldStaticExtent>,
         __partially_static_sizes<_IndexT, size_t, _Strides..., _OldStaticStride>/* intentional space here to work around ICC bug*/> {
    return {
      __partially_static_sizes<_IndexT, size_t, _Offsets..., 0>(
        __construct_psa_from_all_exts_values_tag,
        __offsets.template __get_n<_OffsetIdxs>()..., size_t(0)),
      __partially_static_sizes<_IndexT, size_t, _Exts..., _OldStaticExtent>(
        __construct_psa_from_all_exts_values_tag,
        __exts.template __get_n<_ExtIdxs>()..., __slice.old_extent),
      __partially_static_sizes<_IndexT, size_t, _Strides..., _OldStaticStride>(
        __construct_psa_from_all_exts_values_tag,
        __strides.template __get_n<_StrideIdxs>()..., __slice.old_stride)
    };
  }

  // For a std::tuple, add an offset and add a new dynamic extent (strides still preserved)
  template <size_t _OldStaticExtent, size_t _OldStaticStride>
  MDSPAN_FORCE_INLINE_FUNCTION // NOLINT (misc-unconventional-assign-operator)
  _MDSPAN_CONSTEXPR_14 auto
  operator=(__slice_wrap<_OldStaticExtent, _OldStaticStride, tuple<size_t, size_t>>&& __slice) noexcept
    -> __assign_op_slice_handler<
         _IndexT,
         typename _PreserveLayoutAnalysis::encounter_pair,
         __partially_static_sizes<_IndexT, size_t, _Offsets..., dynamic_extent>,
         __partially_static_sizes<_IndexT, size_t, _Exts..., dynamic_extent>,
         __partially_static_sizes<_IndexT, size_t, _Strides..., _OldStaticStride>/* intentional space here to work around ICC bug*/> {
    return {
      __partially_static_sizes<_IndexT, size_t, _Offsets..., dynamic_extent>(
        __construct_psa_from_all_exts_values_tag,
        __offsets.template __get_n<_OffsetIdxs>()..., ::std::get<0>(__slice.slice)),
      __partially_static_sizes<_IndexT, size_t, _Exts..., dynamic_extent>(
        __construct_psa_from_all_exts_values_tag,
        __exts.template __get_n<_ExtIdxs>()..., ::std::get<1>(__slice.slice) - ::std::get<0>(__slice.slice)),
      __partially_static_sizes<_IndexT, size_t, _Strides..., _OldStaticStride>(
        __construct_psa_from_all_exts_values_tag,
        __strides.template __get_n<_StrideIdxs>()..., __slice.old_stride)
    };
  }

  // For a std::tuple of two std::integral_constant, do something like
  // we did above for a tuple of two size_t, but make sure the
  // result's extents type make the values compile-time constants.
  template <size_t _OldStaticExtent, size_t _OldStaticStride,
	    class IntegerType0, IntegerType0 Value0,
	    class IntegerType1, IntegerType1 Value1>
  MDSPAN_FORCE_INLINE_FUNCTION // NOLINT (misc-unconventional-assign-operator)
  _MDSPAN_CONSTEXPR_14 auto
  operator=(__slice_wrap<_OldStaticExtent, _OldStaticStride, tuple<std::integral_constant<IntegerType0, Value0>, std::integral_constant<IntegerType1, Value1>>>&& __slice) noexcept
    -> __assign_op_slice_handler<
         _IndexT,
         typename _PreserveLayoutAnalysis::encounter_pair,
         __partially_static_sizes<_IndexT, size_t, _Offsets..., size_t(Value0)>,
         __partially_static_sizes<_IndexT, size_t, _Exts..., size_t(Value1 - Value0)>,
         __partially_static_sizes<_IndexT, size_t, _Strides..., _OldStaticStride>/* intentional space here to work around ICC bug*/> {
    static_assert(Value1 >= Value0, "Invalid slice specifier");
    return {
      // We're still turning the template parameters Value0 and Value1
      // into (constexpr) run-time values here.
      __partially_static_sizes<_IndexT, size_t, _Offsets..., size_t(Value0) > (
        __construct_psa_from_all_exts_values_tag,
        __offsets.template __get_n<_OffsetIdxs>()..., Value0),
      __partially_static_sizes<_IndexT, size_t, _Exts..., size_t(Value1 - Value0) > (
        __construct_psa_from_all_exts_values_tag,
        __exts.template __get_n<_ExtIdxs>()..., Value1 - Value0),
      __partially_static_sizes<_IndexT, size_t, _Strides..., _OldStaticStride>(
        __construct_psa_from_all_exts_values_tag,
        __strides.template __get_n<_StrideIdxs>()..., __slice.old_stride)
    };
  }

   // TODO defer instantiation of this?
  using layout_type = typename conditional<
    _PreserveLayoutAnalysis::value,
    typename _PreserveLayoutAnalysis::layout_type_if_preserved,
    layout_stride
  >::type;

  // TODO noexcept specification
  template <class NewLayout>
  MDSPAN_INLINE_FUNCTION
  _MDSPAN_DEDUCE_RETURN_TYPE_SINGLE_LINE(
    (
      _MDSPAN_CONSTEXPR_14 /* auto */
      _make_layout_mapping_impl(NewLayout) noexcept
    ),
    (
      /* not layout stride, so don't pass dynamic_strides */
      /* return */ typename NewLayout::template mapping<::std::experimental::extents<_IndexT, _Exts...>>(
        experimental::extents<_IndexT, _Exts...>::__make_extents_impl(::std::move(__exts))
      ) /* ; */
    )
  )

  MDSPAN_INLINE_FUNCTION
  _MDSPAN_DEDUCE_RETURN_TYPE_SINGLE_LINE(
    (
      _MDSPAN_CONSTEXPR_14 /* auto */
      _make_layout_mapping_impl(layout_stride) noexcept
    ),
    (
      /* return */ layout_stride::template mapping<::std::experimental::extents<_IndexT, _Exts...>>
        ::__make_mapping(::std::move(__exts), ::std::move(__strides)) /* ; */
    )
  )

  template <class OldLayoutMapping> // mostly for deferred instantiation, but maybe we'll use this in the future
  MDSPAN_INLINE_FUNCTION
  _MDSPAN_DEDUCE_RETURN_TYPE_SINGLE_LINE(
    (
      _MDSPAN_CONSTEXPR_14 /* auto */
      make_layout_mapping(OldLayoutMapping const&) noexcept
    ),
    (
      /* return */ this->_make_layout_mapping_impl(layout_type{}) /* ; */
    )
  )
};

//==============================================================================

#if _MDSPAN_USE_RETURN_TYPE_DEDUCTION
// Forking this because the C++11 version will be *completely* unreadable
template <class ET, class ST, size_t... Exts, class LP, class AP, class... SliceSpecs, size_t... Idxs>
MDSPAN_INLINE_FUNCTION
constexpr auto _submdspan_impl(
  integer_sequence<size_t, Idxs...>,
  mdspan<ET, std::experimental::extents<ST, Exts...>, LP, AP> const& src,
  SliceSpecs&&... slices
) noexcept
{
  using _IndexT = ST;
  auto _handled =
    _MDSPAN_FOLD_ASSIGN_LEFT(
      (
        detail::__assign_op_slice_handler<
          _IndexT,
          detail::preserve_layout_analysis<LP>
        >{
          __partially_static_sizes<_IndexT, size_t>{},
          __partially_static_sizes<_IndexT, size_t>{},
          __partially_static_sizes<_IndexT, size_t>{}
        }
      ),
        /* = ... = */
      detail::__wrap_slice<
        Exts, dynamic_extent
      >(
        slices, src.extents().template __extent<Idxs>(),
        src.mapping().stride(Idxs)
      )
    );

  size_t offset_size = src.mapping()(_handled.__offsets.template __get_n<Idxs>()...);
  auto offset_ptr = src.accessor().offset(src.data_handle(), offset_size);
  auto map = _handled.make_layout_mapping(src.mapping());
  auto acc_pol = typename AP::offset_policy(src.accessor());
  return mdspan<
    ET, remove_const_t<remove_reference_t<decltype(map.extents())>>,
        typename decltype(_handled)::layout_type, remove_const_t<remove_reference_t<decltype(acc_pol)>>
  >(
    std::move(offset_ptr), std::move(map), std::move(acc_pol)
  );
}
#else

template <class ET, class AP, class Src, class Handled, size_t... Idxs>
auto _submdspan_impl_helper(Src&& src, Handled&& h, std::integer_sequence<size_t, Idxs...>)
  -> mdspan<
       ET, typename Handled::__extents_type, typename Handled::layout_type, typename AP::offset_policy
     >
{
  return {
    src.accessor().offset(src.data_handle(), src.mapping()(h.__offsets.template __get_n<Idxs>()...)),
    h.make_layout_mapping(src.mapping()),
    typename AP::offset_policy(src.accessor())
  };
}

template <class ET, class ST, size_t... Exts, class LP, class AP, class... SliceSpecs, size_t... Idxs>
MDSPAN_INLINE_FUNCTION
_MDSPAN_DEDUCE_RETURN_TYPE_SINGLE_LINE(
  (
    constexpr /* auto */ _submdspan_impl(
      std::integer_sequence<size_t, Idxs...> seq,
      mdspan<ET, std::experimental::extents<ST, Exts...>, LP, AP> const& src,
      SliceSpecs&&... slices
    ) noexcept
  ),
  (
    /* return */ _submdspan_impl_helper<ET, AP>(
      src,
      _MDSPAN_FOLD_ASSIGN_LEFT(
        (
          detail::__assign_op_slice_handler<
            size_t,
            detail::preserve_layout_analysis<LP>
          >{
            __partially_static_sizes<ST, size_t>{},
            __partially_static_sizes<ST, size_t>{},
            __partially_static_sizes<ST, size_t>{}
          }
        ),
        /* = ... = */
        detail::__wrap_slice<
          Exts, dynamic_extent
        >(
          slices, src.extents().template __extent<Idxs>(), src.mapping().stride(Idxs)
        )
      ),
      seq
    ) /* ; */
  )
)

#endif

template <class T> struct _is_layout_stride : std::false_type { };
template<>
struct _is_layout_stride<
  layout_stride
> : std::true_type
{ };

} // namespace detail

//==============================================================================

MDSPAN_TEMPLATE_REQUIRES(
  class ET, class EXT, class LP, class AP, class... SliceSpecs,
  /* requires */ (
    (
      _MDSPAN_TRAIT(is_same, LP, layout_left)
        || _MDSPAN_TRAIT(is_same, LP, layout_right)
        || detail::_is_layout_stride<LP>::value
    ) &&
    _MDSPAN_FOLD_AND((
      _MDSPAN_TRAIT(is_convertible, SliceSpecs, size_t)
        || _MDSPAN_TRAIT(is_convertible, SliceSpecs, tuple<size_t, size_t>)
        || _MDSPAN_TRAIT(is_convertible, SliceSpecs, full_extent_t)
    ) /* && ... */) &&
    sizeof...(SliceSpecs) == EXT::rank()
  )
)
MDSPAN_INLINE_FUNCTION
_MDSPAN_DEDUCE_RETURN_TYPE_SINGLE_LINE(
  (
    constexpr submdspan(
      mdspan<ET, EXT, LP, AP> const& src, SliceSpecs... slices
    ) noexcept
  ),
  (
    /* return */
      detail::_submdspan_impl(std::make_index_sequence<sizeof...(SliceSpecs)>{}, src, slices...) /*;*/
  )
)
/* clang-format: on */

} // end namespace experimental
} // namespace std
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p0009_bits/submdspan.hpp
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/mdspan
//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p1684_bits/mdarray.hpp
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

//BEGIN_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p1684_bits/../mdspan
/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p1684_bits/../mdspan

namespace std {
namespace experimental {

namespace {
  template<class Extents>
  struct size_of_extents;

  template<class IndexType, size_t ... Extents>
  struct size_of_extents<extents<IndexType, Extents...>> {
    constexpr static size_t value() {
      size_t size = 1;
      for(size_t r=0; r<extents<IndexType, Extents...>::rank(); r++)
        size *= extents<IndexType, Extents...>::static_extent(r);
      return size;
    }
  };
}

namespace {
  template<class C>
  struct container_is_array : false_type {
    template<class M>
    static constexpr C construct(const M& m) { return C(m.required_span_size()); }
  };
  template<class T, size_t N>
  struct container_is_array<array<T,N>> : true_type {
    template<class M>
    static constexpr array<T,N> construct(const M&) { return array<T,N>(); }
  };
}

template <
  class ElementType,
  class Extents,
  class LayoutPolicy = layout_right,
  class Container = vector<ElementType>
>
class mdarray {
private:
  static_assert(detail::__is_extents_v<Extents>, "std::experimental::mdspan's Extents template parameter must be a specialization of std::experimental::extents.");

public:

  //--------------------------------------------------------------------------------
  // Domain and codomain types

  using extents_type = Extents;
  using layout_type = LayoutPolicy;
  using container_type = Container;
  using mapping_type = typename layout_type::template mapping<extents_type>;
  using element_type = ElementType;
  using value_type = remove_cv_t<element_type>;
  using index_type = typename Extents::index_type;
  using size_type = typename Extents::size_type;
  using rank_type = typename Extents::rank_type;
  using pointer = typename container_type::pointer;
  using reference = typename container_type::reference;
  using const_pointer = typename container_type::const_pointer;
  using const_reference = typename container_type::const_reference;

public:

  //--------------------------------------------------------------------------------
  // [mdspan.basic.cons], mdspan constructors, assignment, and destructor

#if !(MDSPAN_HAS_CXX_20)
  MDSPAN_FUNCTION_REQUIRES(
    (MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr),
    mdarray, (), ,
    /* requires */ (extents_type::rank_dynamic()!=0)) {}
#else
  MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mdarray() requires(extents_type::rank_dynamic()!=0) = default;
#endif
  MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mdarray(const mdarray&) = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mdarray(mdarray&&) = default;

  // Constructors for container types constructible from a size
  MDSPAN_TEMPLATE_REQUIRES(
    class... SizeTypes,
    /* requires */ (
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_convertible, SizeTypes, index_type) /* && ... */) &&
      _MDSPAN_TRAIT(is_constructible, extents_type, SizeTypes...) &&
      _MDSPAN_TRAIT(is_constructible, mapping_type, extents_type) &&
      (_MDSPAN_TRAIT(is_constructible, container_type, size_t) ||
       container_is_array<container_type>::value) &&
      (extents_type::rank()>0 || extents_type::rank_dynamic()==0)
    )
  )
  MDSPAN_INLINE_FUNCTION
  explicit constexpr mdarray(SizeTypes... dynamic_extents)
    : map_(extents_type(dynamic_extents...)), ctr_(container_is_array<container_type>::construct(map_))
  { }

  MDSPAN_FUNCTION_REQUIRES(
    (MDSPAN_INLINE_FUNCTION constexpr),
    mdarray, (const extents_type& exts), ,
    /* requires */ ((_MDSPAN_TRAIT(is_constructible, container_type, size_t) ||
                     container_is_array<container_type>::value) &&
                    _MDSPAN_TRAIT(is_constructible, mapping_type, extents_type))
  ) : map_(exts), ctr_(container_is_array<container_type>::construct(map_))
  { }

  MDSPAN_FUNCTION_REQUIRES(
    (MDSPAN_INLINE_FUNCTION constexpr),
    mdarray, (const mapping_type& m), ,
    /* requires */ (_MDSPAN_TRAIT(is_constructible, container_type, size_t) ||
                    container_is_array<container_type>::value)
  ) : map_(m), ctr_(container_is_array<container_type>::construct(map_))
  { }

  // Constructors from container
  MDSPAN_TEMPLATE_REQUIRES(
    class... SizeTypes,
    /* requires */ (
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_convertible, SizeTypes, index_type) /* && ... */) &&
      _MDSPAN_TRAIT(is_constructible, extents_type, SizeTypes...) &&
      _MDSPAN_TRAIT(is_constructible, mapping_type, extents_type)
    )
  )
  MDSPAN_INLINE_FUNCTION
  explicit constexpr mdarray(const container_type& ctr, SizeTypes... dynamic_extents)
    : map_(extents_type(dynamic_extents...)), ctr_(ctr)
  { assert(ctr.size() >= static_cast<size_t>(map_.required_span_size())); }

  MDSPAN_FUNCTION_REQUIRES(
    (MDSPAN_INLINE_FUNCTION constexpr),
    mdarray, (const container_type& ctr, const extents_type& exts), ,
    /* requires */ (_MDSPAN_TRAIT(is_constructible, mapping_type, extents_type))
  ) : map_(exts), ctr_(ctr)
  { assert(ctr.size() >= static_cast<size_t>(map_.required_span_size())); }

  constexpr mdarray(const container_type& ctr, const mapping_type& m)
    : map_(m), ctr_(ctr)
  { assert(ctr.size() >= static_cast<size_t>(map_.required_span_size())); }

  // Constructors from container
  MDSPAN_TEMPLATE_REQUIRES(
    class... SizeTypes,
    /* requires */ (
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_convertible, SizeTypes, index_type) /* && ... */) &&
      _MDSPAN_TRAIT(is_constructible, extents_type, SizeTypes...) &&
      _MDSPAN_TRAIT(is_constructible, mapping_type, extents_type)
    )
  )
  MDSPAN_INLINE_FUNCTION
  explicit constexpr mdarray(container_type&& ctr, SizeTypes... dynamic_extents)
    : map_(extents_type(dynamic_extents...)), ctr_(std::move(ctr))
  { assert(ctr_.size() >= static_cast<size_t>(map_.required_span_size())); }

  MDSPAN_FUNCTION_REQUIRES(
    (MDSPAN_INLINE_FUNCTION constexpr),
    mdarray, (container_type&& ctr, const extents_type& exts), ,
    /* requires */ (_MDSPAN_TRAIT(is_constructible, mapping_type, extents_type))
  ) : map_(exts), ctr_(std::move(ctr))
  { assert(ctr_.size() >= static_cast<size_t>(map_.required_span_size())); }

  constexpr mdarray(container_type&& ctr, const mapping_type& m)
    : map_(m), ctr_(std::move(ctr))
  { assert(ctr_.size() >= static_cast<size_t>(map_.required_span_size())); }

  MDSPAN_TEMPLATE_REQUIRES(
    class OtherElementType, class OtherExtents, class OtherLayoutPolicy, class OtherContainer,
    /* requires */ (
      _MDSPAN_TRAIT(is_constructible, mapping_type, typename OtherLayoutPolicy::template mapping<OtherExtents>) &&
      _MDSPAN_TRAIT(is_constructible, container_type, OtherContainer)
    )
  )
  MDSPAN_INLINE_FUNCTION
  constexpr mdarray(const mdarray<OtherElementType, OtherExtents, OtherLayoutPolicy, OtherContainer>& other)
    : map_(other.mapping()), ctr_(other.container())
  {
    static_assert(is_constructible<extents_type, OtherExtents>::value, "");
  }

  // Constructors for container types constructible from a size and allocator
  MDSPAN_TEMPLATE_REQUIRES(
    class Alloc,
    /* requires */ (_MDSPAN_TRAIT(is_constructible, container_type, size_t, Alloc) &&
                    _MDSPAN_TRAIT(is_constructible, mapping_type, extents_type))
  )
  MDSPAN_INLINE_FUNCTION
  constexpr mdarray(const extents_type& exts, const Alloc& a)
    : map_(exts), ctr_(map_.required_span_size(), a)
  { }

  MDSPAN_TEMPLATE_REQUIRES(
    class Alloc,
    /* requires */ (_MDSPAN_TRAIT(is_constructible, container_type, size_t, Alloc))
  )
  MDSPAN_INLINE_FUNCTION
  constexpr mdarray(const mapping_type& map, const Alloc& a)
    : map_(map), ctr_(map_.required_span_size(), a)
  { }

  // Constructors for container types constructible from a container and allocator
  MDSPAN_TEMPLATE_REQUIRES(
    class Alloc,
    /* requires */ (_MDSPAN_TRAIT(is_constructible, container_type, container_type, Alloc) &&
                    _MDSPAN_TRAIT(is_constructible, mapping_type, extents_type))
  )
  MDSPAN_INLINE_FUNCTION
  constexpr mdarray(const container_type& ctr, const extents_type& exts, const Alloc& a)
    : map_(exts), ctr_(ctr, a)
  { assert(ctr_.size() >= static_cast<size_t>(map_.required_span_size())); }

  MDSPAN_TEMPLATE_REQUIRES(
    class Alloc,
    /* requires */ (_MDSPAN_TRAIT(is_constructible, container_type, size_t, Alloc))
  )
  MDSPAN_INLINE_FUNCTION
  constexpr mdarray(const container_type& ctr, const mapping_type& map, const Alloc& a)
    : map_(map), ctr_(ctr, a)
  { assert(ctr_.size() >= static_cast<size_t>(map_.required_span_size())); }

  MDSPAN_TEMPLATE_REQUIRES(
    class Alloc,
    /* requires */ (_MDSPAN_TRAIT(is_constructible, container_type, container_type, Alloc) &&
                    _MDSPAN_TRAIT(is_constructible, mapping_type, extents_type))
  )
  MDSPAN_INLINE_FUNCTION
  constexpr mdarray(container_type&& ctr, const extents_type& exts, const Alloc& a)
    : map_(exts), ctr_(std::move(ctr), a)
  { assert(ctr_.size() >= static_cast<size_t>(map_.required_span_size())); }

  MDSPAN_TEMPLATE_REQUIRES(
    class Alloc,
    /* requires */ (_MDSPAN_TRAIT(is_constructible, container_type, size_t, Alloc))
  )
  MDSPAN_INLINE_FUNCTION
  constexpr mdarray(container_type&& ctr, const mapping_type& map, const Alloc& a)
    : map_(map), ctr_(std::move(ctr), a)
  { assert(ctr_.size() >= map_.required_span_size()); }

  MDSPAN_TEMPLATE_REQUIRES(
    class OtherElementType, class OtherExtents, class OtherLayoutPolicy, class OtherContainer, class Alloc,
    /* requires */ (
      _MDSPAN_TRAIT(is_constructible, mapping_type, typename OtherLayoutPolicy::template mapping<OtherExtents>) &&
      _MDSPAN_TRAIT(is_constructible, container_type, OtherContainer, Alloc)
    )
  )
  MDSPAN_INLINE_FUNCTION
  constexpr mdarray(const mdarray<OtherElementType, OtherExtents, OtherLayoutPolicy, OtherContainer>& other, const Alloc& a)
    : map_(other.mapping()), ctr_(other.container(), a)
  {
    static_assert(is_constructible<extents_type, OtherExtents>::value, "");
  }

  MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mdarray& operator= (const mdarray&) = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mdarray& operator= (mdarray&&) = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~mdarray() = default;

  //--------------------------------------------------------------------------------
  // [mdspan.basic.mapping], mdspan mapping domain multidimensional index to access codomain element

  #if MDSPAN_USE_BRACKET_OPERATOR
  MDSPAN_TEMPLATE_REQUIRES(
    class... SizeTypes,
    /* requires */ (
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_convertible, SizeTypes, index_type) /* && ... */) &&
      extents_type::rank() == sizeof...(SizeTypes)
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr const_reference operator[](SizeTypes... indices) const noexcept
  {
    return ctr_[map_(static_cast<index_type>(std::move(indices))...)];
  }

  MDSPAN_TEMPLATE_REQUIRES(
    class... SizeTypes,
    /* requires */ (
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_convertible, SizeTypes, index_type) /* && ... */) &&
      extents_type::rank() == sizeof...(SizeTypes)
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr reference operator[](SizeTypes... indices) noexcept
  {
    return ctr_[map_(static_cast<index_type>(std::move(indices))...)];
  }
  #endif

#if 0
  MDSPAN_TEMPLATE_REQUIRES(
    class SizeType, size_t N,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, SizeType, index_type) &&
      N == extents_type::rank()
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr const_reference operator[](const array<SizeType, N>& indices) const noexcept
  {
    return __impl::template __callop<reference>(*this, indices);
  }

  MDSPAN_TEMPLATE_REQUIRES(
    class SizeType, size_t N,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, SizeType, index_type) &&
      N == extents_type::rank()
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr reference operator[](const array<SizeType, N>& indices) noexcept
  {
    return __impl::template __callop<reference>(*this, indices);
  }
#endif

  #if MDSPAN_USE_PAREN_OPERATOR
  MDSPAN_TEMPLATE_REQUIRES(
    class... SizeTypes,
    /* requires */ (
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_convertible, SizeTypes, index_type) /* && ... */) &&
      extents_type::rank() == sizeof...(SizeTypes)
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr const_reference operator()(SizeTypes... indices) const noexcept
  {
    return ctr_[map_(static_cast<index_type>(std::move(indices))...)];
  }
  MDSPAN_TEMPLATE_REQUIRES(
    class... SizeTypes,
    /* requires */ (
      _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(is_convertible, SizeTypes, index_type) /* && ... */) &&
      extents_type::rank() == sizeof...(SizeTypes)
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr reference operator()(SizeTypes... indices) noexcept
  {
    return ctr_[map_(static_cast<index_type>(std::move(indices))...)];
  }

#if 0
  MDSPAN_TEMPLATE_REQUIRES(
    class SizeType, size_t N,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, SizeType, index_type) &&
      N == extents_type::rank()
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr const_reference operator()(const array<SizeType, N>& indices) const noexcept
  {
    return __impl::template __callop<reference>(*this, indices);
  }

  MDSPAN_TEMPLATE_REQUIRES(
    class SizeType, size_t N,
    /* requires */ (
      _MDSPAN_TRAIT(is_convertible, SizeType, index_type) &&
      N == extents_type::rank()
    )
  )
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr reference operator()(const array<SizeType, N>& indices) noexcept
  {
    return __impl::template __callop<reference>(*this, indices);
  }
#endif
  #endif

  MDSPAN_INLINE_FUNCTION constexpr pointer data() noexcept { return ctr_.data(); };
  MDSPAN_INLINE_FUNCTION constexpr const_pointer data() const noexcept { return ctr_.data(); };
  MDSPAN_INLINE_FUNCTION constexpr container_type& container() noexcept { return ctr_; };
  MDSPAN_INLINE_FUNCTION constexpr const container_type& container() const noexcept { return ctr_; };

  //--------------------------------------------------------------------------------
  // [mdspan.basic.domobs], mdspan observers of the domain multidimensional index space

  MDSPAN_INLINE_FUNCTION static constexpr rank_type rank() noexcept { return extents_type::rank(); }
  MDSPAN_INLINE_FUNCTION static constexpr rank_type rank_dynamic() noexcept { return extents_type::rank_dynamic(); }
  MDSPAN_INLINE_FUNCTION static constexpr size_t static_extent(size_t r) noexcept { return extents_type::static_extent(r); }

  MDSPAN_INLINE_FUNCTION constexpr extents_type extents() const noexcept { return map_.extents(); };
  MDSPAN_INLINE_FUNCTION constexpr index_type extent(size_t r) const noexcept { return map_.extents().extent(r); };
  MDSPAN_INLINE_FUNCTION constexpr index_type size() const noexcept {
//    return __impl::__size(*this);
    return ctr_.size();
  };

  //--------------------------------------------------------------------------------
  // [mdspan.basic.obs], mdspan observers of the mapping

  MDSPAN_INLINE_FUNCTION static constexpr bool is_always_unique() noexcept { return mapping_type::is_always_unique(); };
  MDSPAN_INLINE_FUNCTION static constexpr bool is_always_exhaustive() noexcept { return mapping_type::is_always_exhaustive(); };
  MDSPAN_INLINE_FUNCTION static constexpr bool is_always_strided() noexcept { return mapping_type::is_always_strided(); };

  MDSPAN_INLINE_FUNCTION constexpr mapping_type mapping() const noexcept { return map_; };
  MDSPAN_INLINE_FUNCTION constexpr bool is_unique() const noexcept { return map_.is_unique(); };
  MDSPAN_INLINE_FUNCTION constexpr bool is_exhaustive() const noexcept { return map_.is_exhaustive(); };
  MDSPAN_INLINE_FUNCTION constexpr bool is_strided() const noexcept { return map_.is_strided(); };
  MDSPAN_INLINE_FUNCTION constexpr index_type stride(size_t r) const { return map_.stride(r); };

private:
  mapping_type map_;
  container_type ctr_;

  template <class, class, class, class>
  friend class mdarray;
};

} // end namespace experimental
} // end namespace std
//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/__p1684_bits/mdarray.hpp

//END_FILE_INCLUDE: /home/runner/work/mdspan/mdspan/include/experimental/mdarray
#endif // _MDSPAN_SINGLE_HEADER_INCLUDE_GUARD_

#include <array>

namespace jada {

namespace stdex = std::experimental;

template <size_t N> using extents = stdex::dextents<size_type, N>;

/// @brief Converts array-like dimensions to extents
/// @param dims array-like object with spatial dimensions
/// @return extents object with the same dimensions
template <class T> static constexpr auto make_extent(T dims) { return extents<rank(dims)>{dims}; }

namespace detail {

// nvcc doesnt like template lamdas...
struct MakeArray {

    template <class T, size_t... Is>
    static constexpr auto make(T extents, std::index_sequence<Is...>) {
        return std::array{extents.extent(Is)...};
    }
};

} // namespace detail

/// @brief Converts extents to std::array
/// @param ext extensions to convert
/// @return std::array<size_t, Rank> array with the extents
template <class T> static constexpr auto extent_to_array(T ext) {
    return detail::MakeArray::make(ext, std::make_index_sequence<rank(ext)>{});
}

/// @brief Computes the total element count spanned by the extents
/// @param ext extensions to compute the flat size of
/// @return size_t the flat size
template <class T> static constexpr size_type flat_size(T ext) {

    size_type ret(1);
    for (size_t i = 0; i < rank(ext); ++i) { ret *= ext.extent(i); }
    return ret;
}

namespace detail {
// nvcc doesnt like template lamdas...
struct Compare {

    template <class T, class Y, size_t... Is>
    static constexpr auto op(T indices, Y extents, std::index_sequence<Is...>) {
        return std::array<bool, sizeof...(Is)>{std::get<Is>(indices) < extents.extent(Is)...};
    }
};

} // namespace detail

/// @brief Ensures that the input indices are all smaller than the input dims
/// @param indices set of indices (array/tuple) corresponding to a spatial location
/// @param dims input extensions
/// @return true if all indices in bounds, false otherwise
template <class T, class Y> static constexpr bool indices_in_bounds(T indices, Y dims) {

    auto extents = make_extent(dims);
    auto arr     = detail::Compare::op<T, decltype(extents)>(
        indices, extents, std::make_index_sequence<rank(extents)>{});
    return std::all_of(std::begin(arr), std::end(arr), [](bool b) { return b == true; });
}

} // namespace jada

#include <iostream>

namespace jada {

namespace stdex = std::experimental;

template <class ElementType, size_t N, class Layout>
using span_base = stdex::mdspan<ElementType, extents<N>, Layout>;

template <class ElementType, size_t N>
using span = span_base<ElementType, N, stdex::layout_right>;

// TODO: rename this function as extents and rename the extents object something
// else
/// @brief Returns the extent of the input span
/// @tparam Span a mdspan type
/// @param span the input span to query the extent of
/// @return extent of the input span
template <class Span> static constexpr auto extent(const Span& span) {
    return span.extents();
}

/// @brief Returns the dimensions of the input span as an std::array
/// @tparam Span a mdspan type
/// @param span the input span to query the dimensions of
/// @return std::array of dimensions spanned by the input span
template <class Span> static constexpr auto dimensions(const Span& span) {
    return extent_to_array(extent(span));
}

/// @brief Makes a multi-dimensional span of the input container
/// @tparam Container a container which has a value_type, size() and data()
/// members
/// @param c the input container
/// @param dims dimensions of the multi-dimensional span
/// @return a multi-dimensional span
template <class Container, class Dims>
static constexpr auto make_span(Container& c, Dims dims) {
    using value_type = typename Container::value_type;
    auto ext         = make_extent(dims);
    runtime_assert(flat_size(ext) == std::size(c),
                   "Dimension mismatch in make_span");
    return span<value_type, rank(ext)>(std::data(c), ext);
}

/// @brief Makes a multi-dimensional span of the input container
/// @tparam Container a container which has a value_type, size() and data()
/// members
/// @param c the input container
/// @param dims dimensions of the multi-dimensional span
/// @return a multi-dimensional span
template <class Container, class Dims>
static constexpr auto make_span(const Container& c, Dims dims) {
    using value_type = const typename Container::value_type;
    auto ext         = make_extent(dims);
    runtime_assert(flat_size(ext) == std::size(c),
                   "Dimension mismatch in make_span");
    return span<value_type, rank(ext)>(std::data(c), ext);
}

} // namespace jada

#include <array>

namespace jada {

namespace detail {

// nvcc doesnt like template lamdas...
struct TupleMaker {

    template <class Span, class B, class E, size_t... Is>
    static constexpr auto
    make(Span span, B begin, E end, std::index_sequence<Is...>) {
        return std::make_tuple(
            span, std::make_pair(std::get<Is>(begin), std::get<Is>(end))...);
    }
};

} // namespace detail

/// @brief Creates a subspan from the input span based on the index sets begin
/// and end
/// @param span the input span to create the subspan from
/// @param begin set of indices describing the beginning of the subpan
/// @param end set of indices describing the end of the subspan
/// @return a subspan form [begin, end)
template <class Span, class B, class E>
static constexpr auto make_subspan(Span span, B begin, E end) {

    static_assert(rank(span) == rank(begin),
                  "Dimension mismatch in make_subspan");
    static_assert(rank(begin) == rank(end),
                  "Dimension mismatch in make_subspan");

    auto tpl = detail::TupleMaker::make(
        span, begin, end, std::make_index_sequence<rank(span)>{});
    auto callable = [](auto... params) { return stdex::submdspan(params...); };

    return std::apply(callable, tpl);
}

/// @brief Creates a subspan centered at 'center'
/// @param span the input span to create the subspan from 
/// @param center center of the new subspan
/// @return a subspan centered at 'center'
template <class Span, class C>
static constexpr auto make_subspan(Span span, C center) {

    return make_subspan(span, center, center);
}

} // namespace jada

#include <algorithm>
#include <iterator>

namespace jada {

template <class T> static constexpr void swap(T& a, T& b) {
    T c(std::move(a));
    a = std::move(b);
    b = std::move(c);
}

template <class ForwardIt1, class ForwardIt2>
constexpr void iter_swap(ForwardIt1 a, ForwardIt2 b) {
    jada::swap(*a, *b);
}

template <class ForwardIt, class Compare>
static constexpr ForwardIt
is_sorted_until(ForwardIt first, ForwardIt last, Compare comp) {
    if (first != last) {
        ForwardIt next = first;
        while (++next != last) {
            if (comp(*next, *first)) return next;
            first = next;
        }
    }
    return last;
}

template <class ForwardIt, class T>
static constexpr ForwardIt
upper_bound(ForwardIt first, ForwardIt last, const T& value) {
    ForwardIt                                                 it;
    typename std::iterator_traits<ForwardIt>::difference_type count = 0;
    typename std::iterator_traits<ForwardIt>::difference_type step  = 0;

    count = std::distance(first, last);

    while (count > 0) {
        it   = first;
        step = count / 2;
        std::advance(it, step);

        if (!(value < *it)) {
            first = ++it;
            count -= step + 1;
        } else
            count = step;
    }

    return first;
}

template <class BidirIt>
static constexpr void reverse(BidirIt first, BidirIt last) {
    using iter_cat = typename std::iterator_traits<BidirIt>::iterator_category;

    // Tag dispatch, e.g. calling reverse_impl(first, last, iter_cat()),
    // can be used in C++14 and earlier modes.
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag,
                                    iter_cat>) {
        if (first == last) return;

        for (--last; first < last; (void)++first, --last)
            jada::iter_swap(first, last);
    } else
        while (first != last && first != --last) jada::iter_swap(first++, last);
}

/*
template<class ForwardIt>
constexpr //< since C++20
ForwardIt is_sorted_until(ForwardIt first, ForwardIt last)
{
    return is_sorted_until(first, last, std::less<>());
}
*/

template <class BidirIt>
static constexpr bool next_permutation(BidirIt first, BidirIt last) {

    auto f       = [](auto lhs, auto rhs) { return lhs < rhs; };
    auto r_first = std::make_reverse_iterator(last);
    auto r_last  = std::make_reverse_iterator(first);
    auto left    = jada::is_sorted_until(r_first, r_last, f);
    if (left != r_last) {
        auto right = jada::upper_bound(r_first, left, *left);
        jada::iter_swap(left, right);
    }
    jada::reverse(left.base(), last);
    return left != r_last;
}

} // namespace jada

#include <array>
#include <cstddef>
#include <vector>

namespace jada {

enum class ConnectivityType { Star, Box };

template <size_t N> using direction = std::array<index_type, N>;

namespace detail {

template <size_t N, ConnectivityType CT>
static constexpr size_t neighbour_count() {

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

private:
    ///@brief Computes the star neighbours by permuting a positive and negative
    /// unit vectors pos =  {1,0,0,0 ... N}, neg = {-1, 0, 0, 0 ... N}
    ///@return std::array<direction<N>, 2*N> neighbours which correspond
    ///negative
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
        direction<N>                             combination{1};

        auto all_zero = [](auto arr) {
            for (size_t i = 0; i < std::size(arr); ++i) {
                if (arr[i] != index_type(0)) { return false; }
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

#include <array>
#include <cstddef>

namespace jada {

/// @brief Given coordinates of a subdomain returns the gird point offset wrt. position zero of the
/// global grid. The offset can be used to convert local indices to global indices by simple
/// addition: global = local + offset.
/// @param coords Coordinates of the local subdomain.
/// @param coord_dims The number of subdomains in each coordinate direction.
/// @param grid_dims The node count in the global grid
/// @return offset to the beginning index of the subdomain at 'coords'
template <class T1, class T2, class T3>
static constexpr auto offset(T1 coords, T2 coord_dims, T3 global_grid_dims) {

    runtime_assert(indices_in_bounds(coords, coord_dims), "Domain coordinates not in bounds.");
    using idx = typename decltype(global_grid_dims)::value_type;

    auto offset(coords);
    for (size_t i = 0; i < rank(coords); ++i) {
        offset[i] = idx(global_grid_dims[i] / coord_dims[i]) * coords[i];
    }
    return offset;
}

/// @brief Given coordinates of a subdomain returns the local grid point count in that subdomain. In
/// case of uneven splitting of the global_grid_dims to subdomains in a certain direction, the
/// reminding points are added to the last (largest coordinate) subdomain of that correspoding
/// direction. Example: global_grid_dims = {2,3}, coord_dims = {2,2}, the point count of the
/// subdomain at coords= {0,0} is {1,1} and the point count of the subdomain at coords = {0,1} is
/// {1,2}
/// @param coords Coordinates of the local subdomain.
/// @param coord_dims The number of subdomains in each coordinate direction.
/// @param grid_dims The node count in the global grid
/// @return the local dimensions of the subdomain at 'coords'
template <class T1, class T2, class T3>
static constexpr auto local_dimensions(T1 coords, T2 coord_dims, T3 global_grid_dims) {

    runtime_assert(indices_in_bounds(coords, coord_dims), "Domain coordinates not in bounds.");

    decltype(global_grid_dims) local_dims{};
    using idx = typename decltype(local_dims)::value_type;

    for (size_t i = 0; i < rank(coords); ++i) {
        local_dims[i] = idx(global_grid_dims[i] / coord_dims[i]);

        // Uneven points added to the last subdomain in the corresponding direction
        if (coords[i] == idx(coord_dims[i] - 1)) {
            local_dims[i] += idx(global_grid_dims[i] % coord_dims[i]);
        }
    }
    return local_dims;
}

} // namespace jada

#include <array>

//This file is directly copied from https://github.com/TartanLlama/ranges which provides support for the c++23 ranges for c++20

#ifndef TL_RANGES_CARTESIAN_PRODUCT_HPP
#define TL_RANGES_CARTESIAN_PRODUCT_HPP

#include <iterator>
#include <ranges>
#include <type_traits>
#include <tuple>
#include <utility>
#include <functional>
#include <algorithm>

//This chunk is from tl_ranges: meta.hpp file
namespace tl {

   template <class Tuple>
   constexpr inline std::size_t tuple_size = std::tuple_size_v<std::remove_cvref_t<Tuple>>;

   template <std::size_t N>
   using index_constant = std::integral_constant<std::size_t, N>;

   namespace meta {
      //Partially-apply the given template with the given arguments
      template <template <class...> class T, class... Args>
      struct partial {
         template <class... MoreArgs>
         struct apply {
            using type = T<Args..., MoreArgs...>;
         };
      };

      namespace detail {
         template <class T, template<class...> class Into, std::size_t... Idx>
         constexpr auto repeat_into_impl(std::index_sequence<Idx...>)
            ->Into < typename decltype((Idx, std::type_identity<T>{}))::type... > ;
      }

      //Repeat the given type T into the template Into N times
      template <class T, std::size_t N, template <class...> class Into>
      using repeat_into = decltype(tl::meta::detail::repeat_into_impl<T,Into>(std::make_index_sequence<N>{}));
   }
}

//This is the tl_ranges tuple_utility.hpp file
namespace tl {
   //If the size of Ts is 2, returns pair<Ts...>, otherwise returns tuple<Ts...>
   namespace detail {
      template<class... Ts>
      struct tuple_or_pair_impl : std::type_identity<std::tuple<Ts...>> {};
      template<class Fst, class Snd>
      struct tuple_or_pair_impl<Fst, Snd> : std::type_identity<std::pair<Fst, Snd>> {};
   }
   template<class... Ts>
   using tuple_or_pair = detail::tuple_or_pair_impl<Ts...>::type;

    
   template <class Tuple>
   constexpr auto min_tuple(Tuple&& tuple) {
      return std::apply([](auto... sizes) {
         return std::ranges::min({
           std::common_type_t<decltype(sizes)...>(sizes)...
            });
         }, std::forward<Tuple>(tuple));
   }

   template <class Tuple>
   constexpr auto max_tuple(Tuple&& tuple) {
      return std::apply([](auto... sizes) {
         return std::ranges::max({
           std::common_type_t<decltype(sizes)...>(sizes)...
            });
         }, std::forward<Tuple>(tuple));
   }
   

   //Call f on every element of the tuple, returning a new one
   template<class F, class... Tuples>
   constexpr auto tuple_transform(F&& f, Tuples&&... tuples)
   {
      if constexpr (sizeof...(Tuples) > 1) {
         auto call_at_index = []<std::size_t Idx, class Fu, class... Ts>
            (tl::index_constant<Idx>, Fu ff, Ts&&... ttuples) {
            return ff(std::get<Idx>(std::forward<Ts>(ttuples))...);
         };

         constexpr auto min_size = tl::min_tuple(std::tuple(tl::tuple_size<Tuples>...));

         return[&] <std::size_t... Idx>(std::index_sequence<Idx...>) {
            return tuple_or_pair < std::decay_t<decltype(call_at_index(tl::index_constant<Idx>{}, std::move(f), std::forward<Tuples>(tuples)...)) > ... >
               (call_at_index(tl::index_constant<Idx>{}, std::move(f), std::forward<Tuples>(tuples)...)...);
         }(std::make_index_sequence<min_size>{});
      }
      else if constexpr (sizeof...(Tuples) == 1) {
         return std::apply([&]<class... Ts>(Ts&&... elements) {
            return tuple_or_pair<std::invoke_result_t<F&, Ts>...>(
               std::invoke(f, std::forward<Ts>(elements))...
               );
         }, std::forward<Tuples>(tuples)...);
      }
      else {
         return std::tuple{};
      }
   }

    
   //Call f on every element of the tuple
   template<class F, class Tuple>
   constexpr auto tuple_for_each(F&& f, Tuple&& tuple)
   {
      return std::apply([&]<class... Ts>(Ts&&... elements) {
         (std::invoke(f, std::forward<Ts>(elements)), ...);
      }, std::forward<Tuple>(tuple));
   }
   

   template <class Tuple>
   constexpr auto tuple_pop_front(Tuple&& tuple) {
      return std::apply([](auto&& head, auto&&... tail) {
         return std::pair(std::forward<decltype(head)>(head), std::tuple(std::forward<decltype(tail)>(tail)...));
         }, std::forward<Tuple>(tuple));
   }

   namespace detail {
      template <class F, class V>
      constexpr auto tuple_fold_impl(F, V v) {
         return v;
      }
      template <class F, class V, class Arg, class... Args>
      constexpr auto tuple_fold_impl(F f, V v, Arg arg, Args&&... args) {
         return tl::detail::tuple_fold_impl(f, 
            std::invoke(f, std::move(v), std::move(arg)), 
            std::forward<Args>(args)...);
      }
   }

   template <class F, class T, class Tuple>
   constexpr auto tuple_fold(Tuple tuple, T t, F f) {
      return std::apply([&](auto&&... args) {
         return tl::detail::tuple_fold_impl(std::move(f), std::move(t), std::forward<decltype(args)>(args)...);
         }, std::forward<Tuple>(tuple));
   }

   template<class... Tuples>
   constexpr auto tuple_zip(Tuples&&... tuples) {
      auto zip_at_index = []<std::size_t Idx, class... Ts>
         (tl::index_constant<Idx>, Ts&&... ttuples) {
         return tuple_or_pair<std::decay_t<decltype(std::get<Idx>(std::forward<Ts>(ttuples)))>...>(std::get<Idx>(std::forward<Ts>(ttuples))...);
      };

      constexpr auto min_size = tl::min_tuple(std::tuple(tl::tuple_size<Tuples>...));

      return[&] <std::size_t... Idx>(std::index_sequence<Idx...>) {
         return tuple_or_pair<std::decay_t<decltype(zip_at_index(tl::index_constant<Idx>{}, std::forward<Tuples>(tuples)...))>... >
            (zip_at_index(tl::index_constant<Idx>{}, std::forward<Tuples>(tuples)...)...);
      }(std::make_index_sequence<min_size>{});
   }
}

//This is from tl_ranges common.hpp
namespace tl {
   namespace detail {
      template <class I>
      concept single_pass_iterator = std::input_or_output_iterator<I> && !std::forward_iterator<I>;

      template <typename... V>
      constexpr auto common_iterator_category() {
         if constexpr ((std::ranges::random_access_range<V> && ...))
            return std::random_access_iterator_tag{};
         else if constexpr ((std::ranges::bidirectional_range<V> && ...))
            return std::bidirectional_iterator_tag{};
         else if constexpr ((std::ranges::forward_range<V> && ...))
            return std::forward_iterator_tag{};
         else if constexpr ((std::ranges::input_range<V> && ...))
            return std::input_iterator_tag{};
         else
            return std::output_iterator_tag{};
      }
   }

   template <class... V>
   using common_iterator_category = decltype(detail::common_iterator_category<V...>());

   template <class R>
   concept simple_view = std::ranges::view<R> && std::ranges::range<const R> &&
      std::same_as<std::ranges::iterator_t<R>, std::ranges::iterator_t<const R>> &&
      std::same_as<std::ranges::sentinel_t<R>,
      std::ranges::sentinel_t<const R>>;

   struct as_sentinel_t {};
   constexpr inline as_sentinel_t as_sentinel;

   template <bool Const, class T>
   using maybe_const = std::conditional_t<Const, const T, T>;
}

//This is the tl_ranges basic_iterator.hpp file
namespace tl {
   template <std::destructible T>
   class basic_mixin : protected T {
   public:
      constexpr basic_mixin()
         noexcept(std::is_nothrow_default_constructible<T>::value)
         requires std::default_initializable<T> :
         T() {}
      constexpr basic_mixin(const T& t)
         noexcept(std::is_nothrow_copy_constructible<T>::value)
         requires std::copy_constructible<T> :
         T(t) {}
      constexpr basic_mixin(T&& t)
         noexcept(std::is_nothrow_move_constructible<T>::value)
         requires std::move_constructible<T> :
         T(std::move(t)) {}

      constexpr T& get() & noexcept { return *static_cast<T*>(this); }
      constexpr const T& get() const& noexcept { return *static_cast<T const*>(this); }
      constexpr T&& get() && noexcept { return std::move(*static_cast<T*>(this)); }
      constexpr const T&& get() const&& noexcept { return std::move(*static_cast<T const*>(this)); }
   };

   namespace cursor {
      namespace detail {
         template <class C>
         struct tags {
            static constexpr auto single_pass() requires requires { { C::single_pass } -> std::convertible_to<bool>; } {
               return C::single_pass;
            }
            static constexpr auto single_pass() { return false; }

            static constexpr auto contiguous() requires requires { { C::contiguous } -> std::convertible_to<bool>; } {
               return C::contiguous;
            }
            static constexpr auto contiguous() { return false; }
         };
      }
      template <class C>
      constexpr bool single_pass = detail::tags<C>::single_pass();

      template <class C>
      constexpr bool tagged_contiguous = detail::tags<C>::contiguous();

      namespace detail {
         template <class C>
         struct deduced_mixin_t {
            template <class T> static auto deduce(int)->T::mixin;
            template <class T> static auto deduce(...)->tl::basic_mixin<T>;
            using type = decltype(deduce<C>(0));
         };
      }

      template <class C>
      using mixin_t = detail::deduced_mixin_t<C>::type;

      template <class C>
      requires
         requires(const C& c) { c.read(); }
      using reference_t = decltype(std::declval<const C&>().read());

      namespace detail {
         template <class C>
         struct deduced_value_t {
            template<class T> static auto deduce(int)->T::value_type;
            template<class T> static auto deduce(...)->std::decay_t<reference_t<T>>;

            using type = decltype(deduce<C>(0));
         };
      }

      template <class C>
      requires std::same_as<typename detail::deduced_value_t<C>::type, std::decay_t<typename detail::deduced_value_t<C>::type>>
         using value_type_t = detail::deduced_value_t<C>::type;

      namespace detail {
         template <class C>
         struct deduced_difference_t {
            template <class T> static auto deduce(int)->T::difference_type;
            template <class T>
            static auto deduce(long)->decltype(std::declval<const T&>().distance_to(std::declval<const T&>()));
            template <class T>
            static auto deduce(...)->std::ptrdiff_t;

            using type = decltype(deduce<C>(0));
         };
      }

      template <class C>
      using difference_type_t = detail::deduced_difference_t<C>::type;

      template <class C>
      concept cursor = std::semiregular<std::remove_cv_t<C>>
         && std::semiregular<mixin_t<std::remove_cv_t<C>>>
         && requires {typename difference_type_t<C>; };

      template <class C>
      concept readable = cursor<C> && requires(const C & c) {
         c.read();
         typename reference_t<C>;
         typename value_type_t<C>;
      };

      template <class C>
      concept arrow = readable<C>
         && requires(const C & c) { c.arrow(); };

      template <class C, class T>
      concept writable = cursor<C>
         && requires(C & c, T && t) { c.write(std::forward<T>(t)); };

      template <class S, class C>
      concept sentinel_for = cursor<C> && std::semiregular<S>
         && requires(const C & c, const S & s) { {c.equal(s)} -> std::same_as<bool>; };

      template <class S, class C>
      concept sized_sentinel_for = sentinel_for<S, C> &&
         requires(const C & c, const S & s) {
            {c.distance_to(s)} -> std::same_as<difference_type_t<C>>;
      };

      template <class C>
      concept next = cursor<C> && requires(C & c) { c.next(); };

      template <class C>
      concept prev = cursor<C> && requires(C & c) { c.prev(); };

      template <class C>
      concept advance = cursor<C>
         && requires(C & c, difference_type_t<C> n) { c.advance(n); };

      template <class C>
      concept indirect_move = readable<C>
         && requires(const C & c) { c.indirect_move(); };

      template <class C, class O>
      concept indirect_swap = readable<C> && readable<O>
         && requires(const C & c, const O & o) {
         c.indirect_swap(o);
         o.indirect_swap(c);
      };

      template <class C>
      concept input = readable<C> && next<C>;
      template <class C>
      concept forward = input<C> && sentinel_for<C, C> && !single_pass<C>;
      template <class C>
      concept bidirectional = forward<C> && prev<C>;
      template <class C>
      concept random_access = bidirectional<C> && advance<C> && sized_sentinel_for<C, C>;
      template <class C>
      concept contiguous = random_access<C> && tagged_contiguous<C> && std::is_reference_v<reference_t<C>>;

      template <class C>
      constexpr auto cpp20_iterator_category() {
         if constexpr (contiguous<C>)
            return std::contiguous_iterator_tag{};
         else if constexpr (random_access<C>)
            return std::random_access_iterator_tag{};
         else if constexpr (bidirectional<C>)
            return std::bidirectional_iterator_tag{};
         else if constexpr (forward<C>)
            return std::forward_iterator_tag{};
         else
            return std::input_iterator_tag{};
      }
      template <class C>
      using cpp20_iterator_category_t = decltype(cpp20_iterator_category<C>());

      //There were a few changes in requirements on iterators between C++17 and C++20
      //See https://wg21.link/p2259 for discussion
      //- C++17 didn't have contiguous iterators
      //- C++17 input iterators required *it++ to be valid
      //- C++17 forward iterators required the reference type to be exactly value_type&/value_type const& (i.e. not a proxy)
      struct not_a_cpp17_iterator {};

      template <class C>
      concept reference_is_value_type_ref =
         (std::same_as<reference_t<C>, value_type_t<C>&> || std::same_as<reference_t<C>, value_type_t<C> const&>);

      template <class C>
      concept can_create_postincrement_proxy =
         (std::move_constructible<value_type_t<C>> && std::constructible_from<value_type_t<C>, reference_t<C>>);

      template <class C>
      constexpr auto cpp17_iterator_category() {
         if constexpr (random_access<C> && reference_is_value_type_ref<C>)
            return std::random_access_iterator_tag{};
         else if constexpr (bidirectional<C> && reference_is_value_type_ref<C>)
            return std::bidirectional_iterator_tag{};
         else if constexpr (forward<C> && reference_is_value_type_ref<C>)
            return std::forward_iterator_tag{};
         else if constexpr (can_create_postincrement_proxy<C>)
            return std::input_iterator_tag{};
         else
            return not_a_cpp17_iterator{};
      }
      template <class C>
      using cpp17_iterator_category_t = decltype(cpp17_iterator_category<C>());

      //iterator_concept and iterator_category are tricky; this abstracts them out.
      //Since the rules for iterator categories changed between C++17 and C++20
      //a C++20 iterator may have a weaker category in C++17,
      //or it might not be a valid C++17 iterator at all.
      //iterator_concept will be the C++20 iterator category.
      //iterator_category will be the C++17 iterator category, or it will not exist
      //in the case that the iterator is not a valid C++17 iterator.
      template <cursor C, class category = cpp17_iterator_category_t<C>>
      struct associated_types_category_base {
         using iterator_category = category;
      };
      template <cursor C>
      struct associated_types_category_base<C, not_a_cpp17_iterator> {};

      template <cursor C>
      struct associated_types : associated_types_category_base<C> {
         using iterator_concept = cpp20_iterator_category_t<C>;
         using value_type = cursor::value_type_t<C>;
         using difference_type = cursor::difference_type_t<C>;
         using reference = cursor::reference_t<C>;
      };

      namespace detail {
         // We assume a cursor is writeable if it's either not readable
         // or it is writeable with the same type it reads to
         template <class C>
         struct is_writable_cursor {
            template <readable T>
            requires requires (C c) {
               c.write(c.read());
            }
            static auto deduce()->std::true_type;

            template <readable T>
            static auto deduce()->std::false_type;

            template <class T>
            static auto deduce()->std::true_type;

            static constexpr bool value = decltype(deduce<C>())::value;
         };
      }
   }

   namespace detail {
      template <class T>
      struct post_increment_proxy {
      private:
         T cache_;

      public:
         template<typename U>
         constexpr post_increment_proxy(U&& t)
            : cache_(std::forward<U>(t))
         {}
         constexpr T const& operator*() const noexcept
         {
            return cache_;
         }
      };
   }

   template <cursor::input C>
   class basic_iterator :
      public cursor::mixin_t<C>
   {
   private:
      using mixin = cursor::mixin_t<C>;

      constexpr auto& cursor() noexcept { return this->mixin::get(); }
      constexpr auto const& cursor() const noexcept { return this->mixin::get(); }

      template <cursor::input>
      friend class basic_iterator;

      //TODO these need to change to support output iterators
      using reference_t = decltype(std::declval<C>().read());
      using const_reference_t = reference_t;

   public:
      using mixin::get;

      using value_type = cursor::value_type_t<C>;
      using difference_type = cursor::difference_type_t<C>;
      using reference = cursor::reference_t<C>;

      basic_iterator() = default;

      using mixin::mixin;

      constexpr explicit basic_iterator(C&& c)
         noexcept(std::is_nothrow_constructible_v<mixin, C&&>) :
         mixin(std::move(c)) {}

      constexpr explicit basic_iterator(C const& c)
         noexcept(std::is_nothrow_constructible_v<mixin, C const&>) :
         mixin(c) {}

      template <std::convertible_to<C> O>
      constexpr basic_iterator(basic_iterator<O>&& that)
         noexcept(std::is_nothrow_constructible<mixin, O&&>::value) :
         mixin(that.cursor()) {}

      template <std::convertible_to<C> O>
      constexpr basic_iterator(const basic_iterator<O>& that)
         noexcept(std::is_nothrow_constructible<mixin, const O&>::value) :
         mixin(std::move(that.cursor())) {}

      template <std::convertible_to<C> O>
      constexpr basic_iterator& operator=(basic_iterator<O>&& that) &
         noexcept(std::is_nothrow_assignable<C&, O&&>::value) {
         cursor() = std::move(that.cursor());
         return *this;
      }

      template <std::convertible_to<C> O>
      constexpr basic_iterator& operator=(const basic_iterator<O>& that) &
         noexcept(std::is_nothrow_assignable<C&, const O&>::value) {
         cursor() = that.cursor();
         return *this;
      }

      template <class T>
      requires
         (!std::same_as<std::decay_t<T>, basic_iterator> &&
            !cursor::next<C>&&
            cursor::writable<C, T>)
         constexpr basic_iterator& operator=(T&& t) &
         noexcept(noexcept(std::declval<C&>().write(static_cast<T&&>(t)))) {
         cursor() = std::forward<T>(t);
         return *this;
      }

      friend constexpr decltype(auto) iter_move(const basic_iterator& i)
         noexcept(noexcept(i.cursor().indirect_move()))
         requires cursor::indirect_move<C> {
         return i.cursor().indirect_move();
      }

      template <class O>
      requires cursor::indirect_swap<C, O>
         friend constexpr void iter_swap(
            const basic_iterator& x, const basic_iterator<O>& y)
         noexcept(noexcept((void)x.indirect_swap(y))) {
         x.indirect_swap(y);
      }

      //Input iterator
      constexpr decltype(auto) operator*() const
         noexcept(noexcept(std::declval<const C&>().read()))
         requires (cursor::readable<C> && !cursor::detail::is_writable_cursor<C>::value) {
         return cursor().read();
      }

      //Output iterator
      constexpr decltype(auto) operator*()
         noexcept(noexcept(reference_t{ cursor() }))
         requires (cursor::next<C>&& cursor::detail::is_writable_cursor<C>::value) {
         return reference_t{ cursor() };
      }

      //Output iterator
      constexpr decltype(auto) operator*() const
         noexcept(noexcept(
            const_reference_t{ cursor() }))
         requires (cursor::next<C>&& cursor::detail::is_writable_cursor<C>::value) {
         return const_reference_t{ cursor() };
      }

      constexpr basic_iterator& operator*() noexcept
         requires (!cursor::next<C>) {
         return *this;
      }

      // operator->: "Manual" deduction override,
      constexpr decltype(auto) operator->() const
         noexcept(noexcept(cursor().arrow()))
         requires cursor::arrow<C> {
         return cursor().arrow();
      }
      // operator->: Otherwise, if reference_t is an lvalue reference,
      constexpr decltype(auto) operator->() const
         noexcept(noexcept(*std::declval<const basic_iterator&>()))
         requires (cursor::readable<C> && !cursor::arrow<C>)
         && std::is_lvalue_reference<const_reference_t>::value{
         return std::addressof(**this);
      }

         // modifiers
         constexpr basic_iterator& operator++() & noexcept {
         return *this;
      }
      constexpr basic_iterator& operator++() &
         noexcept(noexcept(cursor().next()))
         requires cursor::next<C> {
         cursor().next();
         return *this;
      }

      //C++17 required that *it++ was valid.
      //For input iterators, we can't copy *this, so we need to create a proxy reference.
      constexpr auto operator++(int) &
         noexcept(noexcept(++std::declval<basic_iterator&>()) &&
            std::is_nothrow_move_constructible_v<value_type>&&
            std::is_nothrow_constructible_v<value_type, reference>)
         requires (cursor::single_pass<C>&&
            std::move_constructible<value_type>&&
            std::constructible_from<value_type, reference>) {
         detail::post_increment_proxy<value_type> p(**this);
         ++* this;
         return p;
      }

      //If we can't create a proxy reference, it++ is going to return void
      constexpr void operator++(int) &
         noexcept(noexcept(++std::declval<basic_iterator&>()))
         requires (cursor::single_pass<C> && !(std::move_constructible<value_type>&&
            std::constructible_from<value_type, reference>)) {
         (void)(++(*this));
      }

      //If C is a forward cursor then copying it is fine
      constexpr basic_iterator operator++(int) &
         noexcept(std::is_nothrow_copy_constructible_v<C>&&
            std::is_nothrow_move_constructible_v<C> &&
            noexcept(++std::declval<basic_iterator&>()))
         requires (!cursor::single_pass<C>) {
         auto temp = *this;
         ++* this;
         return temp;
      }

      constexpr basic_iterator& operator--() &
         noexcept(noexcept(cursor().prev()))
         requires cursor::bidirectional<C> {
         cursor().prev();
         return *this;
      }

      //Postfix decrement doesn't have the same issue as postfix increment
      //because bidirectional requires the cursor to be a forward cursor anyway
      //so copying it is fine.
      constexpr basic_iterator operator--(int) &
         noexcept(std::is_nothrow_copy_constructible<basic_iterator>::value&&
            std::is_nothrow_move_constructible<basic_iterator>::value &&
            noexcept(--std::declval<basic_iterator&>()))
         requires cursor::bidirectional<C> {
         auto tmp = *this;
         --* this;
         return tmp;
      }

      constexpr basic_iterator& operator+=(difference_type n) &
         noexcept(noexcept(cursor().advance(n)))
         requires cursor::random_access<C> {
         cursor().advance(n);
         return *this;
      }

      constexpr basic_iterator& operator-=(difference_type n) &
         noexcept(noexcept(cursor().advance(-n)))
         requires cursor::random_access<C> {
         cursor().advance(-n);
         return *this;
      }

      constexpr decltype(auto) operator[](difference_type n) const
         noexcept(noexcept(*(std::declval<basic_iterator&>() + n)))
         requires cursor::random_access<C> {
         return *(*this + n);
      }

      // non-template type-symmetric ops to enable implicit conversions
      friend constexpr difference_type operator-(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(y.cursor().distance_to(x.cursor())))
         requires cursor::sized_sentinel_for<C, C> {
         return y.cursor().distance_to(x.cursor());
      }
      friend constexpr bool operator==(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(x.cursor().equal(y.cursor())))
         requires cursor::sentinel_for<C, C> {
         return x.cursor().equal(y.cursor());
      }
      friend constexpr bool operator!=(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(!(x == y)))
         requires cursor::sentinel_for<C, C> {
         return !(x == y);
      }
      friend constexpr bool operator<(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(y - x))
         requires cursor::sized_sentinel_for<C, C> {
         return 0 < (y - x);
      }
      friend constexpr bool operator>(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(y - x))
         requires cursor::sized_sentinel_for<C, C> {
         return 0 > (y - x);
      }
      friend constexpr bool operator<=(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(y - x))
         requires cursor::sized_sentinel_for<C, C> {
         return 0 <= (y - x);
      }
      friend constexpr bool operator>=(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(y - x))
         requires cursor::sized_sentinel_for<C, C> {
         return 0 >= (y - x);
      }
   };

   namespace detail {
      template <class C>
      struct is_basic_iterator {
         template <class T>
         static auto deduce(basic_iterator<T> const&)->std::true_type;
         template <class T>
         static auto deduce(...)->std::false_type;
         static constexpr inline bool value = decltype(deduce(std::declval<C>()))::value;
      };
   }

   // basic_iterator nonmember functions
   template <class C>
   constexpr basic_iterator<C> operator+(
      const basic_iterator<C>& i, cursor::difference_type_t<C> n)
      noexcept(std::is_nothrow_copy_constructible<basic_iterator<C>>::value&&
         std::is_nothrow_move_constructible<basic_iterator<C>>::value &&
         noexcept(std::declval<basic_iterator<C>&>() += n))
      requires cursor::random_access<C> {
      auto tmp = i;
      tmp += n;
      return tmp;
   }
   template <class C>
   constexpr basic_iterator<C> operator+(
      cursor::difference_type_t<C> n, const basic_iterator<C>& i)
      noexcept(noexcept(i + n))
      requires cursor::random_access<C> {
      return i + n;
   }

   template <class C>
   constexpr basic_iterator<C> operator-(
      const basic_iterator<C>& i, cursor::difference_type_t<C> n)
      noexcept(noexcept(i + (-n)))
      requires cursor::random_access<C> {
      return i + (-n);
   }
   template <class C1, class C2>
   requires cursor::sized_sentinel_for<C1, C2>
      constexpr cursor::difference_type_t<C2> operator-(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept(
         rhs.get().distance_to(lhs.get()))) {
      return rhs.get().distance_to(lhs.get());
   }
   template <class C, class S>
   requires cursor::sized_sentinel_for<S, C>
      constexpr cursor::difference_type_t<C> operator-(
         const S& lhs, const basic_iterator<C>& rhs)
      noexcept(noexcept(rhs.get().distance_to(lhs))) {
      return rhs.get().distance_to(lhs);
   }
   template <class C, class S>
   requires cursor::sized_sentinel_for<S, C>
      constexpr cursor::difference_type_t<C> operator-(
         const basic_iterator<C>& lhs, const S& rhs)
      noexcept(noexcept(-(rhs - lhs))) {
      return -(rhs - lhs);
   }

   template <class C1, class C2>
   requires cursor::sentinel_for<C2, C1>
      constexpr bool operator==(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept(lhs.get().equal(rhs.get()))) {
      return lhs.get().equal(rhs.get());
   }
   template <class C, class S>
   requires cursor::sentinel_for<S, C>
      constexpr bool operator==(
         const basic_iterator<C>& lhs, const S& rhs)
      noexcept(noexcept(lhs.get().equal(rhs))) {
      return lhs.get().equal(rhs);
   }
   template <class C, class S>
   requires cursor::sentinel_for<S, C>
      constexpr bool operator==(
         const S& lhs, const basic_iterator<C>& rhs)
      noexcept(noexcept(rhs == lhs)) {
      return rhs == lhs;
   }

   template <class C1, class C2>
   requires cursor::sentinel_for<C2, C1>
      constexpr bool operator!=(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept(!(lhs == rhs))) {
      return !(lhs == rhs);
   }
   template <class C, class S>
   requires cursor::sentinel_for<S, C>
      constexpr bool operator!=(
         const basic_iterator<C>& lhs, const S& rhs)
      noexcept(noexcept(!lhs.get().equal(rhs))) {
      return !lhs.get().equal(rhs);
   }
   template <class C, class S>
   requires cursor::sentinel_for<S, C>
      constexpr bool operator!=(
         const S& lhs, const basic_iterator<C>& rhs)
      noexcept(noexcept(!rhs.get().equal(lhs))) {
      return !rhs.get().equal(lhs);
   }

   template <class C1, class C2>
   requires cursor::sized_sentinel_for<C1, C2>
      constexpr bool operator<(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept(lhs - rhs < 0)) {
      return (lhs - rhs) < 0;
   }

   template <class C1, class C2>
   requires cursor::sized_sentinel_for<C1, C2>
      constexpr bool operator>(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept((lhs - rhs) > 0)) {
      return (lhs - rhs) > 0;
   }

   template <class C1, class C2>
   requires cursor::sized_sentinel_for<C1, C2>
      constexpr bool operator<=(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept((lhs - rhs) <= 0)) {
      return (lhs - rhs) <= 0;
   }

   template <class C1, class C2>
   requires cursor::sized_sentinel_for<C1, C2>
      constexpr bool operator>=(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept((lhs - rhs) >= 0)) {
      return (lhs - rhs) >= 0;
   }

   template <class V, bool Const>
   class basic_sentinel {
      using Base = std::conditional_t<Const, const V, V>;

   public:
      std::ranges::sentinel_t<Base> end_{};
      basic_sentinel() = default;
      constexpr explicit basic_sentinel(std::ranges::sentinel_t<Base> end)
         : end_{ std::move(end) } {}

      constexpr basic_sentinel(basic_sentinel<V, !Const> other) requires Const&& std::
         convertible_to<std::ranges::sentinel_t<V>,
         std::ranges::sentinel_t<Base>>
         : end_{ std::move(other.end_) } {}

      constexpr auto end() const {
         return end_;
      }

      friend class basic_sentinel<V, !Const>;
   };
}

namespace std {
   template <class C>
   struct iterator_traits<tl::basic_iterator<C>> : tl::cursor::associated_types<C> {};
}

//This is the tl_ranges cartesian_product.hpp file
namespace tl {
   template <std::ranges::forward_range... Vs>
   requires (std::ranges::view<Vs> && ...) class cartesian_product_view
      : public std::ranges::view_interface<cartesian_product_view<Vs...>> {

      //random access + sized is allowed because we can jump all iterators to the end
      template <class... Ts>
      static constexpr bool am_common = (std::ranges::common_range<Ts> && ...)
         || ((std::ranges::random_access_range<Ts> && ...) && (std::ranges::sized_range<Ts> && ...));

      template <class... Ts>
      static constexpr bool am_sized = (std::ranges::sized_range<Ts> && ...);

      //requires common because we need to be able to cycle the iterators from begin to end in O(n)
      template <class... Ts>
      static constexpr bool am_bidirectional =
         ((std::ranges::bidirectional_range<Ts> && ...) && (std::ranges::common_range<Ts> && ...));

      //requires sized because we need to calculate new positions for iterators using arithmetic modulo the range size
      template <class... Ts>
      static constexpr bool am_random_access =
         ((std::ranges::random_access_range<Ts> && ...) &&
            (std::ranges::sized_range<Ts> && ...));

      template <class... Ts>
      static constexpr bool am_distanceable =
         ((std::sized_sentinel_for<std::ranges::iterator_t<Ts>, std::ranges::iterator_t<Ts>>) && ...)
         && am_sized<Ts...>;

      std::tuple<Vs...> bases_;

      template <bool Const>
      class cursor;

      //Wraps the end iterator for the 0th range.
      //This is all that's required because the cursor will only ever set the 0th iterator to end when
      //the cartesian product operation has completed.
      template <bool Const>
      class sentinel {
         using parent = std::conditional_t<Const, const cartesian_product_view, cartesian_product_view>;
         using first_base = decltype(std::get<0>(std::declval<parent>().bases_));
         std::ranges::sentinel_t<first_base> end_;

      public:
         sentinel() = default;
         sentinel(std::ranges::sentinel_t<first_base> end) : end_(std::move(end)) {}

         //const-converting constructor
         constexpr sentinel(sentinel<!Const> other) requires Const &&
            (std::convertible_to<std::ranges::sentinel_t<first_base>, std::ranges::sentinel_t<const first_base>>)
            : end_(std::move(other.end_)) {
         }

         template <bool>
         friend class cursor;
      };

      template <bool Const>
      class cursor {
         template<class T>
         using constify = std::conditional_t<Const, const T, T>;

         constify<std::tuple<Vs...>>* bases_;
         std::tuple<std::ranges::iterator_t<constify<Vs>>...> currents_{};

      public:
         using reference =
            std::tuple<std::ranges::range_reference_t<constify<Vs>>...>;
         using value_type =
            std::tuple<std::ranges::range_value_t<constify<Vs>>...>;

         using difference_type = std::ptrdiff_t;

         cursor() = default;
         constexpr explicit cursor(constify<std::tuple<Vs...>>* bases)
            : bases_{ bases }, currents_( tl::tuple_transform(std::ranges::begin, *bases) )
         {}

         //If the underlying ranges are common, we can get to the end by assigning from end
         constexpr explicit cursor(as_sentinel_t, constify<std::tuple<Vs...>>* bases)
            requires(std::ranges::common_range<Vs> && ...)
            : cursor{ bases }
         {
            std::get<0>(currents_) = std::ranges::end(std::get<0>(*bases_));
         }

         //If the underlying ranges are sized and random access, we can get to the end by moving it forward by size
         constexpr explicit cursor(as_sentinel_t, constify<std::tuple<Vs...>>* bases)
            requires(!(std::ranges::common_range<Vs> && ...) && (std::ranges::random_access_range<Vs> && ...) && (std::ranges::sized_range<Vs> && ...))
            : cursor{ bases }
         {
            std::get<0>(currents_) += std::ranges::size(std::get<0>(*bases_));
         }

         //const-converting constructor
         constexpr cursor(cursor<!Const> i) requires Const && (std::convertible_to<
            std::ranges::iterator_t<Vs>,
            std::ranges::iterator_t<constify<Vs>>> && ...)
            : bases_{ std::move(i.bases_) }, currents_{ std::move(i.currents_) } {}

         constexpr decltype(auto) read() const {
            return tuple_transform([](auto& i) -> decltype(auto) { return *i; }, currents_);
         }

         //Increment the iterator at std::get<N>(currents_)
         //If that iterator hits its end, recurse to std::get<N-1>
         template <std::size_t N = (sizeof...(Vs) - 1)>
         void next() {
            auto& it = std::get<N>(currents_);
            ++it;
            if (it == std::ranges::end(std::get<N>(*bases_))) {
               if constexpr (N > 0) {
                  it = std::ranges::begin(std::get<N>(*bases_));
                  next<N - 1>();
               }
            }
         }

         //Decrement the iterator at std::get<N>(currents_)
         //If that iterator was at its begin, cycle it to end and recurse to std::get<N-1>
         template <std::size_t N = (sizeof...(Vs) - 1)>
         void prev() requires (am_bidirectional<constify<Vs>...>) {
            auto& it = std::get<N>(currents_);
            if (it == std::ranges::begin(std::get<N>(*bases_))) {
               std::ranges::advance(it, std::ranges::end(std::get<N>(*bases_)));
               if constexpr (N > 0) {
                  prev<N - 1>();
               }
            }
            --it;
         }

         template <std::size_t N = (sizeof...(Vs) - 1)>
         void advance(difference_type n) requires (am_random_access<constify<Vs>...>) {
            auto& it = std::get<N>(currents_);
            auto& base = std::get<N>(*bases_);
            auto begin = std::ranges::begin(base);
            auto end = std::ranges::end(base);
            auto size = end - begin;

            auto distance_from_begin = it - begin;

            //Calculate where in the iterator cycle we should end up
            auto offset = (distance_from_begin + n) % size;

            //Calculate how many times incrementing this iterator would cause it to cycle round
            //This will be negative if we cycled by decrementing
            auto times_cycled = (distance_from_begin + n) / size - (offset < 0 ? 1 : 0);

            //Set the iterator to the correct new position
            it = begin + static_cast<difference_type>(offset < 0 ? offset + size : offset);

            if constexpr (N > 0) {
               //If this iterator cycled, then we need to advance the N-1th iterator
               //by the number of times it cycled
               if (times_cycled != 0) {
                  advance<N - 1>(times_cycled);
               }
            }
            else {
               //If we're the 0th iterator, then cycling should set the iterator to the end
               if (times_cycled > 0) {
                  std::ranges::advance(it, end);
               }
            }
         }

         constexpr bool equal(const cursor& rhs) const
            requires (std::equality_comparable<std::ranges::iterator_t<constify<Vs>>> && ...) {
            return currents_ == rhs.currents_;
         }

         constexpr bool equal(const sentinel<Const>& s) const {
            return std::get<0>(currents_) == s.end_;
         }

         template <std::size_t N = (sizeof...(Vs) - 1)>
         constexpr auto distance_to(cursor const& other) const
            requires (am_distanceable<constify<Vs>...>) {
            if constexpr (N == 0) {
               return std::ranges::distance(std::get<0>(currents_), std::get<0>(other.currents_));
            }
            else {
               auto distance = distance_to<N - 1>(other);
               auto scale = std::ranges::distance(std::get<N>(*bases_));
               auto diff = std::ranges::distance(std::get<N>(currents_), std::get<N>(other.currents_));
               return difference_type{ distance * scale + diff };
            }
         }

         friend class cursor<!Const>;
      };

   public:
      cartesian_product_view() = default;
      explicit cartesian_product_view(Vs... bases) : bases_(std::move(bases)...) {}

      constexpr auto begin() requires (!(simple_view<Vs> && ...)) {
         return basic_iterator{ cursor<false>(std::addressof(bases_)) };
      }
      constexpr auto begin() const requires (std::ranges::range<const Vs> && ...) {
         return basic_iterator{ cursor<true>(std::addressof(bases_)) };
      }

      constexpr auto end() requires (!(simple_view<Vs> && ...) && am_common<Vs...>) {
         return basic_iterator{ cursor<false>(as_sentinel, std::addressof(bases_)) };
      }

      constexpr auto end() const requires (am_common<const Vs...>) {
         return basic_iterator{ cursor<true>(as_sentinel, std::addressof(bases_)) };
      }

      constexpr auto end() requires(!(simple_view<Vs> && ...) && !am_common<Vs...>) {
         return sentinel<false>(std::ranges::end(std::get<0>(bases_)));
      }

      constexpr auto end() const requires ((std::ranges::range<const Vs> && ...) && !am_common<const Vs...>) {
         return sentinel<true>(std::ranges::end(std::get<0>(bases_)));
      }

      constexpr auto size() requires (am_sized<Vs...>) {
         //Multiply all the sizes together, returning the common type of all of them
         return std::apply([](auto&&... bases) {
            using size_type = std::common_type_t<std::ranges::range_size_t<decltype(bases)>...>;
            return (static_cast<size_type>(std::ranges::size(bases)) * ...);
            }, bases_);
      }

      constexpr auto size() const requires (am_sized<const Vs...>) {
         return std::apply([](auto&&... bases) {
            using size_type = std::common_type_t<std::ranges::range_size_t<decltype(bases)>...>;
            return (static_cast<size_type>(std::ranges::size(bases)) * ...);
            }, bases_);
      }
   };

   template <class... Rs>
   cartesian_product_view(Rs&&...)->cartesian_product_view<std::views::all_t<Rs>...>;

   namespace views {
      namespace detail {
         class cartesian_product_fn {
         public:
            constexpr std::ranges::empty_view<std::tuple<>> operator()() const noexcept {
               return {};
            }
            template <std::ranges::viewable_range... V>
            requires ((std::ranges::forward_range<V> && ...) && (sizeof...(V) != 0))
               constexpr auto operator()(V&&... vs) const {
               return tl::cartesian_product_view{ std::views::all(std::forward<V>(vs))... };
            }
         };
      }  // namespace detail

      inline constexpr detail::cartesian_product_fn cartesian_product;
   }  // namespace views

}  // namespace tl

#endif

namespace jada {

/// @brief Wrapper around ranges::cartesian_product
/// @tparam ...Rngs Pack of range types
/// @param ...rngs Pack of range values
/// @return A view of size = size(rng1) * size(rng2)... containing tuples of [(rng1[0], rng2[0]...),
/// ...]
template <class... Rngs> constexpr auto cartesian_product(Rngs&&... rngs) {
    return tl::views::cartesian_product(rngs...);
}

} // namespace jada

namespace jada {

/// @brief Returns a view of multi-dimensional index tuples
/// @param begin index set of begin indices
/// @param end index set of end indices
/// @return A view of index tuples from [begin, end)
template <class B, class E> static constexpr auto md_indices(B begin, E end) {

    using Idx = size_type;

    return [&]<Idx... Is>(std::integer_sequence<Idx, Is...>) {
        return cartesian_product(indices(std::get<Is>(begin),
    std::get<Is>(end))...);
    }
    (std::make_integer_sequence<Idx, Idx(rank(begin))>{});
    
}

/// @brief Returns all multi-dimensional indices spanned by the extent of the
/// input span
/// @param span the input span to query the extent from
/// @return A view of index tuples from [begin=0, extent(span) )
template <class Span> static constexpr auto all_indices(Span span) {

     return md_indices(std::array<size_type, rank(span)>{}, dimensions(span));
}

/*
template <size_t N>
static constexpr bool valid_direction(std::array<index_type, N> direction) {

    size_t sum = 0;
    for (size_t i = 0; i < N; ++i) { sum += std::abs(direction[i]); }
    return sum == 1;
}
*/

template <size_t N>
static constexpr auto boundary_indices(std::array<size_type, N>  dims,
                                       std::array<index_type, N> direction) {
    // runtime_assert(valid_direction(direction), "Invalid boundary direction in
    // for_each_boundary_index");
    std::array<index_type, N> begin{};
    std::array<index_type, N> end{};

    for (size_t i = 0; i < N; ++i) { end[i] = index_type(dims[i]); }

    for (size_t i = 0; i < N; ++i) {
        if (direction[i] == 1) { begin[i] = index_type(dims[i]) - 1; }
        if (direction[i] == -1) { end[i] = 1; }
    }

    return md_indices(begin, end);
}

} // namespace jada

#include <algorithm>
#include <execution>

#include <vector>

namespace jada {

struct counting_iterator {

private:
    using self = counting_iterator;

public:
    using value_type        = index_type;
    using difference_type   = index_type; //std::ptrdiff_t;
    using pointer           = index_type*;
    using reference         = index_type&;
    using iterator_category = std::random_access_iterator_tag;

    counting_iterator()
        : value(0) {}
    explicit counting_iterator(value_type v)
        : value(v) {}

    value_type operator*() const { return value; }
    value_type operator[](difference_type n) const { return value + n; }

    self& operator++() {
        ++value;
        return *this;
    }
    self operator++(int) {
        self result{value};
        ++value;
        return result;
    }
    self& operator--() {
        --value;
        return *this;
    }
    self operator--(int) {
        self result{value};
        --value;
        return result;
    }
    self& operator+=(difference_type n) {
        value += n;
        return *this;
    }
    self& operator-=(difference_type n) {
        value -= n;
        return *this;
    }

    friend self operator+(self const& i, difference_type n) {
        return self(i.value + n);
    }
    friend self operator+(difference_type n, self const& i) {
        return self(i.value + n);
    }
    friend difference_type operator-(self const& x, self const& y) {
        return x.value - y.value;
    }
    friend self operator-(self const& i, difference_type n) {
        return self(i.value - n);
    }

    friend bool operator==(self const& x, self const& y) {
        return x.value == y.value;
    }
    friend bool operator!=(self const& x, self const& y) {
        return x.value != y.value;
    }
    friend bool operator<(self const& x, self const& y) {
        return x.value < y.value;
    }
    friend bool operator<=(self const& x, self const& y) {
        return x.value <= y.value;
    }
    friend bool operator>(self const& x, self const& y) {
        return x.value > y.value;
    }
    friend bool operator>=(self const& x, self const& y) {
        return x.value >= y.value;
    }

private:
    value_type value;
};

} // namespace jada

namespace jada {

template <class Indices, class Op>
static constexpr void for_each_index(Indices indices, Op op) {

    /*
    std::for_each(
        std::execution::par,
        std::begin(indices),
        std::end(indices),
        [=](auto tpl) { op(tpl); });
    */
    
    std::for_each_n(std::execution::par,
                    counting_iterator(index_type(0)),
                    indices.size(),
                    [=](index_type i) { op(indices[i]); });
    
}

template <class B, class E, class Op>
static constexpr void for_each_index(B begin, E end, Op op) {

    for_each_index(md_indices(begin, end), op);
}

} // namespace jada

namespace jada {

template <size_t N, class Op>
static constexpr void for_each_boundary_index(
    std::array<index_type, N> direction, std::array<size_type, N> dims, Op op) {

    for_each_index(boundary_indices(dims, direction), op);
}

} // namespace jada

namespace jada {

/// @brief Creates an index handle which maps multidimensional indices to
/// multidimensional indices centered at the input center center. For example,
/// the returned handle created at indices center={1,2,3} can be indexed with
/// index {0,-1, 0} giving the data from the underlying span at index {1,1,3}.
/// Essentially, this handle just shifts the center of the index to some
/// constant location='center'.

/// @param in the input span from which data is indexed
/// @param center the center of the index_handle
/// @return an index handle centered at 'center'
template <class Span, class Idx>
static constexpr auto idxhandle_md_to_md(Span in, Idx center) {

    static_assert(rank(in) == rank(center),
                  "Rank mismatch in idxhandle_md_to_md.");

    // TODO: make it so that md_idx can be a parameter pack also
    return [=](auto md_idx) {
        auto new_span = make_subspan(in, center);
        return new_span(tuple_to_array(md_idx));
    };
}

/// @brief Creates an index handle that maps multidimensional indices to
/// one-dimensional offsets wrt. to the input index 'center' and the direction
/// Dir. For example, if center = {1,2,3}, Dir = 1, the returned index handle
/// 'f' can be indexed with f(1), f(-2), f(3) etc., where f(1) gives data of the
/// underlying span at index {1,3,3} and f(-1) gives data at index {1,1,3}.

/// @tparam Dir the direction along which the offsets are computed
/// @param in the input span from which data is indexed
/// @param center the center of the index_handle
/// @return an index handle centered at 'center'
template <size_t Dir, class Span, class Idx>
static constexpr auto idxhandle_md_to_oned(Span in, Idx center) {

    static_assert(rank(in) == rank(center),
                  "Rank mismatch in idxhandle_md_to_oned.");
    constexpr size_t N = rank(in);

    return [=](index_type oned_idx) {
        std::array<index_type, N> mod_idx{};
        mod_idx[Dir] = oned_idx;
        const auto h = idxhandle_md_to_md(in, center);
        return h(mod_idx);
    };
}

template <class Span, class Idx, class Dir>
static constexpr auto
idxhandle_boundary_md_to_oned(Span in, Idx center, Dir dir) {

    static_assert(rank(in) == rank(center),
                  "Rank mismatch in idxhandle_boundary_md_to_oned.");
    static_assert(rank(in) == rank(dir),
                  "Rank mismatch in idxhandle_boundary_md_to_oned.");

    constexpr size_t N = rank(in);

    // NOTE! Important to return by reference here so that assignment can be
    // used in boundary conditions
    using RT = decltype(in(tuple_to_array(center)));

    return [=](index_type oned_idx) -> RT {
        std::array<index_type, N> mod_idx{};
        for (size_t i = 0; i < N; ++i) { mod_idx[i] = oned_idx * dir[i]; }
        auto new_span = make_subspan(in, center);
        return new_span(mod_idx);
    };
}

} // namespace jada

namespace jada {

/// @brief Evaluates the input tiled stencil operation 'op' on all input 'indices' of the input span
/// 'in' and stores the result to the output span 'out'.
/// @tparam Dir the direction along which the tiled stencil is formed
/// @param in input span
/// @param out output span
/// @param op the tiled stencil operation
/// @param indices the indices in which to evaluate the operation in
template <size_t Dir, class Span1, class Span2, class Op, class Indices>
void evaluate(Span1 in, Span2 out, Op op, Indices indices) {

    auto new_op = [=] (auto idx){
        const auto stencil = idxhandle_md_to_oned<Dir>(in, idx);
        out(tuple_to_array(idx)) = op(stencil);
    };

    for_each_index(indices, new_op);
    
}

/// @brief Evaluates the input 'op' on all possible indices based on the padding requirement of the
/// input operation. If the padding requirement of the operation is 2 and the spans are
/// one-dimensional, the operation is evaluated at indices [2, size(in) - 2]. The operations are
/// evaluated on the input span 'in' and results are stored in the same indices of the output span
/// 'out'.
/// @tparam Dir the direction along which the tiled stencil is formed
/// @param in input span
/// @param out output span
/// @param op the tiled stencil operation
template <size_t Dir, class Span1, class Span2, class Op>
void evaluate_tiled(Span1 in, Span2 out, Op op) {
    static_assert(rank(in) == rank(out), "Rank mismatch in evaluate tiled.");

    runtime_assert(extent(in) == extent(out), "Dimension mismatch");

    auto          end = dimensions(in);
    decltype(end) begin{};

    std::get<Dir>(begin) += op.padding;
    std::get<Dir>(end) -= op.padding;

    evaluate<Dir>(in, out, op, md_indices(begin, end));
}

} // namespace jada

#include <array>

namespace jada {

template <size_t N, class Span, class Op>
static constexpr void
evaluate_boundary(Span in, Op op, std::array<index_type, N> dir) {

    auto new_op = [=](auto idx) {
        const auto stencil = idxhandle_boundary_md_to_oned(in, idx, dir);
        op(stencil);
    };

    for_each_index(boundary_indices(dimensions(in), dir), new_op);
}

} // namespace jada

