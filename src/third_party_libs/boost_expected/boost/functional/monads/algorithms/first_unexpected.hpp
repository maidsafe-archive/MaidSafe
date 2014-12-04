// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_MONADS_ALGORITHMS_FIRST_UNEXPECTED_HPP
#define BOOST_EXPECTED_MONADS_ALGORITHMS_FIRST_UNEXPECTED_HPP

#include <boost/functional/monads/errored.hpp>
#include <utility>

namespace boost
{
namespace functional
{
namespace errored
{

  template< class M, class = if_errored<decay_t<M>> >
  BOOST_CONSTEXPR errored_type<M> first_unexpected( M&& m )
  {
    return get_errored(std::forward<M>(m));
  }

  // todo: create a variadic if_errored
  template< class M1, class ...Ms
    , class = if_errored<decay_t<M1>>
    >
  BOOST_CONSTEXPR errored_type<M1> first_unexpected( M1&& m1, Ms&& ...ms )
  {
    return has_value(std::forward<M1>(m1))
        ? first_unexpected( std::forward<Ms>(ms)... )
            : get_errored(std::forward<M1>(m1)) ;
  }

}
}
}


#endif // BOOST_EXPECTED_MONADS_ALGORITHMS_FIRST_UNEXPECTED_HPP
