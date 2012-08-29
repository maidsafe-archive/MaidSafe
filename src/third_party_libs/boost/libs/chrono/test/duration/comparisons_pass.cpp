//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//  Adaptation to Boost of the libcxx
//  Copyright 2010 Vicente J. Botet Escriba
//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/chrono/duration.hpp>
#include <boost/detail/lightweight_test.hpp>

#define BOOST_CHRONO_TEST(C) if (true) {BOOST_CONSTEXPR bool B = (C); BOOST_TEST(B);} else

int main()
{
    {
      BOOST_CONSTEXPR boost::chrono::seconds s1(3);
      BOOST_CONSTEXPR boost::chrono::seconds s2(3);
    BOOST_CONSTEXPR bool b1 = (s1 == s2);
    BOOST_TEST(s1 == s2);
    BOOST_TEST(!(s1 != s2));
    }
    {
      BOOST_CONSTEXPR boost::chrono::seconds s1(3);
      BOOST_CONSTEXPR boost::chrono::seconds s2(4);
    BOOST_TEST(!(s1 == s2));
    BOOST_TEST(s1 != s2);
    }
    {
      BOOST_CONSTEXPR boost::chrono::milliseconds s1(3);
      BOOST_CONSTEXPR boost::chrono::microseconds s2(3000);
    BOOST_TEST(s1 == s2);
    BOOST_TEST(!(s1 != s2));
    }
    {
      BOOST_CONSTEXPR boost::chrono::milliseconds s1(3);
      BOOST_CONSTEXPR boost::chrono::microseconds s2(4000);
    BOOST_TEST(!(s1 == s2));
    BOOST_TEST(s1 != s2);
    }
    {
      BOOST_CONSTEXPR boost::chrono::duration<int, boost::ratio<2, 3> > s1(9);
      BOOST_CONSTEXPR boost::chrono::duration<int, boost::ratio<3, 5> > s2(10);
    BOOST_TEST(s1 == s2);
    BOOST_TEST(!(s1 != s2));
    }
    {
      BOOST_CONSTEXPR boost::chrono::duration<int, boost::ratio<2, 3> > s1(10);
      BOOST_CONSTEXPR boost::chrono::duration<int, boost::ratio<3, 5> > s2(9);
    BOOST_TEST(!(s1 == s2));
    BOOST_TEST(s1 != s2);
    }
    {
      BOOST_CONSTEXPR boost::chrono::duration<int, boost::ratio<2, 3> > s1(9);
      BOOST_CONSTEXPR boost::chrono::duration<double, boost::ratio<3, 5> > s2(10);
    BOOST_TEST(s1 == s2);
    BOOST_TEST(!(s1 != s2));
    }
    {
      BOOST_CONSTEXPR boost::chrono::seconds s1(3);
      BOOST_CONSTEXPR boost::chrono::seconds s2(3);
    BOOST_TEST(!(s1 < s2));
    BOOST_TEST(!(s1 > s2));
    BOOST_TEST( (s1 <= s2));
    BOOST_TEST( (s1 >= s2));
    }
    {
      BOOST_CONSTEXPR boost::chrono::seconds s1(3);
      BOOST_CONSTEXPR boost::chrono::seconds s2(4);
    BOOST_TEST( (s1 < s2));
    BOOST_TEST(!(s1 > s2));
    BOOST_TEST( (s1 <= s2));
    BOOST_TEST(!(s1 >= s2));
    }
    {
      BOOST_CONSTEXPR boost::chrono::milliseconds s1(3);
      BOOST_CONSTEXPR boost::chrono::microseconds s2(3000);
    BOOST_TEST(!(s1 < s2));
    BOOST_TEST(!(s1 > s2));
    BOOST_TEST( (s1 <= s2));
    BOOST_TEST( (s1 >= s2));
    }
    {
      BOOST_CONSTEXPR boost::chrono::milliseconds s1(3);
      BOOST_CONSTEXPR boost::chrono::microseconds s2(4000);
    BOOST_TEST( (s1 < s2));
    BOOST_TEST(!(s1 > s2));
    BOOST_TEST( (s1 <= s2));
    BOOST_TEST(!(s1 >= s2));
    }
    {
      BOOST_CONSTEXPR boost::chrono::duration<int, boost::ratio<2, 3> > s1(9);
      BOOST_CONSTEXPR boost::chrono::duration<int, boost::ratio<3, 5> > s2(10);
    BOOST_TEST(!(s1 < s2));
    BOOST_TEST(!(s1 > s2));
    BOOST_TEST( (s1 <= s2));
    BOOST_TEST( (s1 >= s2));
    }
    {
      BOOST_CONSTEXPR boost::chrono::duration<int, boost::ratio<2, 3> > s1(10);
      BOOST_CONSTEXPR boost::chrono::duration<int, boost::ratio<3, 5> > s2(9);
    BOOST_TEST(!(s1 < s2));
    BOOST_TEST( (s1 > s2));
    BOOST_TEST(!(s1 <= s2));
    BOOST_TEST( (s1 >= s2));
    }
    {
      BOOST_CONSTEXPR boost::chrono::duration<int, boost::ratio<2, 3> > s1(9);
      BOOST_CONSTEXPR boost::chrono::duration<double, boost::ratio<3, 5> > s2(10);
    BOOST_TEST(!(s1 < s2));
    BOOST_TEST(!(s1 > s2));
    BOOST_TEST( (s1 <= s2));
    BOOST_TEST( (s1 >= s2));
    }
    return boost::report_errors();
}
