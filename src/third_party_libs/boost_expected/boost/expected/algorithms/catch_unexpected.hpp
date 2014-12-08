// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_ALGORITHMS_CATCH_UNEXPECTED_HPP
#define BOOST_EXPECTED_ALGORITHMS_CATCH_UNEXPECTED_HPP

#include <boost/expected/expected.hpp>

namespace boost
{
namespace expected_alg
{

  template <class Ex, class T, class F>
  expected<T> catch_unexpected(expected<T> const& e, F&& f)
  {
    try {
      if(! e) std::rethrow_exception(e.error());
      return e;
    }
    catch(Ex& e)
    {
      return expected<std::exception_ptr,T>(f(e));
    }
    catch (...)
    {
      return e;
    }
  }
  template <class Ex, class T, class F>
  expected<boost::exception_ptr, T> catch_unexpected(expected<T, boost::exception_ptr> const& e, F&& f)
  {
    try {
      if(! e) boost::rethrow_exception(e.error());
      return e;
    }
    catch(Ex& e)
    {
      return expected<boost::exception_ptr,T>(f(e));
    }
    catch (...)
    {
      return e;
    }
  }

} // namespace expected_alg
} // namespace boost

#endif // BOOST_EXPECTED_ALGORITHMS_CATCH_UNEXPECTED_HPP
