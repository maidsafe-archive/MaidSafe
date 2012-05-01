//  Copyright 2011 Vicente J. Botet Escriba
//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/chrono/chrono_io.hpp>
#include <sstream>
#include <boost/detail/lightweight_test.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <boost/chrono/thread_clock.hpp>
#include <boost/chrono/process_cpu_clocks.hpp>

template <typename Clock, typename D>
void test_good(std::string str, D res)
{
  std::istringstream in(str + boost::chrono::clock_string<Clock, char>::since());
  boost::chrono::time_point<Clock, D> tp;
  in >> tp;
  BOOST_TEST(in.eof());
  BOOST_TEST(!in.fail());
  BOOST_TEST( (tp == boost::chrono::time_point<Clock, D>(res)));
}

template <typename Clock, typename D>
void test_fail(const char* str, D)
{
  std::istringstream in(str + boost::chrono::clock_string<Clock, char>::since());
  boost::chrono::time_point<Clock, D> tp;
  in >> tp;
  BOOST_TEST(in.fail());
  BOOST_TEST( (tp == boost::chrono::time_point<Clock, D>()));
}

template <typename Clock, typename D>
void test_fail_no_epoch(const char* str, D )
{
  std::istringstream in(str);
  boost::chrono::time_point<Clock, D> tp;
  in >> tp;
  BOOST_TEST(in.fail());
  BOOST_TEST( (tp == boost::chrono::time_point<Clock, D>()));
}

template <typename Clock, typename D>
void test_fail_epoch(const char* str, D)
{
  std::istringstream in(str);
  boost::chrono::time_point<Clock, D> tp;
  in >> tp;
  BOOST_TEST(in.fail());
  BOOST_TEST( (tp == boost::chrono::time_point<Clock, D>()));
}

template <typename Clock>
void check_all()
{
  using namespace boost::chrono;
  using namespace boost;

  test_good<Clock> ("5000 hours", hours(5000));
  test_good<Clock> ("5000 minutes", minutes(5000));
  test_good<Clock> ("5000 seconds", seconds(5000));
  test_good<Clock> ("1 seconds", seconds(1));
  test_good<Clock> ("1 second", seconds(1));
  test_good<Clock> ("-1 seconds", seconds(-1));
  test_good<Clock> ("0 second", seconds(0));
  test_good<Clock> ("0 seconds", seconds(0));
  test_good<Clock> ("5000 milliseconds", milliseconds(5000));
  test_good<Clock> ("5000 microseconds", microseconds(5000));
  test_good<Clock> ("5000 nanoseconds", nanoseconds(5000));
  test_good<Clock> ("5000 deciseconds", duration<boost::int_least64_t, deci> (5000));
  test_good<Clock> ("5000 [1/30]seconds", duration<boost::int_least64_t, ratio<1, 30> > (5000));

  test_good<Clock> ("5000 h", hours(5000));
#if defined BOOST_CHRONO_DONT_PROVIDE_DEPRECATED_IO_V1
  test_good<Clock>("5000 min", minutes(5000));
#else
  test_good<Clock> ("5000 m", minutes(5000));
#endif
  test_good<Clock> ("5000 s", seconds(5000));
  test_good<Clock> ("5000 ms", milliseconds(5000));
  test_good<Clock> ("5000 ns", nanoseconds(5000));
  test_good<Clock> ("5000 ds", duration<boost::int_least64_t, deci> (5000));
  test_good<Clock> ("5000 [1/30]s", duration<boost::int_least64_t, ratio<1, 30> > (5000));

  test_good<Clock> ("5000 milliseconds", seconds(5));
  test_good<Clock> ("5 milliseconds", nanoseconds(5000000));
  test_good<Clock> ("4000 ms", seconds(4));
  test_fail<Clock> ("3001 ms", seconds(3));
  test_fail_epoch<Clock> ("3001 ms", seconds(3));
  test_fail_epoch<Clock> ("3001 ms since", seconds(3));

}

int main()
{
  std::cout << "high_resolution_clock=" << std::endl;
  check_all<boost::chrono::high_resolution_clock> ();
#ifdef BOOST_CHRONO_HAS_CLOCK_STEADY
  std::cout << "steady_clock=" << std::endl;
  check_all<boost::chrono::steady_clock> ();
#endif
  //std::cout << "system_clock=";
  //check_all<boost::chrono::system_clock>();

#if defined(BOOST_CHRONO_HAS_THREAD_CLOCK)
  std::cout << "thread_clock="<< std::endl;
  check_all<boost::chrono::thread_clock>();
#endif

#if defined(BOOST_CHRONO_HAS_PROCESS_CLOCKS)
  std::cout << "process_real_cpu_clock=" << std::endl;
  check_all<boost::chrono::process_real_cpu_clock> ();
  std::cout << "process_user_cpu_clock=" << std::endl;
  check_all<boost::chrono::process_user_cpu_clock> ();
  std::cout << "process_system_cpu_clock=" << std::endl;
  check_all<boost::chrono::process_system_cpu_clock> ();
  std::cout << "process_cpu_clock=" << std::endl;
  check_all<boost::chrono::process_cpu_clock> ();
#endif

  return boost::report_errors();

}

