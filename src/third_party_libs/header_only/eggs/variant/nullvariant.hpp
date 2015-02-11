//! \file eggs/variant/nullvariant.hpp
// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_NULLVARIANT_HPP
#define EGGS_VARIANT_NULLVARIANT_HPP

#include <type_traits>

#include <eggs/variant/detail/config/prefix.hpp>

namespace eggs { namespace variants
{
    struct nullvariant_t;

    namespace detail
    {
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        struct is_null_variant
          : std::false_type
        {};

        template <>
        struct is_null_variant<nullvariant_t>
          : std::true_type
        {};

        template <>
        struct is_null_variant<nullvariant_t const>
          : std::true_type
        {};

        template <>
        struct is_null_variant<nullvariant_t volatile>
          : std::true_type
        {};

        template <>
        struct is_null_variant<nullvariant_t const volatile>
          : std::true_type
        {};
    }

    ///////////////////////////////////////////////////////////////////////////
    //! struct nullvariant_t {see below};
    //!
    //! The struct `nullvariant_t` is an empty structure type used as a unique
    //! type to indicate the state of not having an active member for `variant`
    //! objects. In particular, `variant<Ts...>` has a constructor with
    //! `nullvariant_t` as a single argument; this indicates that a variant
    //! object with no active member shall be constructed.
    //!
    //! Type `nullvariant_t` shall not have a default constructor. It shall be
    //! a literal type. Constant `nullvariant` shall be initialized with an
    //! argument of literal type.
    struct nullvariant_t
    {
        EGGS_CXX11_CONSTEXPR explicit nullvariant_t(int) EGGS_CXX11_NOEXCEPT {}
    };

    //! constexpr nullvariant_t nullvariant(unspecified);
    EGGS_CXX11_STATIC_CONSTEXPR nullvariant_t nullvariant{0};
}}

#include <eggs/variant/detail/config/suffix.hpp>

#endif /*EGGS_VARIANT_NULLVARIANT_HPP*/
