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
#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <string>
#include <istream>
#include <cstdlib>
#if defined(BOOST_WINDOWS_API)
#   include <Windows.h>
typedef boost::asio::windows::stream_handle pipe_end;
#elif defined(BOOST_POSIX_API)
#   include <sys/wait.h>
#   include <unistd.h>
typedef boost::asio::posix::stream_descriptor pipe_end;
#endif

namespace bp = boost::process;
namespace bpi = boost::process::initializers;
namespace bio = boost::iostreams;

BOOST_AUTO_TEST_CASE(sync_io)
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p = bp::create_pipe();

    {
        bio::file_descriptor_sink sink(p.sink, bio::close_handle);
        boost::system::error_code ec;
        bp::execute(
            bpi::run_exe(master_test_suite().argv[1]),
            bpi::set_cmd_line("test --echo-stderr hello"),
            bpi::bind_stderr(sink),
            bpi::set_on_error(ec)
        );
        BOOST_REQUIRE(!ec);
    }

    bio::file_descriptor_source source(p.source, bio::close_handle);
    bio::stream<bio::file_descriptor_source> is(source);

    std::string s;
    is >> s;
    BOOST_CHECK_EQUAL(s, "hello");
}

bp::pipe create_async_pipe()
{
#if defined(BOOST_WINDOWS_API)
    std::string name = "\\\\.\\pipe\\boost_process_test_bind_stderr";
    HANDLE handle1 = CreateNamedPipeA(name.c_str(), PIPE_ACCESS_INBOUND |
        FILE_FLAG_OVERLAPPED, 0, 1, 8192, 8192, 0, NULL);
    HANDLE handle2 = CreateFileA(name.c_str(), GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    return bp::make_pipe(handle1, handle2);
#elif defined(BOOST_POSIX_API)
    return bp::create_pipe();
#endif
}

struct read_handler
{
    boost::asio::streambuf &buffer_;

    read_handler(boost::asio::streambuf &buffer) : buffer_(buffer) {}

    void operator()(const boost::system::error_code &ec, std::size_t size)
    {
        BOOST_REQUIRE(!ec);
        std::istream is(&buffer_);
        std::string line;
        std::getline(is, line);
        BOOST_CHECK(boost::algorithm::starts_with(line, "abc"));
    }
};

BOOST_AUTO_TEST_CASE(async_io)
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p = create_async_pipe();

    {
        bio::file_descriptor_sink sink(p.sink, bio::close_handle);
        boost::system::error_code ec;
        bp::execute(
            bpi::run_exe(master_test_suite().argv[1]),
            bpi::set_cmd_line("test --echo-stderr abc"),
            bpi::bind_stderr(sink),
            bpi::set_on_error(ec)
        );
        BOOST_REQUIRE(!ec);
    }

    boost::asio::io_service io_service;
    pipe_end pend(io_service, p.source);

    boost::asio::streambuf buffer;
    boost::asio::async_read_until(pend, buffer, '\n',
        read_handler(buffer));

    io_service.run();
}

BOOST_AUTO_TEST_CASE(nul)
{
    using boost::unit_test::framework::master_test_suite;

#if defined(BOOST_WINDOWS_API)
    bio::file_descriptor_sink sink("NUL");
#elif defined(BOOST_POSIX_API)
    bio::file_descriptor_sink sink("/dev/null");
#endif

    boost::system::error_code ec;
    bp::child c = bp::execute(
        bpi::run_exe(master_test_suite().argv[1]),
        bpi::set_cmd_line("test --is-nul-stderr"),
        bpi::bind_stderr(sink),
        bpi::set_on_error(ec)
    );
    BOOST_REQUIRE(!ec);

    int exit_code = bp::wait_for_exit(c);
#if defined(BOOST_WINDOWS_API)
    BOOST_CHECK_EQUAL(EXIT_SUCCESS, exit_code);
#elif defined(BOOST_POSIX_API)
    BOOST_CHECK_EQUAL(EXIT_SUCCESS, WEXITSTATUS(exit_code));
#endif
}
