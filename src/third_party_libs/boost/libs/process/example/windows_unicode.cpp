// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//[unicode
#define UNICODE
#include <boost/process.hpp>
#include <boost/filesystem.hpp>

using namespace boost::process;
using namespace boost::process::initializers;

int main()
{
    boost::filesystem::path p = L"C:\\";
    execute(
        run_exe(L"test.exe"),
        set_cmd_line(L"test --help"),
        start_in_dir(p)
    );
}
//]
