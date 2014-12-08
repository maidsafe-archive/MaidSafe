// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_MONADS_CATEGORIES_VALUED_AND_ERRORED_HPP
#define BOOST_EXPECTED_MONADS_CATEGORIES_VALUED_AND_ERRORED_HPP

#include <boost/config.hpp>
#include <boost/functional/monads/errored.hpp>
#include <boost/functional/monads/functor.hpp>
#include <boost/functional/monads/monad.hpp>
#include <boost/functional/monads/algorithms/have_value.hpp>
#include <boost/functional/monads/algorithms/first_unexpected.hpp>
#include <boost/functional/meta.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <type_traits>

#define REQUIRES(...) typename ::boost::enable_if_c<__VA_ARGS__, void*>::type = 0
#define T_REQUIRES(...) typename = typename ::boost::enable_if_c<(__VA_ARGS__)>::type

namespace boost
{
namespace functional
{
  namespace category
  {
    struct errored {};
  }

  template <>
  struct functor_traits<category::errored> : functor_traits<category::default_>
  {

    template <class F, class M0, class ...M,
    class FR = decltype(std::declval<F>()(errored::deref(std::declval<M0>()), errored::deref(std::declval<M>())...))>
      static BOOST_CONSTEXPR auto map(F&& f, M0&& m0, M&& ...m)
#if !defined BOOST_MSVC || BOOST_MSVC >= 1900
      -> typename errored::rebind<decay_t<M0>, FR>::type
#else
      // VS2013 doesn't cope well with many nested indirections of type specialisations
      -> apply<rebindable::type_constructor<decay_t<M0>>, FR>
#endif
    {
      using namespace errored;
      typedef typename rebind<decay_t<M0>, FR>::type result_type;
      return have_value(std::forward<M0>(m0), std::forward<M>(m)...)
        ? result_type(std::forward<F>(f)(deref(std::forward<M0>(m0)), deref(std::forward<M>(m))...))
        : first_unexpected(std::forward<M0>(m0), std::forward<M>(m)...)
        ;
    }
  };

  template <>
  struct monad_traits<category::errored> : monad_traits<category::forward>
  {
    template <class M, class F, class FR = decltype( std::declval<F>()( errored::deref(std::declval<M>()) ) )>
    static BOOST_CONSTEXPR auto
    bind(M&& m, F&& f,
        REQUIRES(std::is_same<FR, void>::value)
    ) -> typename errored::rebind<decay_t<M>, FR>::type
    {
      using namespace errored;
      typedef typename rebind<decay_t<M>, FR>::type result_type;
#if ! defined BOOST_NO_CXX14_CONSTEXPR
      if(has_value(m))
      {
        f(deref(m));
        return result_type();
      }
      return get_errored(m);
#else
      return (has_value(m)
          ? (f(deref(m)), result_type() )
          : result_type( get_errored(m) )
      );
#endif
    }

    template <class M, class F, class FR = decltype( std::declval<F>()( errored::deref(std::declval<M>()) ) )>
    static BOOST_CONSTEXPR auto
    bind(M&& m, F&& f,
        REQUIRES((! std::is_same<FR, void>::value
                && ! boost::functional::is_monad<FR>::value)
        )) -> typename errored::rebind<decay_t<M>, FR>::type
    {
      using namespace errored;
      typedef typename rebind<decay_t<M>, FR>::type result_type;
#if ! defined BOOST_NO_CXX14_CONSTEXPR
      if(has_value(m))
      {
        return result_type(f(deref(m)));
      }
      return get_errored(m);
#else
      return (has_value(m)
          ? result_type(f(deref(m)))
          : result_type( get_errored(m) )
      );
#endif
    }

    template <class M, class F, class FR = decltype( std::declval<F>()( errored::deref(std::declval<M>()) ) )>
    static BOOST_CONSTEXPR auto
    bind(M&& m, F&& f,
        REQUIRES( boost::functional::is_monad<FR>::value )
    ) -> FR
    {
      using namespace errored;
#if ! defined BOOST_NO_CXX14_CONSTEXPR
      if(has_value(m))
      {
        return f(deref(m));
      }
      return get_errored(m);
#else
      return (has_value(m)
          ? f(deref(m))
          : FR( get_errored(m) )
      );
#endif
    }
  };

}
}

#undef REQUIRES
#undef T_REQUIRES
#endif // BOOST_EXPECTED_MONADS_CATEGORIES_VALUED_AND_ERRORED_HPP
