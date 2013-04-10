//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Alejandro Cabrera 2011.
// Distributed under the Boost
// Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or
// copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/bloom_filter for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#define BOOST_TEST_DYN_LINK 1
#define BOOST_TEST_MODULE "Boost Bloom Filter" 1

#include <boost/bloom_filter/dynamic_counting_bloom_filter.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <boost/bloom_filter/detail/exceptions.hpp>

#include <boost/cstdint.hpp>

using boost::bloom_filters::dynamic_counting_bloom_filter;
using boost::bloom_filters::detail::bin_underflow_exception;
using boost::bloom_filters::detail::bin_overflow_exception;
using boost::bloom_filters::detail::incompatible_size_exception;
using boost::bloom_filters::boost_hash;

BOOST_AUTO_TEST_CASE(allBitsPerBinCompile)
{
  dynamic_counting_bloom_filter<size_t, 1> bloom1;
  dynamic_counting_bloom_filter<size_t, 2> bloom2;
  dynamic_counting_bloom_filter<size_t, 4> bloom4;
  dynamic_counting_bloom_filter<size_t, 8> bloom8;
  dynamic_counting_bloom_filter<size_t, 16> bloom16;
  dynamic_counting_bloom_filter<size_t, 32> bloom32;
}

BOOST_AUTO_TEST_CASE(allReasonableBlockTypesCompile)
{
  typedef boost::mpl::vector<boost_hash<int, 0> > default_hash;

  dynamic_counting_bloom_filter<int, 2, default_hash, unsigned char> a;
  dynamic_counting_bloom_filter<int, 2, default_hash, unsigned short> b;
  dynamic_counting_bloom_filter<int, 2, default_hash, unsigned int> c;
  dynamic_counting_bloom_filter<int, 2, default_hash, unsigned long> d;
  dynamic_counting_bloom_filter<int, 2, default_hash, size_t> e;

  dynamic_counting_bloom_filter<int, 2, default_hash, uint8_t> aa;
  dynamic_counting_bloom_filter<int, 2, default_hash, uint16_t> ab;
  dynamic_counting_bloom_filter<int, 2, default_hash, uint32_t> ac;
  dynamic_counting_bloom_filter<int, 2, default_hash, uintmax_t> ad;
}

BOOST_AUTO_TEST_CASE(defaultConstructor) {
  typedef boost::mpl::vector<
    boost_hash<int, 13>,
    boost_hash<int, 17>,
    boost_hash<int, 19> > BoostHashFunctions;
  
  dynamic_counting_bloom_filter<int, 2> bloom1;
  dynamic_counting_bloom_filter<int, 2, BoostHashFunctions> bloom2;
  dynamic_counting_bloom_filter<int, 2, 
				BoostHashFunctions, 
				unsigned char> bloom3;
  dynamic_counting_bloom_filter<int, 2, 
				BoostHashFunctions, 
				unsigned char,
				std::allocator<unsigned char> > bloom4;
}

BOOST_AUTO_TEST_CASE(countSingle)
{
  dynamic_counting_bloom_filter<int> bloom(2);
  
  bloom.insert(1);
  BOOST_CHECK_EQUAL(bloom.count(), 1ul);
  bloom.remove(1);
  BOOST_CHECK_EQUAL(bloom.count(), 0ul);
}

