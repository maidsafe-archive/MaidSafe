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

#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>

int main()
{
#ifdef BOOST_MSVC
    BOOST_STATIC_ASSERT((boost::is_same<wchar_t, unsigned short>::value));
#endif

    return ::boost::lexical_cast<int>(L"1000") == 1000;
}


