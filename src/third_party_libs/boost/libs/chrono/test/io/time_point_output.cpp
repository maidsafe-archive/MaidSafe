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
void test_good_prefix(const char* str, D d)
{
  std::ostringstream out;
  boost::chrono::time_point<Clock, D> tp(d);
  out << tp;
  BOOST_TEST(out.good());
  BOOST_TEST( (out.str() == std::string(str) + boost::chrono::clock_string<Clock, char>::since()));
}

template <typename Clock, typename D>
void test_good_symbol(const char* str, D d)
{
  std::ostringstream out;
  boost::chrono::time_point<Clock, D> tp(d);
#if defined BOOST_CHRONO_DONT_PROVIDE_DEPRECATED_IO_V1
  out << boost::chrono::duration_fmt(boost::chrono::duration_style::symbol) << tp;
#else
  out << boost::chrono::duration_short << tp;
#endif
  BOOST_TEST(out.good());
  BOOST_TEST( (out.str() == std::string(str) + boost::chrono::clock_string<Clock, char>::since()));
}

#if defined BOOST_CHRONO_DONT_PROVIDE_DEPRECATED_IO_V1
template<typename Clock, typename D>
void test_good(const char* str, D d, boost::chrono::duration_style::type style)
{
  std::ostringstream out;
  boost::chrono::time_point<Clock,D> tp(d);
  out << boost::chrono::duration_fmt(style) << tp;
  BOOST_TEST(out.good());
  BOOST_TEST((out.str() == std::string(str)+boost::chrono::clock_string<Clock,char>::since()));
}
#endif

template <typename Clock>
void check_all()
{
  using namespace boost::chrono;
  using namespace boost;

#if defined BOOST_CHRONO_DONT_PROVIDE_DEPRECATED_IO_V1
  test_good<Clock>("2 hours", hours(2), duration_style::prefix);
  test_good<Clock>("2 h", hours(2), duration_style::symbol);
#endif

  test_good_prefix<Clock> ("2 hours", hours(2));
  test_good_prefix<Clock> ("2 minutes", minutes(2));
  test_good_prefix<Clock> ("2 seconds", seconds(2));
  test_good_prefix<Clock> ("1 second", seconds(1));
  test_good_prefix<Clock> ("-1 second", seconds(-1));
  test_good_prefix<Clock> ("0 seconds", seconds(0));
  test_good_prefix<Clock> ("2 milliseconds", milliseconds(2));
  test_good_prefix<Clock> ("2 microseconds", microseconds(2));
  test_good_prefix<Clock> ("2 nanoseconds", nanoseconds(2));
  test_good_prefix<Clock> ("2 deciseconds", duration<boost::int_least64_t, deci> (2));
  test_good_prefix<Clock> ("2 [1/30]seconds", duration<boost::int_least64_t, ratio<1, 30> > (2));

  test_good_symbol<Clock> ("2 h", hours(2));
#if defined BOOST_CHRONO_DONT_PROVIDE_DEPRECATED_IO_V1
  test_good_symbol<Clock>("2 min", minutes(2));
#else
  test_good_symbol<Clock> ("2 m", minutes(2));
#endif
  test_good_symbol<Clock> ("2 s", seconds(2));
  test_good_symbol<Clock> ("2 ms", milliseconds(2));
  test_good_symbol<Clock> ("2 ns", nanoseconds(2));
  test_good_symbol<Clock> ("2 ds", duration<boost::int_least64_t, deci> (2));
  test_good_symbol<Clock> ("2 [1/30]s", duration<boost::int_least64_t, ratio<1, 30> > (2));
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

