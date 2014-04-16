// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process.hpp>
#if defined(BOOST_POSIX_API)
#   include <signal.h>
#endif

using namespace boost::process;
using namespace boost::process::initializers;

int main()
{
//[cleanup
    child c = execute(run_exe("test.exe"));
    wait_for_exit(c);
//]

//[cleanup_posix
#if defined(BOOST_POSIX_API)
    signal(SIGCHLD, SIG_IGN);
#endif
    execute(run_exe("test.exe"));
//]

//[cleanup_windows
    {
        child c = execute(run_exe("test.exe"));
    }
//]
}
