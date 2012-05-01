// Copyright John Maddock 2006.
// Copyright Paul A. Bristow 2007, 2009
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_MATH_OVERFLOW_ERROR_POLICY ignore_error

#include <boost/math/concepts/real_concept.hpp>
#include <boost/test/test_exec_monitor.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <boost/math/tools/stats.hpp>
#include <boost/math/tools/test.hpp>
#include <boost/array.hpp>
#include "functor.hpp"

#include "handle_test_result.hpp"

#ifndef SC_
#define SC_(x) static_cast<T>(BOOST_JOIN(x, L))
#endif

struct negative_tgamma_ratio
{
   template <class Row>
   typename Row::value_type operator()(const Row& row)
   {
      return boost::math::tgamma_delta_ratio(row[0], -row[1]);
   }
};

template <class T>
void do_test_tgamma_delta_ratio(const T& data, const char* type_name, const char* test_name)
{
   typedef typename T::value_type row_type;
   typedef typename row_type::value_type value_type;

   typedef value_type (*pg)(value_type, value_type);
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   pg funcp = boost::math::tgamma_delta_ratio<value_type, value_type>;
#else
   pg funcp = boost::math::tgamma_delta_ratio;
#endif

   boost::math::tools::test_result<value_type> result;

   std::cout << "Testing " << test_name << " with type " << type_name
      << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

   //
   // test tgamma_delta_ratio against data:
   //
   result = boost::math::tools::test(
      data, 
      bind_func(funcp, 0, 1), 
      extract_result(2));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::tgamma_delta_ratio(a, delta)", test_name);
   result = boost::math::tools::test(
      data, 
      negative_tgamma_ratio(), 
      extract_result(3));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::tgamma_delta_ratio(a -delta)", test_name);
}

template <class T>
void do_test_tgamma_ratio(const T& data, const char* type_name, const char* test_name)
{
   typedef typename T::value_type row_type;
   typedef typename row_type::value_type value_type;

   typedef value_type (*pg)(value_type, value_type);
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   pg funcp = boost::math::tgamma_ratio<value_type, value_type>;
#else
   pg funcp = boost::math::tgamma_ratio;
#endif

   boost::math::tools::test_result<value_type> result;

   std::cout << "Testing " << test_name << " with type " << type_name
      << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

   //
   // test tgamma_ratio against data:
   //
   result = boost::math::tools::test(
      data, 
      bind_func(funcp, 0, 1), 
      extract_result(2));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::tgamma_ratio(a, b)", test_name);
}

template <class T>
void test_tgamma_ratio(T, const char* name)
{
   //
   // The actual test data is rather verbose, so it's in a separate file
   //
#  include "tgamma_delta_ratio_data.ipp"

   do_test_tgamma_delta_ratio(tgamma_delta_ratio_data, name, "tgamma + small delta ratios");

#  include "tgamma_delta_ratio_int.ipp"

   do_test_tgamma_delta_ratio(tgamma_delta_ratio_int, name, "tgamma + small integer ratios");

#  include "tgamma_delta_ratio_int2.ipp"

   do_test_tgamma_delta_ratio(tgamma_delta_ratio_int2, name, "integer tgamma ratios");

#  include "tgamma_ratio_data.ipp"

   do_test_tgamma_ratio(tgamma_ratio_data, name, "tgamma ratios");

}

