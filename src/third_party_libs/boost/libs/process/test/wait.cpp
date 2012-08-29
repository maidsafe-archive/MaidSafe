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
#include <boost/asio.hpp>
#if defined(BOOST_POSIX_API)
#   include <signal.h>
#endif

namespace bp = boost::process;
namespace bpi = boost::process::initializers;

BOOST_AUTO_TEST_CASE(sync_wait)
{
    using boost::unit_test::framework::master_test_suite;

    boost::system::error_code ec;
    bp::child c = bp::execute(
        bpi::run_exe(master_test_suite().argv[1]),
        bpi::set_cmd_line("test --wait 1"),
        bpi::set_on_error(ec)
    );
    BOOST_REQUIRE(!ec);

    bp::wait_for_exit(c);
}

struct wait_handler
{
#if defined(BOOST_WINDOWS_API)
    void operator()(const boost::system::error_code&) {}
#elif defined(BOOST_POSIX_API)
    void operator()(const boost::system::error_code&, int) {}
#endif
};

BOOST_AUTO_TEST_CASE(async_wait)
{
    using boost::unit_test::framework::master_test_suite;
    using namespace boost::asio;

    boost::system::error_code ec;
    bp::child c = bp::execute(
        bpi::run_exe(master_test_suite().argv[1]),
        bpi::set_cmd_line("test --wait 1"),
        bpi::set_on_error(ec)
    );
    BOOST_REQUIRE(!ec);

    boost::asio::io_service io_service;

#if defined(BOOST_WINDOWS_API)
    windows::object_handle handle(io_service, c.process_handle());
    handle.async_wait(wait_handler());
#elif defined(BOOST_POSIX_API)
    signal_set set(io_service, SIGCHLD);
    set.async_wait(wait_handler());
#endif

    io_service.run();
}