BOOST_AUTO_TEST_CASE(countMulti)
{
  dynamic_counting_bloom_filter<int> bloom_default(100);
  dynamic_counting_bloom_filter<int, 1> bloom1(100);
  dynamic_counting_bloom_filter<int, 2> bloom2(100);
  dynamic_counting_bloom_filter<int, 8> bloom8(100);
  dynamic_counting_bloom_filter<int, 16> bloom16(100);
  dynamic_counting_bloom_filter<int, 32> bloom32(100);

  for (size_t i = 0; i < 100; ++i) {
    bloom_default.insert(i);
    bloom1.insert(i);
    bloom2.insert(i);
    bloom8.insert(i);
    bloom16.insert(i);
    bloom32.insert(i);
  }

  BOOST_CHECK_EQUAL(bloom_default.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom1.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom8.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom16.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom32.count(), 100ul);

  for (size_t i = 0; i < 100; ++i) {
    bloom_default.insert(i);
    bloom2.insert(i);
    bloom8.insert(i);
    bloom16.insert(i);
    bloom32.insert(i);
  }

  BOOST_CHECK_EQUAL(bloom_default.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom8.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom16.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom32.count(), 100ul);

  for (size_t i = 0; i < 100; ++i) {
    bloom_default.remove(i);
    bloom2.remove(i);
    bloom8.remove(i);
    bloom16.remove(i);
    bloom32.remove(i);
  }

  BOOST_CHECK_EQUAL(bloom_default.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom8.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom16.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom32.count(), 100ul);

  for (size_t i = 0; i < 100; ++i) {
    bloom_default.remove(i);
    bloom1.remove(i);
    bloom2.remove(i);
    bloom8.remove(i);
    bloom16.remove(i);
    bloom32.remove(i);
  }

  BOOST_CHECK_EQUAL(bloom_default.count(), 0ul);
  BOOST_CHECK_EQUAL(bloom1.count(), 0ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 0ul);
  BOOST_CHECK_EQUAL(bloom8.count(), 0ul);
  BOOST_CHECK_EQUAL(bloom16.count(), 0ul);
  BOOST_CHECK_EQUAL(bloom32.count(), 0ul);
}

BOOST_AUTO_TEST_CASE(rangeConstructor) {
  int elems[5] = {1,2,3,4,5};
  dynamic_counting_bloom_filter<int> bloom(elems, elems+5);

  BOOST_CHECK_EQUAL(bloom.count(), 5ul);
}

BOOST_AUTO_TEST_CASE(copyConstructor) {
  int elems[5] = {1,2,3,4,5};
  dynamic_counting_bloom_filter<int, 2> bloom1(elems, elems+5);
  dynamic_counting_bloom_filter<int, 2> bloom2(bloom1);

  BOOST_CHECK_EQUAL(bloom1.count(), bloom2.count());
}

BOOST_AUTO_TEST_CASE(assignment)
{
  dynamic_counting_bloom_filter<int> bloom1(200);
  dynamic_counting_bloom_filter<int> bloom2(200);

  for (size_t i = 0; i < 200; ++i) {
    bloom1.insert(i);
    BOOST_CHECK_EQUAL(bloom1.probably_contains(i), true);
  }

  bloom2 = bloom1;

  for (size_t i = 0; i < 200; ++i) {
    BOOST_CHECK_EQUAL(bloom2.probably_contains(i), true);
  }
}

BOOST_AUTO_TEST_CASE(bit_capacity) {
  dynamic_counting_bloom_filter<size_t> bloom_8(8);
  dynamic_counting_bloom_filter<size_t> bloom_256(256);
  dynamic_counting_bloom_filter<size_t> bloom_2048(2048);
  
  BOOST_CHECK_EQUAL(bloom_8.bit_capacity(), 
		    8ul * bloom_8.bits_per_bin());
  BOOST_CHECK_EQUAL(bloom_256.bit_capacity(), 
		    256ul * bloom_256.bits_per_bin());
  BOOST_CHECK_EQUAL(bloom_2048.bit_capacity(), 
		    2048ul * bloom_2048.bits_per_bin());
}

BOOST_AUTO_TEST_CASE(empty) {
  dynamic_counting_bloom_filter<size_t> bloom(2);
  
  BOOST_CHECK_EQUAL(bloom.empty(), true);
  bloom.insert(1);
  BOOST_CHECK_EQUAL(bloom.empty(), false);
  bloom.clear();
  BOOST_CHECK_EQUAL(bloom.empty(), true);
}

BOOST_AUTO_TEST_CASE(numHashFunctions) {
  dynamic_counting_bloom_filter<size_t> bloom_1;
  dynamic_counting_bloom_filter<size_t, 1, boost::mpl::vector<
    boost_hash<size_t, 1>,
    boost_hash<size_t, 2> > > bloom_2;
  dynamic_counting_bloom_filter<size_t, 1, boost::mpl::vector<
    boost_hash<size_t, 1>,
    boost_hash<size_t, 2>,
    boost_hash<size_t, 3>,
    boost_hash<size_t, 4>,
    boost_hash<size_t, 5>,
    boost_hash<size_t, 6>,
    boost_hash<size_t, 7> > > bloom_7;

  BOOST_CHECK_EQUAL(bloom_1.num_hash_functions(), 1ul);
  BOOST_CHECK_EQUAL(bloom_2.num_hash_functions(), 2ul);
  BOOST_CHECK_EQUAL(bloom_7.num_hash_functions(), 7ul);
}

