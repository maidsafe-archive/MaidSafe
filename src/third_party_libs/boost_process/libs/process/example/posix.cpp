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
#include <boost/assign/list_of.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <errno.h>

using namespace boost::process;
using namespace boost::process::initializers;
using namespace boost::iostreams;

int main()
{
//[bind_fd
    file_descriptor_sink sink("output.txt");
    execute(
        run_exe("test"),
        bind_fd(4, sink)
    );
//]

//[close_fd
    execute(
        run_exe("test"),
        close_fd(STDIN_FILENO)
    );
//]

//[close_fds
    execute(
        run_exe("test"),
        close_fds(boost::assign::list_of(STDIN_FILENO)(4))
    );
//]

//[close_fds_if
    execute(
        run_exe("test"),
        close_fds_if([](int fd){ return fd == STDIN_FILENO; })
    );
//]

//[fork_execve
    const char *env[2] = { 0 };
    env[0] = "LANG=de";
    execute(
        run_exe("test"),
        on_fork_setup([env](executor &e)
            { e.env = const_cast<char**>(env); }),
        on_fork_error([](executor&)
            { std::cerr << errno << std::endl; }),
        on_exec_setup([](executor&)
            { chroot("/new/root/directory/"); }),
        on_exec_error([](executor&)
            { std::ofstream ofs("log.txt"); if (ofs) ofs << errno; })
    );
//]
}
