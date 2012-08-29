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

// void join();

#define BOOST_THREAD_VESRION 3
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <new>
#include <cstdlib>
#include <cassert>
#include <iostream>
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
    std::cout << __FILE__ << ":" << __LINE__ <<" " << n_alive << std::endl;
    BOOST_TEST(n_alive == 1);
    op_run = true;
  }
};

int G::n_alive = 0;
bool G::op_run = false;

boost::thread* resource_deadlock_would_occur_th;
boost::mutex resource_deadlock_would_occur_mtx;
void resource_deadlock_would_occur_tester()
{
  try
  {
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
    boost::unique_lock<boost::mutex> lk(resource_deadlock_would_occur_mtx);
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;

    resource_deadlock_would_occur_th->join();
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
    BOOST_TEST(false);
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
  }
  catch (boost::system::system_error& e)
  {
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
    BOOST_TEST(e.code().value() == boost::system::errc::resource_deadlock_would_occur);
  }
  catch (...)
  {
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
    BOOST_TEST(false&&"exception thrown");
  }
}

int main()
{
  {
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
    boost::thread t0( (G()));
    BOOST_TEST(t0.joinable());
    t0.join();
    BOOST_TEST(!t0.joinable());
  }
  {
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
    boost::unique_lock<boost::mutex> lk(resource_deadlock_would_occur_mtx);
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
    boost::thread t0( resource_deadlock_would_occur_tester );
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
    resource_deadlock_would_occur_th = &t0;
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
    BOOST_TEST(t0.joinable());
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
    lk.unlock();
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
    t0.join();
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
    BOOST_TEST(!t0.joinable());
    std::cout << __FILE__ << ":" << __LINE__ <<" " << std::endl;
  }

//  {
//    boost::thread t0( (G()));
//    t0.detach();
//    try
//    {
//      t0.join();
//      BOOST_TEST(false);
//    }
//    catch (boost::system::system_error& e)
//    {
//      BOOST_TEST(e.code().value() == boost::system::errc::no_such_process);
//    }
//  }
//  {
//    boost::thread t0( (G()));
//    BOOST_TEST(t0.joinable());
//    t0.join();
//    BOOST_TEST(!t0.joinable());
//    try
//    {
//      t0.join();
//      BOOST_TEST(false);
//    }
//    catch (boost::system::system_error& e)
//    {
//      BOOST_TEST(e.code().value() == boost::system::errc::invalid_argument);
//    }
//
//  }

  return boost::report_errors();
}

