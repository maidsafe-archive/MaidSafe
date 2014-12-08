// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_MONADS_CATEGORIES_POINTER_LIKE_HPP
#define BOOST_EXPECTED_MONADS_CATEGORIES_POINTER_LIKE_HPP

#include <boost/functional/monads/errored.hpp>
#include <cstddef>

namespace boost
{
namespace functional
{
  namespace category
  {
    struct pointer_like {};
  }

  template <class T>
  struct rebindable_traits<T*> : rebindable_traits<category::default_>
  {
    template <class M>
    using value_type = T;

    template <class M>
    struct type_constructor {
      template <class U> using type = U*;
    };
  };

  template <>
  struct valued_traits<category::pointer_like> : valued_traits<category::default_>
  {
    template <class M>
    static BOOST_CONSTEXPR bool has_value(M&& m) { return bool(m); }

    template <class M>
    static BOOST_CONSTEXPR auto deref(M&& m) -> decltype(*m) { return *m; }

  };

  template <class T>
  struct valued_traits<T*> : valued_traits<category::pointer_like>  {};

  template <>
  struct errored_traits<category::pointer_like>  : errored_traits<category::default_>
  {
    template <class M>
    using error_type = std::nullptr_t;
    template <class M>
    using errored_type = std::nullptr_t;

    template <class M>
    static BOOST_CONSTEXPR std::nullptr_t get_errored(M&& m)
    { return nullptr;}

    template <class M>
    static BOOST_CONSTEXPR std::nullptr_t error(M&& m)
    { return nullptr;}
  };
}
}

#endif // BOOST_EXPECTED_MONADS_CATEGORIES_POINTER_LIKE_HPP
