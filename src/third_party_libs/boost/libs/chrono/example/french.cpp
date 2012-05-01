//  french.cpp  ----------------------------------------------------------//

//  Copyright 2010 Howard Hinnant
//  Copyright 2011 Vicente J. Botet Escriba
//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

// Adapted to Boost from the original Hawards's code

#include <boost/chrono/chrono_io.hpp>
#include <boost/chrono/process_cpu_clocks.hpp>
#include <boost/chrono/thread_clock.hpp>
#include <iostream>
#include <locale>

int main()
{
    using std::cout;
    using std::locale;
    using namespace boost;
    using namespace boost::chrono;

    cout.imbue(locale(locale(), new duration_punct<char>
        (
            duration_punct<char>::use_long,
            "secondes", "minutes", "heures",
            "s", "m", "h"
        )));
    hours h(5);
    minutes m(45);
    seconds s(15);
    milliseconds ms(763);
    cout << h << ", " << m << ", " << s << " et " << ms << '\n';
}
