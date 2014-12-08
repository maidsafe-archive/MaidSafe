// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_FUNCTIONAL_MONAD_HPP
#define BOOST_FUNCTIONAL_MONAD_HPP

#include <boost/functional/type_traits_t.hpp>
#include <boost/functional/meta.hpp>
#include <boost/functional/monads/functor.hpp>
#include <boost/functional/monads/categories/forward.hpp>
#include <boost/functional/monads/categories/default.hpp>
#include <utility>
#include <type_traits>

namespace boost
{
namespace functional
{
//inline namespace functional_v0_1
//{

  template <class Mo>
  struct monad_traits : std::false_type {};

  template <class M>
  struct is_monad : std::integral_constant<bool, is_functor<M>::value &&
  monad_traits<M>::value
  >
  {};

  template <class M> using if_monad =
      typename std::enable_if<is_monad<M>::value, monad_traits<M> >::type;

namespace monad
{

  using namespace ::boost::functional::functor;

  template <class M, class T,
      class Mo = apply<M,decay_t<T>>, class Traits = if_monad<Mo> >
  Mo make(T&& v)
  {
    return Traits::template make<M>(std::forward<T>(v));
  }

  template <template <class ...> class M, class T,
      class Mo = M<decay_t<T>>, class Traits = if_monad<decay_t<Mo>> >
  Mo make(T&& v)
  {
    return Traits::template make<lift<M>>(std::forward<T>(v));
  }

  template <class M, class F, class Traits = if_monad<decay_t<M>> >
  auto
  bind(M&& m, F&& f) -> decltype(Traits::bind(std::forward<M>(m), std::forward<F>(f)))
  {
    return Traits::bind(std::forward<M>(m), std::forward<F>(f));
  }

  template <class M1, class M2, class Traits = if_monad<decay_t<M1>>, class = if_monad<decay_t<M2>> >
  auto mdo(M1&& m1, M2&& m2)
  -> decltype( Traits::mdo(std::forward<M1>(m1), std::forward<M2>(m2)) )
  {
    return Traits::mdo(std::forward<M1>(m1), std::forward<M2>(m2)) ;
  }

  template <class M, class F, class Traits = if_monad<decay_t<M>> >
  auto operator&(M&& m, F&& f)
  -> decltype(bind(std::forward<M>(m), std::forward<F>(f)))
  {
    return bind(std::forward<M>(m),std::forward<F>(f));
  }

  template <class M1, class M2, class Traits = if_monad<decay_t<M1>>, class = if_monad<decay_t<M2>> >
  auto operator>>(M1&& m1, M2&& m2)
  -> decltype( mdo(std::forward<M1>(m1), std::forward<M2>(m2)) )

  {
    return mdo(std::forward<M1>(m1), std::forward<M2>(m2));
  }
}

template <>
struct monad_traits<category::default_> : std::true_type
{
  template <class M1, class M2 >
  static M2 mdo(M1&& m1, M2&& m2)
  {
    return monad::bind(std::forward<M1>(m1), [&](rebindable::value_type<decay_t<M1>> ) { return std::forward<M2>(m2); });
  }

};

template <>
struct monad_traits<category::forward> : monad_traits<category::default_>
{
  // make use of constructor
  template <class M, class T>
  static apply<M, T> make(T&& v)
  {
    return apply<M, T>(std::forward<T>(v));
  }

  // make use of member function
  template <class M, class F>
  static auto
  bind(M&& m, F&& f) -> decltype(m.bind(std::forward<F>(f)))
  {
    return m.bind(std::forward<F>(f));
  }

};
//} // v0_1
} // functional
} // boost

#endif // BOOST_FUNCTIONAL_MONAD_HPP
