#include <boost/thread.hpp>

void run_thread() {
        return;
}

int main() {
        boost::thread t(run_thread);
        return 0;
}

