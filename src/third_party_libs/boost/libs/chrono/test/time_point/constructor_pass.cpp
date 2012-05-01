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

#include <boost/chrono/chrono.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <libs/chrono/test/rep.h>

int main()
{
    {
        typedef boost::chrono::system_clock Clock;
        typedef boost::chrono::microseconds Duration1;
        typedef boost::chrono::milliseconds Duration2;
        boost::chrono::time_point<Clock, Duration2> t2(Duration2(3));
        boost::chrono::time_point<Clock, Duration1> t1 = t2;
        BOOST_TEST(t1.time_since_epoch() == Duration1(3000));
    }
    {
        typedef boost::chrono::system_clock Clock;
        typedef boost::chrono::duration<Rep, boost::milli> Duration;
        boost::chrono::time_point<Clock, Duration> t;
        BOOST_TEST(t.time_since_epoch() == Duration::zero());
    }
    {
        typedef boost::chrono::system_clock Clock;
        typedef boost::chrono::milliseconds Duration;
        boost::chrono::time_point<Clock, Duration> t(Duration(3));
        BOOST_TEST(t.time_since_epoch() == Duration(3));
    }
    {
        typedef boost::chrono::system_clock Clock;
        typedef boost::chrono::milliseconds Duration;
        boost::chrono::time_point<Clock, Duration> t(boost::chrono::seconds(3));
        BOOST_TEST(t.time_since_epoch() == Duration(3000));
    }


    return boost::report_errors();
}
