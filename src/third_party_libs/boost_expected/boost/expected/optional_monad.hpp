// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_OPTIONAL_MONAD_HPP
#define BOOST_EXPECTED_OPTIONAL_MONAD_HPP


#include <boost/functional/meta.hpp>
#include <boost/optional.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/functional/monads/errored.hpp>
#include <boost/functional/monads/functor.hpp>
#include <boost/functional/monads/categories/errored.hpp>
#include <boost/functional/monads/categories/pointer_like.hpp>
#include <boost/functional/monads/monad.hpp>
#include <boost/functional/monads/monad_error.hpp>
#include <type_traits>

namespace boost
{
  using optional_monad = functional::lift<optional>;

namespace functional
{
  template <class T>
  struct rebindable_traits<optional<T>> : rebindable_traits<category::default_>
  {
    template <class M>
    using value_type = typename M::value_type;

    template <class M>
    struct type_constructor {
      template <class U>
      using type = optional<U>;
    };

  };

  template <class T>
  struct valued_traits<optional<T>> : valued_traits<category::pointer_like>
  {
    template <class M>
    static BOOST_CONSTEXPR auto get_value(M&& m) -> decltype(m.value())
    { return m.value(); }
  };

  template <class T>
  struct errored_traits<optional<T> > : errored_traits<category::default_>
  {
    template< class M >
    using error_type = none_t;
    template< class M >
    using errored_type = none_t;

    template <class M, class E>
    static M make_error(E&&)
    { return none; }

    static BOOST_CONSTEXPR none_t get_errored(optional<T> const& )
    { return none; }

    template< class M >
    static BOOST_CONSTEXPR none_t error(M && m)
    { return none; }
  };

  template <class T>
  struct functor_traits<optional<T> > : functor_traits<category::errored> {};

  template <class T>
  struct monad_traits<optional<T> > : monad_traits<category::errored> {};

  template <>
  struct monad_error_traits<optional_monad > : monad_error_traits<category::default_>
  {
    template <class M, class E>
    static auto make_error(E&&) -> decltype(none)
    {
      return none;
    }

    // f : E -> T
    // todo complete with the other prototypes
    // f : E -> void
    // f : E -> M
    template <class M, class F, class FR = decltype( std::declval<F>()( none ) ) >
    static BOOST_CONSTEXPR M
    catch_error(M&& m, F&& f)
    {
      using namespace ::boost::functional::errored;

      typedef typename rebind<decay_t<M>, FR>::type result_type;
#if ! defined BOOST_NO_CXX14_CONSTEXPR
      if(! has_value(m))
      {
        result_type(f(none));
      }
      return deref(m);
#else
      return (! has_value(m)
          ? result_type(f(none))
          : deref(m)
      );
#endif
    }
  };

  template <class T>
  struct monad_error_traits<optional<T> > : monad_error_traits<optional_monad > {};

}
}

#endif // BOOST_EXPECTED_OPTIONAL_MONAD_HPP
