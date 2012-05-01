//  Unit test for boost::lexical_cast.
//
//  See http://www.boost.org for most recent version, including documentation.
//
//  Copyright Antony Polukhin, 2011.
//
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#include <boost/config.hpp>

#if defined(__INTEL_COMPILER)
#pragma warning(disable: 193 383 488 981 1418 1419)
#elif defined(BOOST_MSVC)
#pragma warning(disable: 4097 4100 4121 4127 4146 4244 4245 4511 4512 4701 4800)
#endif

#include <boost/lexical_cast.hpp>

#include <boost/cstdint.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

using namespace boost;

void test_char_types_conversions()
{
#ifndef BOOST_LCAST_NO_WCHAR_T
    const char c_arr[]            = "Test array of chars";
    const unsigned char uc_arr[]  = "Test array of chars";
    const signed char sc_arr[]    = "Test array of chars";
    const wchar_t wc_arr[]        =L"Test array of chars";

    // Following tests depend on realization of std::locale
    // and pass for popular compilers and STL realizations
    BOOST_CHECK(boost::lexical_cast<wchar_t>(c_arr[0]) == wc_arr[0]);
    BOOST_CHECK(boost::lexical_cast<std::wstring>(c_arr) == std::wstring(wc_arr));

    BOOST_CHECK(boost::lexical_cast<std::wstring>(sc_arr) == std::wstring(wc_arr) );
    BOOST_CHECK(boost::lexical_cast<std::wstring>(uc_arr) == std::wstring(wc_arr) );

    BOOST_CHECK_EQUAL(boost::lexical_cast<wchar_t>(uc_arr[0]), wc_arr[0]);
    BOOST_CHECK_EQUAL(boost::lexical_cast<wchar_t>(sc_arr[0]), wc_arr[0]);
#endif
    BOOST_CHECK(1);
}

unit_test::test_suite *init_unit_test_suite(int, char *[])
{
    unit_test::test_suite *suite =
        BOOST_TEST_SUITE("lexical_cast char<->wchar_t unit test");
    suite->add(BOOST_TEST_CASE(&test_char_types_conversions));

    return suite;
}
