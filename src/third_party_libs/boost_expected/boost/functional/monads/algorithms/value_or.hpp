// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_MONADS_ALGORITHMS_VALUE_OR_HPP
#define BOOST_EXPECTED_MONADS_ALGORITHMS_VALUE_OR_HPP

#include <boost/functional/monads/valued.hpp>

#include <utility>

namespace boost {
namespace functional {
namespace valued {

#if defined __clang
  template <class PV, class U, class = if_pvalued<decay_t<PV>> >
  BOOST_CONSTEXPR value_type<PV> value_or(PV const& e, U&& v)
  {
    return has_value(e)
      ? deref(e)
      : static_cast<value_type<PV>>(std::forward<U>(v));
  }

  template <class PV, class U, class = if_pvalued<decay_t<PV>> >
  value_type<PV> value_or(PV && e, U&& v)
  {
    return has_value(e)
      ? std::move(deref(std::forward<PV>(e)))
      : static_cast<value_type<PV>>(std::forward<U>(v));
  }
#else
  template <class PV, class U, class = if_pvalued<decay_t<PV>> >
  BOOST_CONSTEXPR U value_or(PV const& e, U&& v)
  {
    return has_value(e)
      ? deref(e)
      : static_cast<value_type<PV>>(std::forward<U>(v));
  }

  template <class PV, class U, class = if_pvalued<decay_t<PV>> >
  U value_or(PV && e, U&& v)
  {
    return has_value(e)
      ? std::move(deref(std::forward<PV>(e)))
      : static_cast<value_type<PV>>(std::forward<U>(v));
  }
#endif
} // namespace valued
} // namespace functional
} // namespace boost

#endif // BOOST_EXPECTED_HPP
