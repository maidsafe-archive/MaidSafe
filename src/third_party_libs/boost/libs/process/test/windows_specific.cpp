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
#include <Windows.h>

namespace bp = boost::process;
namespace bpi = boost::process::initializers;
namespace bio = boost::iostreams;

void set_flags(bp::executor &e)
{
    e.startup_info.dwFlags |= STARTF_FORCEOFFFEEDBACK;
}

BOOST_AUTO_TEST_CASE(flags)
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p = bp::create_pipe();

    {
        bio::file_descriptor_sink sink(p.sink, bio::close_handle);
        boost::system::error_code ec;
        bp::execute(
            bpi::run_exe(master_test_suite().argv[1]),
            bpi::set_cmd_line("test --windows-print-flags"),
            bpi::bind_stdout(sink),
            bpi::on_CreateProcess_setup(set_flags),
            bpi::set_on_error(ec)
        );
        BOOST_CHECK(!ec);
    }

    bio::file_descriptor_source source(p.source, bio::close_handle);
    bio::stream<bio::file_descriptor_source> is(source);

    int i;
    is >> i;
    BOOST_CHECK_EQUAL(i, STARTF_USESTDHANDLES | STARTF_FORCEOFFFEEDBACK);
}
