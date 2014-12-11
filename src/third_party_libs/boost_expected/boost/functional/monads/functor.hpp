// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_FUNCTIONAL_FUNCTOR_HPP
#define BOOST_FUNCTIONAL_FUNCTOR_HPP

#include <boost/functional/type_traits_t.hpp>
#include <boost/functional/meta.hpp>
#include <boost/functional/monads/rebindable.hpp>
#include <boost/functional/monads/categories/forward.hpp>
#include <boost/functional/monads/categories/default.hpp>
#include <utility>
#include <type_traits>

namespace boost
{
namespace functional
{

  template <class Mo>
  struct functor_traits : std::false_type {};

  template <>
  struct functor_traits<category::default_> : std::true_type {};
  template <>
  struct functor_traits<category::forward> : functor_traits<category::default_>
  {

    template <class F, class M0, class ...M, class FR = decltype( std::declval<F>()(*std::declval<M0>(), *std::declval<M>()...) )>
    static auto
    map(F&& f, M0&& m0, M&& ...ms) -> typename rebindable::rebind<decay_t<M0>, FR>::type
    {
      return M0::map(std::forward<F>(f), std::forward<M0>(m0), std::forward<M>(ms)...);
    }
  };

  template <class M>
  struct is_functor : std::integral_constant<bool, is_rebindable<M>::value && functor_traits<M>::value
  >
  {};

  template <class M> using if_functor =
      typename std::enable_if<is_functor<M>::value, functor_traits<M> >::type;

namespace functor
{
  using namespace ::boost::functional::rebindable;

  template <class F, class M0, class ...M, class Traits = if_functor<decay_t<M0>> >
  auto
  map(F&& f, M0&& m0, M&& ...m)
  -> decltype(Traits::map(std::forward<F>(f), std::forward<M0>(m0), std::forward<M>(m)...))
  {
    return Traits::map(std::forward<F>(f),std::forward<M0>(m0), std::forward<M>(m)...);
  }

  template <class F, class M, class = if_functor<decay_t<M> > >
  auto operator^(F&& f, M&& m)
  -> decltype(map(std::forward<F>(f), std::forward<M>(m)))
  {
    return map(std::forward<F>(f), std::forward<M>(m));
  }

}
}
}

#endif // BOOST_FUNCTIONAL_FUNCTOR_HPP
