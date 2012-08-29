//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINER_TEST_CHECK_EQUAL_CONTAINER_HPP
#define BOOST_CONTAINER_TEST_CHECK_EQUAL_CONTAINER_HPP

#include <boost/container/detail/config_begin.hpp>
#include <functional>
#include <iostream>
#include <algorithm>

namespace boost{
namespace container {
namespace test{

//Function to check if both containers are equal
template<class MyBoostCont
        ,class MyStdCont>
bool CheckEqualContainers(const MyBoostCont *boostcont, const MyStdCont *stdcont)
{
   if(boostcont->size() != stdcont->size())
      return false;

   typedef typename MyBoostCont::value_type value_type;

   typename MyBoostCont::const_iterator itboost(boostcont->begin()), itboostend(boostcont->end());
   typename MyStdCont::const_iterator itstd(stdcont->begin());
   typename MyStdCont::size_type dist = (typename MyStdCont::size_type)std::distance(itboost, itboostend);
   if(dist != boostcont->size()){
      return false;
   }
   std::size_t i = 0;
   for(; itboost != itboostend; ++itboost, ++itstd, ++i){
      value_type val(*itstd);
      const value_type &v = *itboost;
      if(v != val)
         return false;
   }
   return true;
}

template<class MyBoostCont
        ,class MyStdCont>
bool CheckEqualPairContainers(const MyBoostCont *boostcont, const MyStdCont *stdcont)
{
   if(boostcont->size() != stdcont->size())
      return false;

   typedef typename MyBoostCont::key_type      key_type;
   typedef typename MyBoostCont::mapped_type   mapped_type;

   typename MyBoostCont::const_iterator itboost(boostcont->begin()), itboostend(boostcont->end());
   typename MyStdCont::const_iterator itstd(stdcont->begin());
   for(; itboost != itboostend; ++itboost, ++itstd){
      if(itboost->first != key_type(itstd->first))
         return false;

      if(itboost->second != mapped_type(itstd->second))
         return false;
   }
   return true;
}
}  //namespace test{
}  //namespace container {
}  //namespace boost{

#include <boost/container/detail/config_end.hpp>

#endif //#ifndef BOOST_CONTAINER_TEST_CHECK_EQUAL_CONTAINER_HPP
