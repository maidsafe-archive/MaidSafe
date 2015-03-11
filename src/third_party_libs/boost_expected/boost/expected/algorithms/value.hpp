// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_ALGORITHMS_VALUE_HPP
#define BOOST_EXPECTED_ALGORITHMS_VALUE_HPP

#include <boost/expected/expected.hpp>

//decay_t

namespace boost
{
namespace expected_alg
{

  template <class T>
  struct thrower
  {
    T operator()(std::exception_ptr e)
    {
      if (e==std::exception_ptr())
      throw expected_default_constructed();
      std::rethrow_exception(e);
    }
    T operator()(boost::exception_ptr e)
    {
      if (e==boost::exception_ptr())
      throw expected_default_constructed();
      boost::rethrow_exception(e);
    }
    template <class E>
    T operator()(E const& e)
    {
      throw bad_expected_access<E>(e);
    }
    template <class ErrorType, class Exception>
    T operator()(error_exception<ErrorType, Exception> const& e)
    {
      throw Exception(e.value);
    }
  };

  // free function value to get the value of an expected or throw.
  // Note that this function could not be equivalent to the e.value() as it creates a temporary and so
  // it can not return the address.
  template <class T, class E>
  BOOST_CONSTEXPR T value(expected<T, E> const& e)
  {
    // We are sure that e.catch_error(thrower<T>()) will be valid or a exception will be thrown
    // so the derefference is safe
    return * e.catch_error(thrower<T>());
  }

  template <class T, class E>
  BOOST_CONSTEXPR T value(expected<T, E> && e)
  {
    // We are sure that e.catch_error(thrower<T>()) will be valid or a exception will be thrown
    // so the derefference is safe
    return * e.catch_error(thrower<T>());
  }

} // namespace expected_alg
} // namespace boost

#endif // BOOST_EXPECTED_HPP
