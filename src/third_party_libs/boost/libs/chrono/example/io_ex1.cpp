//  io_ex1.cpp  ----------------------------------------------------------//

//  Copyright 2010 Howard Hinnant
//  Copyright 2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

/*
This code was adapted by Vicente J. Botet Escriba from Hinnant's html documentation.
Many thanks to Howard for making his code available under the Boost license.

*/

#include <iostream>
#include <boost/chrono/chrono_io.hpp>
#include <boost/chrono/thread_clock.hpp>

int main()
{
    using std::cout;
    using namespace boost;
    using namespace boost::chrono;

    cout << "milliseconds(3) + microseconds(10) = "
         <<  milliseconds(3) + microseconds(10) << '\n';

    cout << "hours(3) + minutes(10) = "
         <<  hours(3) + minutes(10) << '\n';

    typedef duration<long long, ratio<1, 2500000000ULL> > ClockTick;
    cout << "ClockTick(3) + nanoseconds(10) = "
         <<  ClockTick(3) + nanoseconds(10) << '\n';

    cout << "\nSet cout to use short names:\n";
    cout << duration_short;

    cout << "milliseconds(3) + microseconds(10) = "
         <<  milliseconds(3) + microseconds(10) << '\n';

    cout << "hours(3) + minutes(10) = "
         <<  hours(3) + minutes(10) << '\n';

    cout << "ClockTick(3) + nanoseconds(10) = "
         <<  ClockTick(3) + nanoseconds(10) << '\n';

    cout << "\nsystem_clock::now() = " << system_clock::now() << '\n';
#ifdef BOOST_CHRONO_HAS_CLOCK_STEADY
    cout << "steady_clock::now() = " << steady_clock::now() << '\n';
#endif
    cout << "\nSet cout to use long names:\n" << duration_long
         << "high_resolution_clock::now() = " << high_resolution_clock::now() << '\n';
#if defined(BOOST_CHRONO_HAS_THREAD_CLOCK)
    cout << "\nthread_clock::now() = " << thread_clock::now() << '\n';
#endif
    cout << "\nprocess_real_cpu_clock::now() = " << process_real_cpu_clock::now() << '\n';
    cout << "\nprocess_user_cpu_clock::now() = " << process_user_cpu_clock::now() << '\n';
    cout << "\nprocess_system_cpu_clock::now() = " << process_system_cpu_clock::now() << '\n';
    cout << "\nprocess_cpu_clock::now() = " << process_cpu_clock::now() << '\n';
    return 0;
}
