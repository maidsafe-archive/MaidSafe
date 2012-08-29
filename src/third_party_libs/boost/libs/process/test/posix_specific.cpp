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
#include <boost/system/system_error.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <string>
#include <sys/wait.h>
#include <errno.h>

namespace bp = boost::process;
namespace bpi = boost::process::initializers;
namespace bio = boost::iostreams;

BOOST_AUTO_TEST_CASE(bind_fd)
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p = bp::create_pipe();

    {
        bio::file_descriptor_sink sink(p.sink, bio::close_handle);
        boost::system::error_code ec;
        bp::execute(
            bpi::run_exe(master_test_suite().argv[1]),
            bpi::set_cmd_line("test --posix-echo-one 3 hello"),
            bpi::bind_fd(3, sink),
            bpi::set_on_error(ec)
        );
        BOOST_CHECK(!ec);
    }

    bio::file_descriptor_source source(p.source, bio::close_handle);
    bio::stream<bio::file_descriptor_source> is(source);

    std::string s;
    is >> s;
    BOOST_CHECK_EQUAL(s, "hello");
}

BOOST_AUTO_TEST_CASE(bind_fds)
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p1 = bp::create_pipe();
    bp::pipe p2 = bp::create_pipe();

    {
        bio::file_descriptor_sink sink1(p1.sink, bio::close_handle);
        bio::file_descriptor_sink sink2(p2.sink, bio::close_handle);
        boost::system::error_code ec;
        bp::execute(
            bpi::run_exe(master_test_suite().argv[1]),
            bpi::set_cmd_line("test --posix-echo-two 3 hello 99 bye"),
            bpi::bind_fd(3, sink1),
            bpi::bind_fd(99, sink2),
            bpi::set_on_error(ec)
        );
        BOOST_CHECK(!ec);
    }

    bio::file_descriptor_source source1(p1.source, bio::close_handle);
    bio::stream<bio::file_descriptor_source> is1(source1);

    std::string s1;
    is1 >> s1;
    BOOST_CHECK_EQUAL(s1, "hello");

    bio::file_descriptor_source source2(p2.source, bio::close_handle);
    bio::stream<bio::file_descriptor_source> is2(source2);

    std::string s2;
    is2 >> s2;
    BOOST_CHECK_EQUAL(s2, "bye");
}

BOOST_AUTO_TEST_CASE(execve_set_on_error)
{
    boost::system::error_code ec;
    bp::execute(
        bpi::run_exe("doesnt-exist"),
        bpi::set_on_error(ec)
    );
    BOOST_CHECK(ec);
    BOOST_CHECK_EQUAL(ec.value(), ENOENT);
}

BOOST_AUTO_TEST_CASE(execve_throw_on_error)
{
    try
    {
        bp::execute(
            bpi::run_exe("doesnt-exist"),
            bpi::throw_on_error()
        );
    }
    catch (boost::system::system_error &e)
    {
        BOOST_CHECK(e.code());
        BOOST_CHECK_EQUAL(e.code().value(), ENOENT);
    }
}
