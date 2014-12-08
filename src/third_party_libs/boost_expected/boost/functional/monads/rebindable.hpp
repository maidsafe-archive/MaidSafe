// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_FUNCTIONAL_REBINDABLE_HPP
#define BOOST_FUNCTIONAL_REBINDABLE_HPP

#include <boost/functional/type_traits_t.hpp>
#include <boost/functional/monads/categories/forward.hpp>
#include <boost/functional/monads/categories/default.hpp>
#include <boost/functional/meta.hpp>
#include <utility>
#include <type_traits>

namespace boost
{
namespace functional
{
  namespace category
  {
    struct rebindable {};
  }

//  template <bool condition>
//  struct when;
//  template <typename ...>
//  constexpr bool is_valid = true;

//  namespace core_detail {
//      constexpr auto is_a_impl(std::true_type*)      { return std::true_type; }
//      constexpr auto is_a_impl(std::false_type*) { return std::false_type; }
//  }
//
//  template <typename TC, typename ...DT>
//  constexpr auto _has_instance = core_detail::is_a_impl(
//      (typename TC::template instance<DT...>*)0
//  );

//  template <class TC, class ...DT>
//  struct has_instance :
//
//  template <class TC, class ...DT>
//  struct is_a :
//
//
//  namespace type_class
//  {
//    struct rebindable {
//      template <class T, class Void=void>
//      struct instance<T, when<true> >  {};
//      template <class T, bool Cond>
//      struct instance<T, when<Cond>> : std::false_type {};
//
//
//    };
//  }
//  template <class DT>
//  struct is_a<type_class::rebindable, DT> : has_instance<type_class::rebindable, DT> {};

  template <class T>
  struct rebindable_traits : std::false_type {};

  template <class M>
  struct is_rebindable : rebindable_traits<M> {};

  template <class M> using if_rebindable =
      typename std::enable_if<is_rebindable<M>::value, rebindable_traits<M> >::type;

namespace rebindable
{
  template <class M, class Traits = if_rebindable<M>>
  using value_type = typename Traits::template value_type<M>;

  template <class M, class Traits = if_rebindable<M> >
  using type_constructor = typename Traits::template type_constructor<M>;

  // Using a template alias here causes an ICE in VS2013 and VS14 CTP 3
  // so back to the old fashioned way
  template <class M, class U, class Traits = if_rebindable<M> >
  struct rebind
  {
    typedef typename Traits::template rebind<M, U>::type type;
  };
}

template <>
struct rebindable_traits<category::default_>  : std::true_type {
  // Using a template alias here causes an ICE in VS2013 and VS14 CTP 3
  // so back to the old fashioned way
  template <class M, class U, class Traits = if_rebindable<M> >
  struct rebind
  {
    typedef apply<rebindable::type_constructor<M>, U> type;
  };

};
template <>
struct rebindable_traits<category::forward>  : rebindable_traits<category::default_>
{
  template <class M>
  using value_type = typename M::value_type;

  template <class M>
  using type_constructor = typename M::type_constructor;
};

}
}

#endif // BOOST_FUNCTIONAL_VALUED_HPP
