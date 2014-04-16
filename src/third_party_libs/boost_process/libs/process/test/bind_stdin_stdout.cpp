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
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <string>
#include <iostream>

namespace bp = boost::process;
namespace bpi = boost::process::initializers;
namespace bio = boost::iostreams;

BOOST_AUTO_TEST_CASE(sync_io)
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p1 = bp::create_pipe();
    bp::pipe p2 = bp::create_pipe();

    {
        bio::file_descriptor_source source(p1.source, bio::close_handle);
        bio::file_descriptor_sink sink(p2.sink, bio::close_handle);
        boost::system::error_code ec;
        bp::execute(
            bpi::run_exe(master_test_suite().argv[1]),
            bpi::set_cmd_line("test --stdin-to-stdout"),
            bpi::bind_stdin(source),
            bpi::bind_stdout(sink),
#if defined(BOOST_POSIX_API)
            bpi::close_fd(p1.sink),
#endif
            bpi::set_on_error(ec)
        );
        BOOST_REQUIRE(!ec);
    }

    bio::file_descriptor_sink sink(p1.sink, bio::close_handle);
    bio::stream<bio::file_descriptor_sink> os(sink);

    bio::file_descriptor_source source(p2.source, bio::close_handle);
    bio::stream<bio::file_descriptor_source> is(source);

    std::string s = "abcdefghi j";
    for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
    {
        os << *it << std::flush;
        char c;
        is >> std::noskipws >> c;
        BOOST_CHECK_EQUAL(*it, c);
    }
}
