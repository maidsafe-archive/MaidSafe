#include <boost/thread.hpp>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

boost::mutex mtx;
boost::condition_variable cv;

int main()
{
         for (int i=0; i<3; ++i) {
                 const time_t wait_time = ::time(0)+1;

                 boost::mutex::scoped_lock lk(mtx);
                 const bool res = cv.timed_wait(lk, boost::posix_time::from_time_t(wait_time));
                 const time_t end_time = ::time(0);
                 assert(end_time >= wait_time);
                 std::cerr << end_time - wait_time << " OK\n";
         }
         return 0;
}
