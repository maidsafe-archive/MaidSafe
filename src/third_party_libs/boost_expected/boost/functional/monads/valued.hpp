// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_FUNCTIONAL_VALUED_HPP
#define BOOST_FUNCTIONAL_VALUED_HPP

#include <boost/expected/config.hpp>
#include <boost/functional/type_traits_t.hpp>
#include <boost/functional/monads/rebindable.hpp>
#include <boost/functional/monads/categories/forward.hpp>
#include <boost/functional/monads/categories/default.hpp>
#include <utility>
#include <stdexcept>
#include <type_traits>

namespace boost
{
namespace functional
{
  namespace category
  {
    struct pvalued {};
  }

//  namespace type_class
//  {
//    struct valued {
//      template <class T, class Void=void>
//      struct instance : std::false_type {};
//      struct requires : identity<tuple<rebindable> > {};
//
//    };
//  }

  template <class T>
  struct valued_traits  : std::false_type {};

  template <class M>
  struct is_valued : std::integral_constant<bool, is_rebindable<M>::value &&
    valued_traits<M>::value
  >
  {};

  template <class M> using if_pvalued =
      typename std::enable_if<is_valued<M>::value, valued_traits<M> >::type;

namespace valued
{
  using namespace ::boost::functional::rebindable;
  // bad_access exception class.
  class bad_access : public std::logic_error
  {
    public:
      bad_access()
      : std::logic_error("Bad access on PossibleValued object.")
      {}

      // todo - Add implicit/explicit conversion to error_type ?
  };

  template <class M, class Traits = if_pvalued<decay_t<M>> >
  BOOST_CONSTEXPR auto
  has_value(M&& e) -> decltype(Traits::has_value(std::forward<M>(e)))
  {
    return Traits::has_value(std::forward<M>(e));
  }

  template <class M, class Traits = if_pvalued<decay_t<M>> >
  BOOST_CONSTEXPR auto
  deref(M&& e) -> decltype(Traits::deref(std::forward<M>(e)))
  {
    return Traits::deref(std::forward<M>(e));
  }

  template <class M, class Traits = if_pvalued<decay_t<M>> >
  static BOOST_CONSTEXPR auto
  value(M&& e) -> decltype(Traits::get_value(std::forward<M>(e)))
  {
    return Traits::get_value(std::forward<M>(e));
  }

}

template <>
struct valued_traits<category::default_> : std::true_type {
  template <class M>
  static BOOST_CONSTEXPR auto get_value(M&& m) -> decltype (has_value(m) ? deref(m) : throw valued::bad_access(), deref(m))
  {
    return has_value(m) ? deref(m) : throw valued::bad_access(),deref(m);
  }
};

template <>
struct valued_traits<category::forward> : valued_traits<category::default_>
{
  template <class M>
  static BOOST_CONSTEXPR bool has_value(M&& m)
  { return m.has_value(); }

  template <class M>
  static BOOST_CONSTEXPR auto deref(M&& m) -> decltype(m.deref())
  { return m.deref(); }

};



}
}

#endif // BOOST_FUNCTIONAL_VALUED_HPP
