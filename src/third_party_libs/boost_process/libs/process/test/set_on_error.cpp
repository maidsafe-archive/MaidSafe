// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MAIN
#define BOOST_TEST_IGNORE_SIGCHLD
#include <boost/test/included/unit_test.hpp>
#include <boost/process.hpp>
#include <boost/system/error_code.hpp>

namespace bp = boost::process;
namespace bpi = boost::process::initializers;

BOOST_AUTO_TEST_CASE(set_on_error_test)
{
    boost::system::error_code ec;
    bp::execute(
        bpi::run_exe("doesnt-exist"),
        bpi::set_on_error(ec)
    );
    BOOST_CHECK(ec);
}