BOOST_AUTO_TEST_CASE(falsePositiveRate) {
  dynamic_counting_bloom_filter<size_t> bloom(64);

  BOOST_CHECK_EQUAL(bloom.false_positive_rate(), 0.0);

  bloom.insert(1);
  BOOST_CHECK_CLOSE(bloom.false_positive_rate(), 0.015504, .01);

  bloom.insert(2);
  BOOST_CHECK_CLOSE(bloom.false_positive_rate(), 0.030768, .01);

  bloom.insert(3);
  BOOST_CHECK_CLOSE(bloom.false_positive_rate(), 0.045794, .01);

  bloom.insert(4);
  BOOST_CHECK_CLOSE(bloom.false_positive_rate(), 0.060588, .01);

  bloom.insert(5);
  BOOST_CHECK_CLOSE(bloom.false_positive_rate(), 0.075151, .01);

  bloom.insert(6);
  BOOST_CHECK_CLOSE(bloom.false_positive_rate(), 0.089491, .01);

  for (size_t i = 7; i < 128; ++i)
    bloom.insert(i);
  
  BOOST_CHECK_GE(bloom.false_positive_rate(), 0.6);
  BOOST_CHECK_LE(bloom.false_positive_rate(), 1.0);
}

BOOST_AUTO_TEST_CASE(probably_contains) {
  dynamic_counting_bloom_filter<size_t> bloom(2);

  bloom.insert(1);
  BOOST_CHECK_EQUAL(bloom.probably_contains(1), true);
  BOOST_CHECK_EQUAL(bloom.count(), 1ul);
}

BOOST_AUTO_TEST_CASE(doesNotContain) {
  dynamic_counting_bloom_filter<size_t> bloom(2);

  BOOST_CHECK_EQUAL(bloom.probably_contains(1), false);
}

BOOST_AUTO_TEST_CASE(insertNoFalseNegatives) {
  dynamic_counting_bloom_filter<size_t> bloom(100);

  for (size_t i = 0; i < 100; ++i) {
    bloom.insert(i);
    BOOST_CHECK_EQUAL(bloom.probably_contains(i), true);
  }
}

BOOST_AUTO_TEST_CASE(insertOverflowExceptionThrown) {
  dynamic_counting_bloom_filter<size_t, 1> bloom(2);
  bool exception_occurred = false;

  bloom.insert(1);
  BOOST_CHECK_EQUAL(bloom.probably_contains(1), true);

  try {
    bloom.insert(1);
  }

  catch (bin_overflow_exception e) {
    exception_occurred = true;
  }

  BOOST_CHECK_EQUAL(exception_occurred, true);
  BOOST_CHECK_EQUAL(bloom.probably_contains(1), true);
}

BOOST_AUTO_TEST_CASE(removeUnderflowExceptionThrown) {
  dynamic_counting_bloom_filter<size_t, 1> bloom(2);
  bool exception_occurred = false;

  try {
    bloom.remove(1);
  }

  catch (bin_underflow_exception e) {
    exception_occurred = true;
  }

  BOOST_CHECK_EQUAL(exception_occurred, true);
  BOOST_CHECK_EQUAL(bloom.probably_contains(1), false);
}

BOOST_AUTO_TEST_CASE(insertOverflowException4bit)
{
  dynamic_counting_bloom_filter<size_t> bloom(8);
  bool exception_occurred = false;

  try {
    for (size_t i = 0; i < (1ul << bloom.bits_per_bin()); ++i)
      bloom.insert(1);
  }

  catch (bin_overflow_exception e) {
    exception_occurred = true;
  }

  BOOST_CHECK_EQUAL(exception_occurred, true);
  BOOST_CHECK_EQUAL(bloom.probably_contains(1), true);
}

BOOST_AUTO_TEST_CASE(incompatibleSizeExceptionThrown_equality)
{
  dynamic_counting_bloom_filter<int> bloom1(1);
  dynamic_counting_bloom_filter<int> bloom2(2);
  bool exception_thrown = false;

  try {
    if (bloom1 == bloom2)
      exception_thrown = false;
  }

  catch (incompatible_size_exception e) {
    exception_thrown = true;
  }

  BOOST_CHECK_EQUAL(exception_thrown, true);
}

