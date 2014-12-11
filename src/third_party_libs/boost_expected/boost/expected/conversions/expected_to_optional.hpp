// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_EXPECTED_TO_OPTIONAL_HPP
#define BOOST_EXPECTED_EXPECTED_TO_OPTIONAL_HPP

#include <boost/expected/expected.hpp>
#include <boost/optional.hpp>
//#include <optional>

namespace boost
{
  struct conversion_from_nullopt {};

  template <class T>
  expected<T> make_expected(optional<T> v) {
    if (v) return make_expected(*v);
    else make_unexpected(conversion_from_nullopt());
  }

  template <class T>
  optional<T> make_optional(expected<T> e) {
    if (e.valid()) return optional<T>(*e);
    else return none;
  }

} // namespace boost

#endif // BOOST_EXPECTED_UNEXPECTED_HPP
