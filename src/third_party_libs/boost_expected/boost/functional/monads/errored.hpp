// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_FUNCTIONAL_ERRORED_HPP
#define BOOST_FUNCTIONAL_ERRORED_HPP

#include <boost/functional/type_traits_t.hpp>
#include <boost/functional/monads/valued.hpp>
#include <boost/functional/monads/categories/forward.hpp>
#include <boost/functional/monads/categories/default.hpp>
#include <utility>
#include <type_traits>

namespace boost
{
namespace functional
{

  template <class T>
  struct errored_traits  : std::false_type {};

  template <>
  struct errored_traits<category::default_>  : std::true_type {};
  template <>
  struct errored_traits<category::forward>  : errored_traits<category::default_>
  {
    template <class M>
    using error_type = typename M::error_type;
    template <class M>
    using errored_type = typename M::errored_type;

    template <class M>
    static BOOST_CONSTEXPR auto get_errored(M&& m) -> decltype(m.get_errored())
    { return m.get_errored();}

    template <class M>
    static BOOST_CONSTEXPR auto error(M&& m) -> decltype(m.error())
    { return m.error();}
  };

  template <class M>
  struct is_errored : std::integral_constant<bool, is_valued<M>::value &&
    errored_traits<M>::value
  >
  {};

  template <class M> using if_errored =
      typename std::enable_if<is_errored<M>::value, errored_traits<M> >::type;

namespace errored
{
  using namespace ::boost::functional::valued;

  template <class M, class Traits = if_errored<decay_t<M>> >
  using errored_type = typename Traits::template errored_type<M>;

  template <class M, class Traits = if_errored<decay_t<M>> >
  using error_type = typename Traits::template error_type<M>;

  template <class M, class Traits = if_errored<decay_t<M>> >
  static BOOST_CONSTEXPR auto
  get_errored(M&& e) -> decltype(Traits::get_errored(std::forward<M>(e)))
  {
    return Traits::get_errored(std::forward<M>(e));
  }
  template <class M, class Traits = if_errored<decay_t<M>> >
  static BOOST_CONSTEXPR auto
  error(M&& e) -> decltype(Traits::error(std::forward<M>(e)))
  {
    return Traits::error(std::forward<M>(e));
  }

}
}
}

#endif // BOOST_FUNCTIONAL_ERRORED_HPP
