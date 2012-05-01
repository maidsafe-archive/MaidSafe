//  io_ex2.cpp  ----------------------------------------------------------//

//  Copyright 2010 Howard Hinnant
//  Copyright 2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

/*
This code was adapted by Vicente J. Botet Escriba from Hinnant's html documentation.
Many thanks to Howard for making his code available under the Boost license.
*/

#include <iostream>
#include <boost/ratio/ratio_io.hpp>

int main()
{
    using namespace std;
    using namespace boost;

    cout << "ratio_string<deca, char>::long_name() = "
             <<  ratio_string<boost::deca, char>::long_name() << '\n';
    cout << "ratio_string<deca, char>::short_name() = "
             <<  ratio_string<boost::deca, char>::short_name() << '\n';

    cout << "ratio_string<giga, char>::long_name() = "
             <<  ratio_string<boost::giga, char>::long_name() << '\n';
    cout << "ratio_string<giga, char>::short_name() = "
             <<  ratio_string<boost::giga, char>::short_name() << '\n';

    cout << "ratio_string<ratio<4, 6>, char>::long_name() = "
             <<  ratio_string<boost::ratio<4, 6>, char>::long_name() << '\n';
    cout << "ratio_string<ratio<4, 6>, char>::short_name() = "
             <<  ratio_string<boost::ratio<4, 6>, char>::short_name() << '\n';
        
    return 0;
}

