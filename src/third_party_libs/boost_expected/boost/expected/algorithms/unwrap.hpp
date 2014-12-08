// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_ALGORITHMS_UNWRAP_HPP
#define BOOST_EXPECTED_ALGORITHMS_UNWRAP_HPP

#include <boost/expected/expected.hpp>

//decay_t

namespace boost
{
namespace expected_alg
{

  // Factories
  // unwrap and if_then_else factories could be added as member functions
  template <class T, class E>
  expected<T, E> unwrap(expected<expected<T, E>, E > ee) {
    if (ee) return *ee;
    return ee.get_unexpected();
  }
  template <class T, class E>
  expected<T, E> unwrap(expected<T, E> e) {
    return e;
  }

} // namespace expected_alg
} // namespace boost

#endif // BOOST_EXPECTED_ALGORITHMS_UNWRAP_HPP
