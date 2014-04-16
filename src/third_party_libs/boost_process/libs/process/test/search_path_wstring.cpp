// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define _UNICODE
#define UNICODE
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/process.hpp>
#include <string>

namespace bp = boost::process;

BOOST_AUTO_TEST_CASE(search)
{
    std::wstring filename = L"cmd";
    BOOST_CHECK(!bp::search_path(filename).empty());
    std::wstring path = L"C:\\Windows;C:\\Windows\\System32";
    BOOST_CHECK(!bp::search_path(filename, path).empty());
}
