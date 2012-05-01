//  This file was automatically generated on Fri May 15 11:57:42 2009
//  by libs/config/tools/generate.cpp
//  Copyright John Maddock 2002-4.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for the most recent version.//
//  Revision $Id: no_0x_hdr_future_pass.cpp 53445 2009-05-30 11:22:59Z bemandawes $
//


// Test file for macro BOOST_NO_0X_HDR_FUTURE
// This file should compile, if it does not then
// BOOST_NO_0X_HDR_FUTURE should be defined.
// See file boost_no_0x_hdr_future.ipp for details

// Must not have BOOST_ASSERT_CONFIG set; it defeats
// the objective of this file:
#ifdef BOOST_ASSERT_CONFIG
#  undef BOOST_ASSERT_CONFIG
#endif

#include <boost/config.hpp>
#include "test.hpp"

#ifndef BOOST_NO_0X_HDR_FUTURE
#include "boost_no_0x_hdr_future.ipp"
#else
namespace boost_no_0x_hdr_future = empty_boost;
#endif

int main( int, char *[] )
{
   return boost_no_0x_hdr_future::test();
}

