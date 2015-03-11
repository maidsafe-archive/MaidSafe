// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_ALGORITHMS_VALUE_OR_CALL_HPP
#define BOOST_EXPECTED_ALGORITHMS_VALUE_OR_CALL_HPP

#include <boost/expected/expected.hpp>
#include <utility>

namespace boost
{
namespace expected_alg
{

  template <class F>
  struct defer_t
  {
  private:
    F fct_;
  public:
    defer_t(F&& f) :
      fct_(std::move(f))
    {
    }
    template <class ...A>
    auto operator()(A...) -> decltype(fct_())
    { return fct_();}
  };

  template <class F>
  inline defer_t<decay_t<F>> defer(F&& f)
  {
    return defer_t<decay_t<F> >(std::forward<F>(f));
  }

  template <class T, class E, class F>
  BOOST_CONSTEXPR T value_or_call(expected<T, E> const& e, F&& f)
  {
    // We are sure that e.catch_error(just(std::forward<T>(v))) will be valid or a exception will be thrown
    // so the derefference is safe
    return * e.catch_error(defer(std::forward<F>(f)));
  }

  template <class T, class E, class F>
  BOOST_CONSTEXPR T value_or_call(expected<T, E> && e, F&& f)
  {
    // We are sure that e.catch_error(just(std::forward<T>(v))) will be valid or a exception will be thrown
    // so the derefference is safe
    return * e.catch_error(defer(std::forward<F>(f)));
  }

} // namespace expected_alg
} // namespace boost

#endif // BOOST_EXPECTED_ALGORITHMS_VALUE_OR_CALL_HPP
