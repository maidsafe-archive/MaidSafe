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
#include <string>
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

    bp::pipe p1 = bp::create_pipe();
    bp::pipe p2 = bp::create_pipe();

    {
        bio::file_descriptor_source source1(p1.source, bio::close_handle);
        bio::file_descriptor_sink sink2(p2.sink, bio::close_handle);
        boost::system::error_code ec;
        bp::execute(
            bpi::run_exe(master_test_suite().argv[1]),
            bpi::set_cmd_line("test --prefix abc"),
            bpi::bind_stdin(source1),
            bpi::bind_stdout(sink2),
#if defined(BOOST_POSIX_API)
            bpi::close_fd(p1.sink),
#endif
            bpi::set_on_error(ec)
        );
        BOOST_REQUIRE(!ec);
    }

    bio::file_descriptor_sink sink1(p1.sink, bio::close_handle);
    bio::stream<bio::file_descriptor_sink> os(sink1);

    os << "hello" << std::endl;

    bio::file_descriptor_source source2(p2.source, bio::close_handle);
    bio::stream<bio::file_descriptor_source> is(source2);

    std::string s;
    is >> s;
    BOOST_CHECK_EQUAL(s, "abchello");
    os << 123 << std::endl;
    is >> s;
    BOOST_CHECK_EQUAL(s, "abc123");
    os << 3.1415 << std::endl;
    is >> s;
    BOOST_CHECK_EQUAL(s, "abc3.1415");
}

bp::pipe create_async_pipe()
{
#if defined(BOOST_WINDOWS_API)
    std::string name = "\\\\.\\pipe\\boost_process_test_bind_stdin";
    HANDLE handle1 = CreateNamedPipeA(name.c_str(), PIPE_ACCESS_INBOUND,
        0, 1, 8192, 8192, 0, NULL);
    HANDLE handle2 = CreateFileA(name.c_str(), GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    return bp::make_pipe(handle1, handle2);
#elif defined(BOOST_POSIX_API)
    return bp::create_pipe();
#endif
}

struct write_handler
{
    bio::stream<bio::file_descriptor_source> &is_;

    write_handler(bio::stream<bio::file_descriptor_source> &is) : is_(is) {}

    void operator()(const boost::system::error_code &ec, std::size_t size)
    {
        BOOST_REQUIRE(!ec);
        BOOST_REQUIRE_EQUAL(6u, size);
        std::string s;
        is_ >> s;
        BOOST_CHECK_EQUAL(s, "abchello");
    }
};

BOOST_AUTO_TEST_CASE(async_io)
{
    using boost::unit_test::framework::master_test_suite;

    bp::pipe p1 = create_async_pipe();
    bp::pipe p2 = bp::create_pipe();

    {
        bio::file_descriptor_source source1(p1.source, bio::close_handle);
        bio::file_descriptor_sink sink2(p2.sink, bio::close_handle);
        boost::system::error_code ec;
        bp::execute(
            bpi::run_exe(master_test_suite().argv[1]),
            bpi::set_cmd_line("test --prefix-once abc"),
            bpi::bind_stdin(source1),
            bpi::bind_stdout(sink2),
            bpi::set_on_error(ec)
        );
        BOOST_REQUIRE(!ec);
    }

    bio::file_descriptor_source source2(p2.source, bio::close_handle);
    bio::stream<bio::file_descriptor_source> is(source2);

    boost::asio::io_service io_service;
    pipe_end pend(io_service, p1.sink);

    std::string s = "hello\n";
    boost::asio::async_write(pend, boost::asio::buffer(s),
        write_handler(is));

    io_service.run();
}

BOOST_AUTO_TEST_CASE(nul)
{
    using boost::unit_test::framework::master_test_suite;

#if defined(BOOST_WINDOWS_API)
    bio::file_descriptor_source source("NUL");
#elif defined(BOOST_POSIX_API)
    bio::file_descriptor_source source("/dev/null");
#endif

    boost::system::error_code ec;
    bp::child c = bp::execute(
        bpi::run_exe(master_test_suite().argv[1]),
        bpi::set_cmd_line("test --is-nul-stdin"),
        bpi::bind_stdin(source),
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
