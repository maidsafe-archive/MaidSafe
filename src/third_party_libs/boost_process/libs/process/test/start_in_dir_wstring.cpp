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
#define BOOST_TEST_IGNORE_SIGCHLD
#include <boost/test/included/unit_test.hpp>
#include <boost/process.hpp>
#include <boost/system/error_code.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filter/newline.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace bp = boost::process;
namespace bpi = boost::process::initializers;

struct test_dir
{
    std::wstring ws_;
    test_dir(const std::wstring &ws) : ws_(ws)
    { BOOST_REQUIRE_NO_THROW(boost::filesystem::create_directory(ws)); }
    ~test_dir() { boost::filesystem::remove(ws_); }
};

BOOST_AUTO_TEST_CASE(start_in_dir)
{
    using boost::unit_test::framework::master_test_suite;
    namespace bio = boost::iostreams;

    test_dir dir(L"start_in_dir_test");

    boost::filesystem::path exe = master_test_suite().argv[1];

    bp::pipe p = bp::create_pipe();

    bio::file_descriptor_sink sink(p.sink, bio::close_handle);
    boost::system::error_code ec;
    bp::child c = bp::execute(
        bpi::run_exe(boost::filesystem::absolute(exe)),
        bpi::set_cmd_line(L"test --pwd"),
        bpi::start_in_dir(dir.ws_),
        bpi::bind_stdout(sink),
        bpi::set_on_error(ec)
    );
    BOOST_REQUIRE(!ec);
    bio::close(sink);

    bio::file_descriptor_source source(p.source, bio::close_handle);
    bio::filtering_istream is;
    is.push(bio::newline_filter(bio::newline::posix));
    is.push(source);

    std::string s;
    std::getline(is, s);
    BOOST_CHECK_EQUAL(s, boost::filesystem::absolute(dir.ws_));

    BOOST_REQUIRE_NO_THROW(bp::wait_for_exit(c));
}
