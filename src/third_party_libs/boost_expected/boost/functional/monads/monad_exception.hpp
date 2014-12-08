// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_FUNCTIONAL_MONAD_EXCEPTION_HPP
#define BOOST_FUNCTIONAL_MONAD_EXCEPTION_HPP

#include <boost/functional/type_traits_t.hpp>
#include <boost/functional/meta.hpp>
#include <boost/functional/monads/monad.hpp>
#include <boost/expected/unexpected.hpp>
#include <boost/functional/monads/categories/forward.hpp>
#include <boost/functional/monads/categories/default.hpp>
#include <boost/functional/monads/monad_error.hpp>
#include <utility>
#include <type_traits>
#include <functional>

namespace boost
{
namespace functional
{

  template <class Mo>
  struct monad_exception_traits : std::false_type {};
  template <>
  struct monad_exception_traits<category::default_> : std::true_type {};
  template <>
  struct monad_exception_traits<category::forward> : monad_exception_traits<category::default_>
  {
    template <class M, class E>
    static auto make_exception(E&& e) -> decltype(make_exception(std::forward<E>(e)))
    {
      return make_exception(std::forward<E>(e));
    }

    template <class E, class M>
    static bool has_exception(M&& m)
    {
      return m.template has_exception<E>();
    }

    template <class E, class M, class F>
    static M catch_exception(M&& m, F&& f)
    {
      return m.template catch_exception<E>(std::forward<F>(f));
    }
  };

  template <class M>
  struct is_monad_exception : std::integral_constant<bool, is_monad_error<M>::value &&
    monad_exception_traits<M>::value
  >
  {};
  template <class M> using if_monad_exception =
      typename std::enable_if<is_monad_exception<M>::value, monad_exception_traits<M> >::type;

namespace monad_exception
{
  using namespace ::boost::functional::monad_error;

  // todo shouldn' we use if_monad_exception<M> or something like that here?
  template <class M, class E, class Traits = monad_exception_traits<M> >
  auto make_exception(E&& e) -> decltype(Traits::template make_exception<M>(std::forward<E>(e)))
  {
    return Traits::template make_exception<M>(std::forward<E>(e));
  }

  // todo shouldn' we use if_monad_exception<M> or something like that here?
  template <template <class ...> class M, class E, class Traits = monad_exception_traits<lift<M>> >
  auto make_exception(E&& e) -> decltype(Traits::template make_exception<M>(std::forward<E>(e)))
  {
    return Traits::template make_error<M>(std::forward<E>(e));
  }

  template <class E, class M, class Traits = if_monad_exception<decay_t<M>> >
  static bool has_exception(M&& m)
  {
    return Traits::template has_exception<E>(std::forward<M>(m));
  }
  template <class E, class M, class F, class Traits = if_monad_exception<decay_t<M>> >
  static M catch_exception(M&& m, F&& f)
  {
    return Traits::template catch_exception<E>(std::forward<M>(m), std::forward<F>(f));
  }

  template <class M, class E, class = if_monad_exception<decay_t<M>>>
  auto operator||(M&& m, M(*f)(E&))
  -> decltype(catch_exception<E>(std::forward<M>(m), f))
  {
    return catch_exception<E>(std::forward<M>(m), f);
  }
  template <class M, class E, class = if_monad_exception<decay_t<M>>>
  auto operator||(M&& m, std::function<M(E&)>&& f)
  -> decltype(catch_exception<E>(std::forward<M>(m), std::forward<std::function<M(E&)>>(f)))
  {
    return catch_exception<E>(std::forward<M>(m), std::forward<std::function<M(E&)>>(f));
  }
}
}
}

#endif // BOOST_FUNCTIONAL_MONAD_EXCEPTION_HPP
