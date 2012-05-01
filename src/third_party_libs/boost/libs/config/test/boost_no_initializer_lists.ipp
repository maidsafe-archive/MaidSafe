//  (C) Copyright Daniel James 2008.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for most recent version.


//  MACRO:         BOOST_NO_INITIALIZER_LISTS
//  TITLE:         Initializer Lists 
//  DESCRIPTION:   If the compiler does not support C++0x initializer lists

#include <initializer_list>
#include <vector>
#include <string>

namespace boost_no_initializer_lists {

void quiet_warning(const std::initializer_list<int>&){}

void f(std::initializer_list<int>)
{
}

int test()
{
    std::vector<std::string> v{"once", "upon", "a", "time"}; // See C++ std 8.5.4
    f( { 1, 2, 3, 4 } );
    std::initializer_list<int> x = { 1, 2 };
    quiet_warning(x);
    return 0;
}

}
