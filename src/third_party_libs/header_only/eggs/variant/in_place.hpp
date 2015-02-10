//! \file eggs/variant/in_place.hpp
// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_IN_PLACE_HPP
#define EGGS_VARIANT_IN_PLACE_HPP

#include <eggs/variant/detail/pack.hpp>

#include <cstddef>
#include <stdexcept>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

namespace eggs { namespace variants
{
    ///////////////////////////////////////////////////////////////////////////
    //! struct in_place_t {};
    //!
    //! The struct `in_place_t` is an empty structure type used as a unique
    //! type to disambiguate constructor and function overloading.
    //! Specifically, `variant<Ts...>` has a constructor with an unspecified
    //! first parameter that matches an expression of the form `in_place<T>`,
    //! followed by a parameter pack; this indicates that `T` should be
    //! constructed in-place (as if by a call to a placement new expression)
    //! with the forwarded pack expansion as arguments for the initialization
    //! of `T`.
    struct in_place_t {};

    //! template <std::size_t I>
    //! in_place_t in_place(unspecified<I>);
    template <std::size_t I>
    inline in_place_t in_place(detail::pack_c<std::size_t, I> = {})
    {
        return {};
    }

    //! template <class T>
    //! in_place_t in_place(unspecified<T>);
    template <typename T>
    inline in_place_t in_place(detail::pack<T> = {})
    {
        return {};
    }
}}

#include <eggs/variant/detail/config/suffix.hpp>

#endif /*EGGS_VARIANT_IN_PLACE_HPP*/
