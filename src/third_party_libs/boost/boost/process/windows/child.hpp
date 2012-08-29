// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/**
 * \file boost/process/windows/child.hpp
 *
 * Defines a child process class.
 */

#ifndef BOOST_PROCESS_WINDOWS_CHILD_HPP
#define BOOST_PROCESS_WINDOWS_CHILD_HPP

#include <Windows.h>

namespace boost { namespace process { namespace windows {

struct child
{
    PROCESS_INFORMATION proc_info;

    explicit child(const PROCESS_INFORMATION &pi) : proc_info(pi) {}

    HANDLE process_handle() const { return proc_info.hProcess; }

    void discard() { CloseHandle(proc_info.hProcess); CloseHandle(proc_info.hThread); }
};

}}}

#endif
