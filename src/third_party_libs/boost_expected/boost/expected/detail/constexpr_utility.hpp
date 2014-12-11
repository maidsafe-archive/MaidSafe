// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2013,2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_DETAIL_CONSTEXPR_UTILITY_HPP
#define BOOST_EXPECTED_DETAIL_CONSTEXPR_UTILITY_HPP

#include <boost/expected/config.hpp>

#include <utility>
#include <type_traits>

namespace boost {

// workaround: std utility functions aren't constexpr yet
template <class T> inline
BOOST_CONSTEXPR T&& constexpr_forward(typename std::remove_reference<T>::type& t) BOOST_NOEXCEPT
{
  return static_cast<T&&>(t);
}

template <class T> inline
BOOST_CONSTEXPR T&& constexpr_forward(typename std::remove_reference<T>::type&& t) BOOST_NOEXCEPT
{
    static_assert(!std::is_lvalue_reference<T>::value, "!!");
    return static_cast<T&&>(t);
}

template <class T> inline
BOOST_CONSTEXPR typename std::remove_reference<T>::type&& constexpr_move(T&& t) BOOST_NOEXCEPT
{
    return static_cast<typename std::remove_reference<T>::type&&>(t);
}

template<class T> inline
BOOST_CONSTEXPR T * constexpr_addressof(T& Val)
{
  return ((T *) &(char&)Val);
}

} // namespace boost


#endif // BOOST_EXPECTED_DETAIL_CONSTEXPR_UTILITY_HPP
