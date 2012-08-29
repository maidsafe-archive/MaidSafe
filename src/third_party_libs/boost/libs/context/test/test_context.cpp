
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/assert.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/utility.hpp>

#include <boost/context/all.hpp>

namespace ctx = boost::ctx;

ctx::fcontext_t fcm, fc1, fc2;
int value1 = 0;
std::string value2;
double value3 = 0.;

void f1( intptr_t)
{
    ++value1;
    ctx::jump_fcontext( & fc1, & fcm, 0);
}

void f3( intptr_t)
{
    ++value1;
    ctx::jump_fcontext( & fc1, & fcm, 0);
    ++value1;
    ctx::jump_fcontext( & fc1, & fcm, 0);
}

void f4( intptr_t)
{
    ctx::jump_fcontext( & fc1, & fcm, 7);
}

void f5( intptr_t arg)
{
    ctx::jump_fcontext( & fc1, & fcm, arg);
}

void f6( intptr_t arg)
{
    std::pair< int, int > data = * ( std::pair< int, int > * ) arg;
    int res = data.first + data.second;
    data = * ( std::pair< int, int > *)
        ctx::jump_fcontext( & fc1, & fcm, ( intptr_t) res);
    res = data.first + data.second;
    ctx::jump_fcontext( & fc1, & fcm, ( intptr_t) res);
}

void f7( intptr_t arg)
{
    try
    { throw std::runtime_error( ( char *) arg); }
    catch ( std::runtime_error const& e)
    { value2 = e.what(); }
    ctx::jump_fcontext( & fc1, & fcm, arg);
}

void f8( intptr_t arg)
{
    double d = * ( double *) arg;
    d += 3.45;
    value3 = d;
    ctx::jump_fcontext( & fc1, & fcm, 0);
}

void test_start()
{
    value1 = 0;

    ctx::stack_allocator alloc;

    fc1.fc_stack.base = alloc.allocate( ctx::minimum_stacksize() );
    fc1.fc_stack.limit = static_cast< char * >( fc1.fc_stack.base) - ctx::minimum_stacksize();
    ctx::make_fcontext( & fc1, f1);

    BOOST_CHECK_EQUAL( 0, value1);
    ctx::jump_fcontext( & fcm, & fc1, 0);
    BOOST_CHECK_EQUAL( 1, value1);
}

void test_jump()
{
    value1 = 0;

    ctx::stack_allocator alloc;

    fc1.fc_stack.base = alloc.allocate( ctx::minimum_stacksize() );
    fc1.fc_stack.limit = static_cast< char * >( fc1.fc_stack.base) - ctx::minimum_stacksize();
    ctx::make_fcontext( & fc1, f3);

    BOOST_CHECK_EQUAL( 0, value1);
    ctx::jump_fcontext( & fcm, & fc1, 0);
    BOOST_CHECK_EQUAL( 1, value1);
    ctx::jump_fcontext( & fcm, & fc1, 0);
    BOOST_CHECK_EQUAL( 2, value1);
}

void test_result()
{
    ctx::stack_allocator alloc;

    fc1.fc_stack.base = alloc.allocate( ctx::minimum_stacksize() );
    fc1.fc_stack.limit = static_cast< char * >( fc1.fc_stack.base) - ctx::minimum_stacksize();
    ctx::make_fcontext( & fc1, f4);

    int result = ( int) ctx::jump_fcontext( & fcm, & fc1, 0);
    BOOST_CHECK_EQUAL( 7, result);
}

void test_arg()
{
    ctx::stack_allocator alloc;

    fc1.fc_stack.base = alloc.allocate( ctx::minimum_stacksize() );
    fc1.fc_stack.limit = static_cast< char * >( fc1.fc_stack.base) - ctx::minimum_stacksize();
    int i = 7;
    ctx::make_fcontext( & fc1, f5);

    int result = ( int) ctx::jump_fcontext( & fcm, & fc1, i);
    BOOST_CHECK_EQUAL( i, result);
}

void test_transfer()
{
    ctx::stack_allocator alloc;

    fc1.fc_stack.base = alloc.allocate( ctx::minimum_stacksize() );
    fc1.fc_stack.limit = static_cast< char * >( fc1.fc_stack.base) - ctx::minimum_stacksize();
    std::pair< int, int > data = std::make_pair( 3, 7);
    ctx::make_fcontext( & fc1, f6);

    int result = ( int) ctx::jump_fcontext( & fcm, & fc1, ( intptr_t) & data);
    BOOST_CHECK_EQUAL( 10, result);
    data = std::make_pair( 7, 7);
    result = ( int) ctx::jump_fcontext( & fcm, & fc1, ( intptr_t) & data);
    BOOST_CHECK_EQUAL( 14, result);
}

void test_exception()
{
    ctx::stack_allocator alloc;

    fc1.fc_stack.base = alloc.allocate( ctx::minimum_stacksize() );
    fc1.fc_stack.limit = static_cast< char * >( fc1.fc_stack.base) - ctx::minimum_stacksize();
    const char * what = "hello world";
    ctx::make_fcontext( & fc1, f7);

    ctx::jump_fcontext( & fcm, & fc1, ( intptr_t) what);
    BOOST_CHECK_EQUAL( std::string( what), value2);
}

void test_fp()
{
    ctx::stack_allocator alloc;

    fc1.fc_stack.base = alloc.allocate( ctx::minimum_stacksize() );
    fc1.fc_stack.limit = static_cast< char * >( fc1.fc_stack.base) - ctx::minimum_stacksize();
    double d = 7.13;
    ctx::make_fcontext( & fc1, f8);

    ctx::jump_fcontext( & fcm, & fc1, (intptr_t) & d);
    BOOST_CHECK_EQUAL( 10.58, value3);
}

boost::unit_test::test_suite * init_unit_test_suite( int, char* [])
{
    boost::unit_test::test_suite * test =
        BOOST_TEST_SUITE("Boost.Context: context test suite");

    test->add( BOOST_TEST_CASE( & test_start) );
    test->add( BOOST_TEST_CASE( & test_jump) );
    test->add( BOOST_TEST_CASE( & test_result) );
    test->add( BOOST_TEST_CASE( & test_arg) );
    test->add( BOOST_TEST_CASE( & test_transfer) );
    test->add( BOOST_TEST_CASE( & test_exception) );
    test->add( BOOST_TEST_CASE( & test_fp) );

    return test;
}
