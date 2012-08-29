#include <boost/thread/thread.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <iostream>

boost::shared_mutex mutex;

void thread()
{
  std::cout << __FILE__ << ":" << __LINE__ << std::endl;
#ifndef BOOST_NO_EXCEPTIONS
  try
#endif
  {
    for (int i =0; i<10; ++i)
    {
      boost::system_time timeout = boost::get_system_time() + boost::posix_time::milliseconds(50);

      if (mutex.timed_lock(timeout))
      {
        std::cout << __FILE__ << ":" << __LINE__ << std::endl;
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        mutex.unlock();
        std::cout << __FILE__ << ":" << __LINE__ << std::endl;
      }
    }
  }
#ifndef BOOST_NO_EXCEPTIONS
  catch (boost::lock_error& le)
  {
    std::cerr << "lock_error exception\n";
  }
#endif
  std::cout << __FILE__ << ":" << __LINE__ << std::endl;
}

int main()
{
  std::cout << __FILE__ << ":" << __LINE__ << std::endl;
  const int nrThreads = 20;
  boost::thread* threads[nrThreads];

  for (int i = 0; i < nrThreads; ++i)
    threads[i] = new boost::thread(&thread);

  for (int i = 0; i < nrThreads; ++i)
  {
    threads[i]->join();
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    delete threads[i];
  }
  std::cout << __FILE__ << ":" << __LINE__ << std::endl;
  return 0;
}
