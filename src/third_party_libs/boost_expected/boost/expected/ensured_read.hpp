// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_ENSURED_READ_HPP
#define BOOST_EXPECTED_ENSURED_READ_HPP

#include <boost/expected/expected.hpp>
#include <boost/expected/unexpected.hpp>
#include <exception>
#include <utility>

namespace boost {

  template <class T>
  struct ensured_read {
    BOOST_CONSTEXPR ensured_read() : value_(), read_(false) {}
    BOOST_CONSTEXPR ensured_read(T const& v) : value_(v), read_(false) {}
    BOOST_CONSTEXPR ensured_read(T&& v) : value_(std::move(v)), read_(false) {}
      ensured_read(ensured_read const&x) = delete;
      ensured_read& operator=(ensured_read const&x) = delete;
      ensured_read(ensured_read && x)
        : value_(std::move(x.value_)),
          read_(x.read_) {
        x.read_ = true;
      }

      ensured_read& operator=(ensured_read&& x) {
        value_ = std::move(x.value_);
        read_= x.read_;
        x.read = true;
        return *this;
      }

      ~ensured_read() { if (! read_) std::terminate(); }

      operator T() const { read_=true; return std::move(value_); }
      operator T const&() const { read_=true; return value_; }
      operator T& () { read_=true; return value_; }

      //T value() const { read_=true; return std::move(value_); }
      T const& value() const { read_=true; return value_; }
      T & value() { read_=true; return value_; }
  private:
      T value_;
      mutable bool read_;
  };

  template <class T>
  ensured_read<decay_t<T>> make_ensured_read(T&& v)
  {
    return ensured_read<decay_t<T>>(std::forward<T>(v));
  }
  template <class E>
  BOOST_CONSTEXPR bool operator==(const ensured_read<E>& x, const ensured_read<E>& y)
  {
    return x.value() == y.value();
  }
  template <class E>
  BOOST_CONSTEXPR bool operator==(const ensured_read<E>& x, const E& y)
  {
    return x.value() == y;
  }

  ensured_read<std::exception_ptr> make_error_from_current_exception(ensured_read<std::exception_ptr>)
  {
    return make_ensured_read(std::current_exception());
  }

  template <class Error>
  ensured_read<std::exception_ptr> make_error(Error e, ensured_read<std::exception_ptr>)
  {
    return make_ensured_read(std::make_exception_ptr(e));
  }
  void rethrow(ensured_read<std::exception_ptr> e)
  {
    std::rethrow_exception(e.value());
  }

} // namespace boost

#endif // BOOST_EXPECTED_ERROR_EXCEPTION_HPP
