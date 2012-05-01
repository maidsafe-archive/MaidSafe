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

// test ratio_add

#include <boost/ratio/ratio_io.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <climits>

int main()
{

    {
        BOOST_TEST((
                boost::ratio_string<boost::atto, char>::long_name() == "atto"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::atto, char>::short_name() == "a"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::femto, char>::long_name() == "femto"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::femto, char>::short_name() == "f"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::pico, char>::long_name() == "pico"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::pico, char>::short_name() == "p"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::nano, char>::long_name() == "nano"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::nano, char>::short_name() == "n"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::micro, char>::long_name() == "micro"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::micro, char>::short_name() == "\xC2\xB5"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::milli, char>::long_name() == "milli"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::milli, char>::short_name() == "m"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::centi, char>::long_name() == "centi"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::centi, char>::short_name() == "c"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::deci, char>::long_name() == "deci"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::deci, char>::short_name() == "d"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::deca, char>::long_name() == "deca"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::deca, char>::short_name() == "da"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::hecto, char>::long_name() == "hecto"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::hecto, char>::short_name() == "h"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::kilo, char>::long_name() == "kilo"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::kilo, char>::short_name() == "k"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::mega, char>::long_name() == "mega"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::mega, char>::short_name() == "M"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::giga, char>::long_name() == "giga"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::giga, char>::short_name() == "G"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::tera, char>::long_name() == "tera"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::tera, char>::short_name() == "T"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::peta, char>::long_name() == "peta"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::peta, char>::short_name() == "P"
        ));
    }
    {
        BOOST_TEST((
                boost::ratio_string<boost::exa, char>::long_name() == "exa"
        ));
        BOOST_TEST((
                boost::ratio_string<boost::exa, char>::short_name() == "E"
        ));
    }
//    return 1;
    return boost::report_errors();
}


