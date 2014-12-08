// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_FUNCTIONAL_TYPE_TRAITS_T_HPP
#define BOOST_FUNCTIONAL_TYPE_TRAITS_T_HPP


#include <type_traits>

namespace boost
{
  template <class M>
  using decay_t = typename std::decay<M>::type;

  template <class S>
  using result_of_t = typename std::result_of<S>::type;

#if 0
    template< class... > using void_t = void;
#else
    template< class... > struct voider { using type = void; };
    template< class... T0toN > using void_t = typename voider<T0toN...>::type;
#endif

}

#endif // BOOST_FUNCTIONAL_TYPE_TRAITS_T_HPP
