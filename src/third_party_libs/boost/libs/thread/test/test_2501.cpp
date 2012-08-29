#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

int main() {

    boost::shared_mutex mtx; boost::upgrade_lock<boost::shared_mutex> lk(mtx);

    boost::upgrade_to_unique_lock<boost::shared_mutex> lk2(lk);

    return 0;
}
