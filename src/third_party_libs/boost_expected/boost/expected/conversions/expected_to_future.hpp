// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_EXPECTED_TO_FUTURE_HPP
#define BOOST_EXPECTED_EXPECTED_TO_FUTURE_HPP

#include <boost/config.hpp>
#include <boost/expected/expected.hpp>
//#include <boost/thread/future.hpp>
#include <future>

namespace boost
{
  template <class T>
  expected<T> make_expected(std::future<T>&& f) {
    //assert (f.ready() && "future not ready");
    try {
      return f.get();
    } catch (...) {
      return make_unexpected_from_current_exception();
    }
  }

  template <class T, class E>
  std::future<T> make_unexpected_future(E e)  {
    std::promise<T> p;
    std::future<T> f = p.get_future();
    p.set_exception(std::make_exception_ptr(e));
    return std::move(f);
  }

  template <class T>
  std::future<T> make_ready_future(expected<T> e) {
    if (e.valid()) return make_ready_future(*e);
    else return make_unexpected_future<T>(e.error());
  }



} // namespace boost

#endif // BOOST_EXPECTED_UNEXPECTED_HPP
