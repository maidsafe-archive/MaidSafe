// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process.hpp>
#include <boost/asio.hpp>
#if defined(BOOST_WINDOWS_API)
#   include <Windows.h>
#elif defined(BOOST_POSIX_API)
#   include <sys/wait.h>
#   include <errno.h>
#endif

using namespace boost::process;
using namespace boost::process::initializers;

int main()
{
    {
//[sync
    child c = execute(run_exe("test.exe"));
    auto exit_code = wait_for_exit(c);
//]
    }

    {
//[async
    boost::asio::io_service io_service;

#if defined(BOOST_POSIX_API)
    int status;
    boost::asio::signal_set set(io_service, SIGCHLD);
    set.async_wait(
        [&status](const boost::system::error_code&, int) { ::wait(&status); }
    );
#endif

    child c = execute(
        run_exe("test.exe")
#if defined(BOOST_POSIX_API)
        , notify_io_service(io_service)
#endif
    );

#if defined(BOOST_WINDOWS_API)
    DWORD exit_code;
    boost::asio::windows::object_handle handle(io_service, c.process_handle());
    handle.async_wait(
        [&handle, &exit_code](const boost::system::error_code&)
            { ::GetExitCodeProcess(handle.native(), &exit_code); }
    );
#endif

    io_service.run();
//]
    }
}
