#include <boost/thread.hpp>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#if defined(BOOST_THREAD_PLATFORM_PTHREAD)
#include <unistd.h>
#endif

boost::mutex mtx;
boost::condition_variable cv;

using namespace boost::posix_time;
using namespace boost::gregorian;
int main()
{
#if defined(BOOST_THREAD_PLATFORM_PTHREAD)

  for (int i=0; i<3; ++i)
  {
    const time_t now_time = ::time(0);
    const time_t wait_time = now_time+1;
    time_t end_time;
    assert(now_time < wait_time);

    boost::mutex::scoped_lock lk(mtx);
    //const bool res =
    (void)cv.timed_wait(lk, from_time_t(wait_time));
    end_time = ::time(0);
    std::cerr << "now_time =" << now_time << " \n";
    std::cerr << "end_time =" << end_time << " \n";
    std::cerr << "wait_time=" << wait_time << " \n";
    std::cerr << end_time - wait_time << " \n";
    assert(end_time >= wait_time);
    std::cerr << " OK\n";
  }
#endif
  return 0;
}
