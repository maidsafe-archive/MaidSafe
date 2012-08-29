//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// Copyright (C) 2011 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/timed_mutex.hpp>

// class timed_mutex;

// timed_mutex(const timed_mutex&) = delete;

#include <boost/thread/mutex.hpp>
#include <boost/detail/lightweight_test.hpp>

int main()
{
  boost::timed_mutex m0;
  boost::timed_mutex m1(m0);
}

void remove_unused_warning()
{
  //../../../boost/system/error_code.hpp:214:36: warning: ‘boost::system::posix_category’ defined but not used [-Wunused-variable]
  //../../../boost/system/error_code.hpp:215:36: warning: ‘boost::system::errno_ecat’ defined but not used [-Wunused-variable]
  //../../../boost/system/error_code.hpp:216:36: warning: ‘boost::system::native_ecat’ defined but not used [-Wunused-variable]

  (void)boost::system::posix_category;
  (void)boost::system::errno_ecat;
  (void)boost::system::native_ecat;
}
