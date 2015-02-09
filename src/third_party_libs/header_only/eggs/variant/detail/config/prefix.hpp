//! \file eggs/variant/detail/config/prefix.hpp
// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// no header guards

/// constexpr support
#ifndef EGGS_CXX11_CONSTEXPR
#  if defined(_MSC_FULL_VER)
#    define EGGS_CXX11_CONSTEXPR
#  else
#    define EGGS_CXX11_CONSTEXPR constexpr
#  endif
#  define EGGS_CXX11_CONSTEXPR_DEFINED
#endif

#ifndef EGGS_CXX11_STATIC_CONSTEXPR
#  if defined(_MSC_FULL_VER)
#    define EGGS_CXX11_STATIC_CONSTEXPR static const
#  else
#    define EGGS_CXX11_STATIC_CONSTEXPR static constexpr
#  endif
#  define EGGS_CXX11_STATIC_CONSTEXPR_DEFINED
#endif

#ifndef EGGS_CXX14_CONSTEXPR
#  if __cplusplus < 201402L
#    define EGGS_CXX14_CONSTEXPR
#  elif defined __GNUC__ && !defined(__clang__)
#    define EGGS_CXX14_CONSTEXPR
#  else
#    define EGGS_CXX14_CONSTEXPR constexpr
#  endif
#  define EGGS_CXX14_CONSTEXPR_DEFINED
#endif

/// defaulted functions support
#ifndef EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS
#  if defined(_MSC_FULL_VER) && _MSC_FULL_VER < 190021730
#    define EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS 0
#  else
#    define EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS 1
#  endif
#  define EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS_DEFINED
#endif

/// deleted functions support
#ifndef EGGS_CXX11_HAS_DELETED_FUNCTIONS
#  if defined(_MSC_FULL_VER) && _MSC_FULL_VER < 190021730
#    define EGGS_CXX11_HAS_DELETED_FUNCTIONS 0
#  else
#    define EGGS_CXX11_HAS_DELETED_FUNCTIONS 1
#  endif
#  define EGGS_CXX11_HAS_DELETED_FUNCTIONS_DEFINED
#endif

/// noexcept support
#ifndef EGGS_CXX11_NOEXCEPT
#  if defined(_MSC_FULL_VER) && _MSC_FULL_VER < 190021730
#    define EGGS_CXX11_NOEXCEPT
#  else
#    define EGGS_CXX11_NOEXCEPT noexcept
#  endif
#  define EGGS_CXX11_NOEXCEPT_DEFINED
#endif

#ifndef EGGS_CXX11_NOEXCEPT_IF
#  if defined(_MSC_FULL_VER) && _MSC_FULL_VER < 190021730
#    define EGGS_CXX11_NOEXCEPT_IF(...)
#  else
#    define EGGS_CXX11_NOEXCEPT_IF(...) noexcept(__VA_ARGS__)
#  endif
#  define EGGS_CXX11_NOEXCEPT_IF_DEFINED
#endif

#ifndef EGGS_CXX11_NOEXCEPT_EXPR
#  if defined(_MSC_FULL_VER)
#    define EGGS_CXX11_NOEXCEPT_EXPR(...) false
#  else
#    define EGGS_CXX11_NOEXCEPT_EXPR(...) noexcept(__VA_ARGS__)
#  endif
#  define EGGS_CXX11_NOEXCEPT_EXPR_DEFINED
#endif

/// noreturn support
#ifndef EGGS_CXX11_NORETURN
#  if defined(_MSC_FULL_VER)
#    define EGGS_CXX11_NORETURN __declspec(noreturn)
#  elif defined(__GNUC__) && !defined(__clang__)
#    if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
#      define EGGS_CXX11_NORETURN __attribute__ ((__noreturn__))
#    else
#      define EGGS_CXX11_NORETURN [[noreturn]]
#    endif
#  elif defined(__clang__)
#    if !__has_feature(cxx_attributes)
#      define EGGS_CXX11_NORETURN __attribute__ ((__noreturn__))
#    else
#      define EGGS_CXX11_NORETURN [[noreturn]]
#    endif
#  else
#    define EGGS_CXX11_NORETURN [[noreturn]]
#  endif
#  define EGGS_CXX11_NORETURN_DEFINED
#endif

/// sfinae for expressions support
#ifndef EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
#  if defined(_MSC_FULL_VER)
#    define EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS 0
#  elif defined __GNUC__ && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 9)) && !defined(__clang__)
#    define EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS 0
#  else
#    define EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS 1
#  endif
#  define EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS_DEFINED
#endif

/// variable templates support
#ifndef EGGS_CXX14_HAS_VARIABLE_TEMPLATES
#  if __cplusplus < 201402L
#    define EGGS_CXX14_HAS_VARIABLE_TEMPLATES 0
#  elif defined __GNUC__ && !defined(__clang__)
#    define EGGS_CXX14_HAS_VARIABLE_TEMPLATES 0
#  elif defined(__clang__)
#    define EGGS_CXX14_HAS_VARIABLE_TEMPLATES __has_feature(cxx_variable_templates)
#  else
#    define EGGS_CXX14_HAS_VARIABLE_TEMPLATES 1
#  endif
#  define EGGS_CXX14_HAS_VARIABLE_TEMPLATES_DEFINED
#endif

/// std::aligned_union support
#ifndef EGGS_CXX11_STD_HAS_ALIGNED_UNION
#  if defined(__GLIBCXX__)
#    define EGGS_CXX11_STD_HAS_ALIGNED_UNION 0
#  else
#    define EGGS_CXX11_STD_HAS_ALIGNED_UNION 1
#  endif
#  define EGGS_CXX11_STD_HAS_ALIGNED_UNION_DEFINED
#endif

/// std::integer_sequence support
#ifndef EGGS_CXX14_STD_HAS_INTEGER_SEQUENCE
#  if __cplusplus < 201402L
#    define EGGS_CXX14_STD_HAS_INTEGER_SEQUENCE 0
#  else
#    define EGGS_CXX14_STD_HAS_INTEGER_SEQUENCE 1
#  endif
#  define EGGS_CXX14_STD_HAS_INTEGER_SEQUENCE_DEFINED
#endif

/// std::is_nothrow_* support
#ifndef EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
#  if defined(__GLIBCXX__) && !defined(_GLIBCXX_NOEXCEPT)
#    define EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS 0
#  else
#    define EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS 1
#  endif
#  define EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS_DEFINED
#endif

/// std::is_trivially_copyable support
#ifndef EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE
#  if defined(__GLIBCXX__)
#    define EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE 0
#  elif defined (_CPPLIB_VER)
#    define EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE 0
#  else
#    define EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE 1
#  endif
#  define EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE_DEFINED
#endif

/// std::is_trivially_destructible support
#ifndef EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE
#  if defined(__GLIBCXX__) && __GLIBCXX__ < 20130531
#    define EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE 0
#  else
#    define EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE 1
#  endif
#  define EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE_DEFINED
#endif
