// Copyright (C) 2012 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/locks.hpp>

// template <class Mutex> class reverse_lock;

// reverse_lock& operator=(reverse_lock const&) = delete;

#include <boost/thread/locks.hpp>
#include <boost/thread/reverse_lock.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/detail/lightweight_test.hpp>


int main()
{
  boost::mutex m0;
  boost::mutex m1;
  boost::unique_lock<boost::mutex> lk0(m0);
  boost::unique_lock<boost::mutex> lk1(m1);
  {
    boost::reverse_lock<boost::unique_lock<boost::mutex> > lg0(lk0);
    boost::reverse_lock<boost::unique_lock<boost::mutex> > lg1(lk1);
    lg1 = lg0;
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
