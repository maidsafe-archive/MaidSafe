 // Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_ALGORITHMS_HAS_ERROR_HPP
#define BOOST_EXPECTED_ALGORITHMS_HAS_ERROR_HPP

#include <boost/expected/expected.hpp>

namespace boost
{
namespace expected_alg
{

  template <class T, class E, class E1>
  BOOST_CONSTEXPR bool has_error(expected<T, E> const& e, E1 const& err)
  {
    return (e) ? false : (e.error() == err) ;
 }

} // namespace expected_alg
} // namespace boost

#endif // BOOST_EXPECTED_ALGORITHMS_HAS_ERROR_HPP
