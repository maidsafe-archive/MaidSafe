
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>

#include <boost/assert.hpp>
#include <boost/context/all.hpp>

namespace ctx = boost::ctx;

ctx::fcontext_t fc1, fcm;

typedef std::pair< int, int >   pair_t;

void f1( intptr_t param)
{
    pair_t * p = ( pair_t *) param;

    p = ( pair_t *) ctx::jump_fcontext( & fc1, & fcm, ( intptr_t) ( p->first + p->second) );

    ctx::jump_fcontext( & fc1, & fcm, ( intptr_t) ( p->first + p->second) );
}

int main( int argc, char * argv[])
{
    ctx::stack_allocator alloc;

    fc1.fc_stack.base = alloc.allocate(ctx::minimum_stacksize());
    fc1.fc_stack.limit =
        static_cast< char * >( fc1.fc_stack.base) - ctx::minimum_stacksize();
    ctx::make_fcontext( & fc1, f1);

    pair_t p( std::make_pair( 2, 7) );
    int res = ( int) ctx::jump_fcontext( & fcm, & fc1, ( intptr_t) & p);
    std::cout << p.first << " + " << p.second << " == " << res << std::endl;

    p = std::make_pair( 5, 6);
    res = ( int) ctx::jump_fcontext( & fcm, & fc1, ( intptr_t) & p);
    std::cout << p.first << " + " << p.second << " == " << res << std::endl;

    std::cout << "main: done" << std::endl;

    return EXIT_SUCCESS;
}
