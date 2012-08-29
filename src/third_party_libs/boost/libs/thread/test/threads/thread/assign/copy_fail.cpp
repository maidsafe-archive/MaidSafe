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

// <boost/thread/thread.hpp>

// class thread

// thread& operator=(thread&& t);

#include <boost/thread/thread.hpp>
#include <new>
#include <cstdlib>
#include <boost/detail/lightweight_test.hpp>

class G
{
  int alive_;
public:
  static int n_alive;
  static bool op_run;

  G() :
    alive_(1)
  {
    ++n_alive;
  }
  G(const G& g) :
    alive_(g.alive_)
  {
    ++n_alive;
  }
  ~G()
  {
    alive_ = 0;
    --n_alive;
  }

  void operator()()
  {
    BOOST_TEST(alive_ == 1);
    BOOST_TEST(n_alive == 1);
    op_run = true;
  }

};

int G::n_alive = 0;
bool G::op_run = false;

int main()
{
  {
    boost::thread t0( (G()));
    boost::thread t1;
    t1 = t0;
  }
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
