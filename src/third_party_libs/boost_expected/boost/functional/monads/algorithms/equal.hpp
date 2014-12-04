// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_MONADS_ALGORITHMS_EQUAL_HPP
#define BOOST_EXPECTED_MONADS_ALGORITHMS_EQUAL_HPP

#include <boost/config.hpp>
#include <boost/functional/monads/valued.hpp>
#include <utility>

namespace boost
{
namespace functional
{
namespace valued
{
  template <typename PV
      , class = if_pvalued<decay_t<PV>>
      //, class = std::enable_if<is_equality_comparable<value_type<decay_t<PV>>::value>
  >
  BOOST_CONSTEXPR bool equal( const PV& x, const PV& y ) {
      using namespace valued;

      return has_value(x) != has_value(y)
         ? false
         : ( has_value(x) ? deref(x) == deref(y) : true );
  }

}
}
}

#endif // BOOST_EXPECTED_MONADS_ALGORITHMS_EQUAL_HPP
