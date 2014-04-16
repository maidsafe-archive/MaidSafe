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
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#if defined(BOOST_WINDOWS_API)
#   include <Windows.h>
#elif defined(BOOST_POSIX_API)
#   include <errno.h>
#endif

namespace bp = boost::process;
namespace bpi = boost::process::initializers;
namespace bio = boost::iostreams;

void run_exe(const std::string &exe, bp::executor &e)
{
    e.exe = exe.c_str();
}

void set_on_error(boost::system::error_code &ec, bp::executor&)
{
    using namespace boost::system;
#if defined(BOOST_WINDOWS_API)
    ec = error_code(::GetLastError(), system_category());
#elif defined(BOOST_POSIX_API)
    ec = error_code(errno, system_category());
#endif
}

BOOST_AUTO_TEST_CASE(extensions)
{
    using boost::unit_test::framework::master_test_suite;

    std::string exe = master_test_suite().argv[1];

    boost::system::error_code ec;
    bp::execute(
#if defined(BOOST_WINDOWS_API)
        bpi::on_CreateProcess_setup(
            boost::bind(run_exe, exe, _1)),
        bpi::set_cmd_line("test"),
        bpi::on_CreateProcess_error(
            boost::bind(set_on_error, boost::ref(ec), _1))
#elif defined(BOOST_POSIX_API)
        bpi::on_exec_setup(
            boost::bind(run_exe, exe, _1)),
        bpi::set_cmd_line("test"),
        bpi::on_exec_error(
            boost::bind(set_on_error, boost::ref(ec), _1))
#endif
    );
    BOOST_CHECK(!ec);
}
