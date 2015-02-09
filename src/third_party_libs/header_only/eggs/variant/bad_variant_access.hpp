//! \file eggs/variant/bad_variant_access.hpp
// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_BAD_VARIANT_ACCESS_HPP
#define EGGS_VARIANT_BAD_VARIANT_ACCESS_HPP

#include <stdexcept>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

namespace eggs { namespace variants
{
    ///////////////////////////////////////////////////////////////////////////
    //! class bad_variant_access : public std::logic_error
    //!
    //! The class `bad_variant_access` defines the type of objects thrown as
    //! exceptions to report the situation where an attempt is made to access
    //! an inactive member of a `variant` object.
    class bad_variant_access
      : public std::logic_error
    {
    public:
        //! bad_variant_access();
        //!
        //! \effects Constructs an object of class `bad_variant_access`.
        //!  `what()` returns an implementation-defined NTBS.
        bad_variant_access()
          : std::logic_error{"bad_variant_access"}
        {}

        //! explicit bad_variant_access(std::string const& what_arg);
        //!
        //! \effects Constructs an object of class `bad_variant_access`.
        //!  `strcmp(what(), what_arg.c_str()) == 0`.
        explicit bad_variant_access(std::string const& what_arg)
          : std::logic_error{what_arg}
        {}

        //! explicit bad_variant_access(char const* what_arg);
        //!
        //! \effects Constructs an object of class `bad_variant_access`.
        //!  `strcmp(what(), what_arg) == 0`.
        explicit bad_variant_access(char const* what_arg)
          : std::logic_error{what_arg}
        {}
    };
}}

#include <eggs/variant/detail/config/suffix.hpp>

#endif /*EGGS_VARIANT_BAD_VARIANT_ACCESS_HPP*/
