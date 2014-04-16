// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#if defined(BOOST_WINDOWS_API)
#   include <Windows.h>
#endif

using namespace boost::process;
using namespace boost::process::initializers;
using namespace boost::iostreams;

boost::process::pipe create_async_pipe()
{
#if defined(BOOST_WINDOWS_API)
    std::string name = "\\\\.\\pipe\\boost_process_async_io";
    HANDLE handle1 = ::CreateNamedPipeA(name.c_str(), PIPE_ACCESS_INBOUND |
        FILE_FLAG_OVERLAPPED, 0, 1, 8192, 8192, 0, NULL);
    HANDLE handle2 = ::CreateFileA(name.c_str(), GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    return make_pipe(handle1, handle2);
#elif defined(BOOST_POSIX_API)
    return create_pipe();
#endif
}

int main()
{
//[async_io
    boost::process::pipe p = create_async_pipe();

    file_descriptor_sink sink(p.sink, close_handle);
    execute(
        run_exe("test.exe"),
        bind_stdout(sink)
    );

    file_descriptor_source source(p.source, close_handle);

#if defined(BOOST_WINDOWS_API)
    typedef boost::asio::windows::stream_handle pipe_end;
#elif defined(BOOST_POSIX_API)
    typedef boost::asio::posix::stream_descriptor pipe_end;
#endif

    boost::asio::io_service io_service;
    pipe_end pend(io_service, p.source);

    boost::array<char, 4096> buffer;
    boost::asio::async_read(pend, boost::asio::buffer(buffer),
        [](const boost::system::error_code&, std::size_t){});

    io_service.run();
//]
}