BOOST_AUTO_TEST_CASE(incompatibleSizeExceptionThrown_inequality)
{
  dynamic_counting_bloom_filter<int> bloom1(1);
  dynamic_counting_bloom_filter<int> bloom2(2);
  bool exception_thrown = false;

  try {
    if (bloom1 != bloom2)
      exception_thrown = false;
  }

  catch (incompatible_size_exception e) {
    exception_thrown = true;
  }

  BOOST_CHECK_EQUAL(exception_thrown, true);
}

BOOST_AUTO_TEST_CASE(rangeInsert) {
  int elems[5] = {1,2,3,4,5};
  dynamic_counting_bloom_filter<size_t> bloom(5);

  bloom.insert(elems, elems+5);
  BOOST_CHECK_EQUAL(bloom.count(), 5ul);
}

BOOST_AUTO_TEST_CASE(_remove) {
  dynamic_counting_bloom_filter<size_t> bloom(1);
  
  bloom.insert(1);
  bloom.remove(1);
  BOOST_CHECK_EQUAL(bloom.count(), 0ul);
}

BOOST_AUTO_TEST_CASE(removeMulti) {
  dynamic_counting_bloom_filter<size_t> bloom(100);

  for (size_t i = 0; i < 100; ++i) {
    bloom.insert(i);
    bloom.remove(i);
    BOOST_CHECK_EQUAL(bloom.count(), 0ul);
  }
}

BOOST_AUTO_TEST_CASE(rangeRemove) {
  size_t arr[] = {1,2,3,4,5};
  dynamic_counting_bloom_filter<size_t> bloom(5);

  bloom.insert(arr, arr+5);
  bloom.remove(arr, arr+5);
  BOOST_CHECK_EQUAL(bloom.count(), 0ul);
}

BOOST_AUTO_TEST_CASE(clear) {
  dynamic_counting_bloom_filter<size_t> bloom(100);

  for (size_t i = 0; i < 100; ++i)
    bloom.insert(i);

  bloom.clear();
  BOOST_CHECK_EQUAL(bloom.probably_contains(1), false);
  BOOST_CHECK_EQUAL(bloom.count(), 0ul);
}

struct SwapFixture {
  SwapFixture() 
    : bloom1(5), bloom2(5)
  {
    for (size_t i = 0; i < 5; ++i)
      elems[i] = i+1;
    
    bloom1.insert(elems, elems+2);
    bloom2.insert(elems+2, elems+5);
  }

  size_t elems[5];
  dynamic_counting_bloom_filter<size_t> bloom1;
  dynamic_counting_bloom_filter<size_t> bloom2;
};

BOOST_FIXTURE_TEST_CASE(memberSwap, SwapFixture) {
  bloom1.swap(bloom2);

  BOOST_CHECK_EQUAL(bloom1.count(), 3ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 2ul);
}

BOOST_FIXTURE_TEST_CASE(globalSwap, SwapFixture) {
  swap(bloom1, bloom2);

  BOOST_CHECK_EQUAL(bloom1.count(), 3ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 2ul);
}

struct PairwiseOpsFixture {

  PairwiseOpsFixture() 
    : bloom1(300), bloom2(300), bloom_result(300)
  {}

  dynamic_counting_bloom_filter<size_t> bloom1;
  dynamic_counting_bloom_filter<size_t> bloom2;
  dynamic_counting_bloom_filter<size_t> bloom_result;
};

BOOST_FIXTURE_TEST_CASE(equalityOperator, PairwiseOpsFixture) {
  BOOST_CHECK_EQUAL(bloom1 == bloom2, true);
  bloom1.insert(1);
  BOOST_CHECK_EQUAL(bloom1 == bloom2, false);
  bloom2.insert(1);
  BOOST_CHECK_EQUAL(bloom1 == bloom2, true);
}

BOOST_FIXTURE_TEST_CASE(inequalityOperator, PairwiseOpsFixture) {
  BOOST_CHECK_EQUAL(bloom1 != bloom2, false);
  bloom1.insert(1);
  BOOST_CHECK_EQUAL(bloom1 != bloom2, true);
  bloom2.insert(1);
  BOOST_CHECK_EQUAL(bloom1 != bloom2, false);
}
