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

#define BOOST_CHRONO_EXTENSIONS
#include <boost/chrono/chrono.hpp>
#include <boost/detail/lightweight_test.hpp>

int main()
{    
  {
    typedef boost::chrono::system_clock Clock;
    typedef boost::chrono::milliseconds Duration;
    boost::chrono::time_point<Clock, Duration> t(Duration(3));
    t += 2;
    BOOST_TEST(t.time_since_epoch() == Duration(5));
  }
  {
    typedef boost::chrono::system_clock Clock;
    typedef boost::chrono::milliseconds Duration;
    boost::chrono::time_point<Clock, Duration> t(Duration(3));
    t++;
    BOOST_TEST(t.time_since_epoch() == Duration(4));
  }
  {
    typedef boost::chrono::system_clock Clock;
    typedef boost::chrono::milliseconds Duration;
    boost::chrono::time_point<Clock, Duration> t(Duration(3));
    ++t;
    BOOST_TEST(t.time_since_epoch() == Duration(4));
  }
  {
    typedef boost::chrono::system_clock Clock;
    typedef boost::chrono::milliseconds Duration;
    boost::chrono::time_point<Clock, Duration> t(Duration(3));
    t -= 2;
    BOOST_TEST(t.time_since_epoch() == Duration(1));
  }
  {
    typedef boost::chrono::system_clock Clock;
    typedef boost::chrono::milliseconds Duration;
    boost::chrono::time_point<Clock, Duration> t(Duration(3));
    t--;
    BOOST_TEST(t.time_since_epoch() == Duration(2));
  }
  {
    typedef boost::chrono::system_clock Clock;
    typedef boost::chrono::milliseconds Duration;
    boost::chrono::time_point<Clock, Duration> t(Duration(3));
    --t;
    BOOST_TEST(t.time_since_epoch() == Duration(2));
  }
#if 0
  {
    typedef boost::chrono::system_clock Clock;
    typedef boost::chrono::milliseconds Duration1;
    typedef boost::chrono::microseconds Duration2;
    boost::chrono::time_point<Clock, Duration1> t1(Duration1(3));
    boost::chrono::time_point<Clock, Duration2> t2 = t1 - Duration2(5);
    BOOST_TEST(t2.time_since_epoch() == Duration2(2995));
  }
  {
    typedef boost::chrono::system_clock Clock;
    typedef boost::chrono::milliseconds Duration1;
    typedef boost::chrono::microseconds Duration2;
    boost::chrono::time_point<Clock, Duration1> t1(Duration1(3));
    boost::chrono::time_point<Clock, Duration2> t2(Duration2(5));
    BOOST_TEST((t1 - t2) == Duration2(2995));
  }
  {
    typedef boost::chrono::system_clock Clock;
    typedef boost::chrono::milliseconds Duration1;
    typedef boost::chrono::microseconds Duration2;
    boost::chrono::time_point<Clock, Duration1> t1(Duration1(3));
    boost::chrono::time_point<Clock, Duration2> t2 = t1 + Duration2(5);
    BOOST_TEST(t2.time_since_epoch() == Duration2(3005));
    t2 = Duration2(6) + t1;
    BOOST_TEST(t2.time_since_epoch() == Duration2(3006));
  }  
#endif  
    
    return boost::report_errors();
}
