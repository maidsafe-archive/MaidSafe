// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_FUNCTIONAL_MONAD_ERROR_HPP
#define BOOST_FUNCTIONAL_MONAD_ERROR_HPP

#include <boost/functional/type_traits_t.hpp>
#include <boost/functional/meta.hpp>
#include <boost/functional/monads/monad.hpp>
#include <boost/expected/unexpected.hpp>
#include <boost/functional/monads/categories/forward.hpp>
#include <boost/functional/monads/categories/default.hpp>
#include <utility>
#include <type_traits>

namespace boost
{
namespace functional
{
  template <class Mo>
  struct monad_error_traits : std::false_type {};
  template <>
  struct monad_error_traits<category::default_> : std::true_type {};
  template <>
  struct monad_error_traits<category::forward> : monad_error_traits<category::default_>
  {
    template <class M, class E>
    static auto make_error(E&& e) -> decltype(make_unexpected(std::forward<E>(e)))
    {
      return make_unexpected(std::forward<E>(e));
    }

    template <class M, class F>
    static M catch_error(M&& m, F&& f)
    {
      return m.catch_error(std::forward<F>(f));
    }
  };

  template <class M>
  struct is_monad_error : std::integral_constant<bool, is_monad<M>::value &&
    monad_error_traits<M>::value
  >
  {};

  template <class M> using if_monad_error =
      typename std::enable_if<is_monad_error<M>::value, monad_error_traits<M> >::type;

namespace monad_error
{
  using namespace ::boost::functional::monad;

  // todo shouldn' we use if_monad_error<M> or something like that here?
  template <class M, class E, class Traits = monad_error_traits<M> >
  auto make_error(E&& e) -> decltype(Traits::template make_error<M>(std::forward<E>(e)))
  {
    return Traits::template make_error<M>(std::forward<E>(e));
  }

  // todo shouldn' we use if_monad_error<lift<M>> or something like that here?
  template <template <class ...> class M, class E, class Traits = monad_error_traits<lift<M>> >
  auto make_error(E&& e) -> decltype(Traits::template make_error<M>(std::forward<E>(e)))
  {
    return Traits::template make_error<M>(std::forward<E>(e));
  }

  template <class M, class F, class Traits = if_monad_error<decay_t<M>> >
  static M catch_error(M&& m, F&& f)
  {
    return Traits::catch_error(std::forward<M>(m), std::forward<F>(f));
  }

  template <class M, class F, class = if_monad_error<decay_t<M>>>
  auto operator|(M&& m, F&& f)
  -> decltype(::boost::functional::monad_error::catch_error(std::forward<M>(m), std::forward<F>(f)))
  {
    return ::boost::functional::monad_error::catch_error(std::forward<M>(m), std::forward<F>(f));
  }
}
}
}

#endif // BOOST_FUNCTIONAL_MONAD_ERROR_HPP
