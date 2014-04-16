// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process.hpp>
#include <iostream>
#include <Windows.h>

using namespace boost::process;
using namespace boost::process::initializers;

int main()
{
//[show_window
    execute(
        run_exe("test.exe"),
        show_window(SW_HIDE)
    );
//]

//[create_process
    execute(
        run_exe("test.exe"),
        on_CreateProcess_setup([](executor &e)
            { e.startup_info.dwFlags = STARTF_RUNFULLSCREEN; }),
        on_CreateProcess_error([](executor&)
            { std::cerr << GetLastError() << std::endl; })
    );
//]
}
