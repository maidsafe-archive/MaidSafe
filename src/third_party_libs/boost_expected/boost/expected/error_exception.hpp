// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_ERROR_EXCEPTION_HPP
#define BOOST_EXPECTED_ERROR_EXCEPTION_HPP

#include <boost/expected/expected.hpp>

namespace boost {

template <class Error, class Exception>
struct error_exception {
    typedef Error error_type;
    typedef Exception exception_type;
    error_exception() : value_(){}
    error_exception(Error e) : value_(e){}
    error_exception& operator=(error_exception const&e) = default;
    error_exception& operator=(Error e) { value_= e; return *this;}
    operator Error() const { return value_; }
    Error value() const { return value_; }
private:
    Error value_;
};

template <class E, class X>
BOOST_CONSTEXPR bool operator==(const error_exception<E,X>& x, const error_exception<E,X>& y)
{
  return x.value() == y.value();
}
template <class E, class X>
BOOST_CONSTEXPR bool operator!=(const error_exception<E,X>& x, const error_exception<E,X>& y)
{
  return !(x == y);
}

template <class E, class X>
BOOST_CONSTEXPR bool operator<(const error_exception<E,X>& x, const error_exception<E,X>& y)
{
  return x.value() < y.value();
}

template <class E, class X>
BOOST_CONSTEXPR bool operator>(const error_exception<E,X>& x, const error_exception<E,X>& y)
{
  return (y < x);
}

template <class E, class X>
BOOST_CONSTEXPR bool operator<=(const error_exception<E,X>& x, const error_exception<E,X>& y)
{
  return !(y < x);
}

template <class E, class X>
BOOST_CONSTEXPR bool operator>=(const error_exception<E,X>& x, const error_exception<E,X>& y)
{
  return !(x < y);
}

template <class Error, class ErrorType, class Exception>
error_exception<ErrorType, Exception> make_error(Error e, error_exception<ErrorType, Exception>)
{
  return error_exception<ErrorType, Exception>(e);
}
template <class ErrorType, class Exception>
void rethrow(error_exception<ErrorType, Exception> e)
{
  throw Exception(e);
}
template <class ErrorType, class Exception>
error_exception<ErrorType, Exception> make_error_from_current_exception(error_exception<ErrorType, Exception>)
{
  return error_exception<ErrorType, Exception>();
}


} // namespace boost

#endif // BOOST_EXPECTED_ERROR_EXCEPTION_HPP
