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
#define BOOST_TEST_MODULE "Boost Dynamic Bloom Filter" 1
#include <iostream>

#include <boost/bloom_filter/dynamic_bloom_filter.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

using boost::bloom_filters::dynamic_bloom_filter;
using boost::bloom_filters::boost_hash;
using boost::bloom_filters::detail::incompatible_size_exception;

BOOST_AUTO_TEST_CASE(defaultConstructor) {
  typedef boost::mpl::vector<
    boost_hash<int, 13>,
    boost_hash<int, 17>,
    boost_hash<int, 19> > BoostHashFunctions;
  
  dynamic_bloom_filter<int> bloom1;
  dynamic_bloom_filter<int, BoostHashFunctions> bloom2;
}

BOOST_AUTO_TEST_CASE(rangeConstructor) {
  int elems[5] = {1,2,3,4,5};
  dynamic_bloom_filter<int> bloom(elems, elems+5);

  BOOST_CHECK_EQUAL(bloom.count(), 5ul);
}

BOOST_AUTO_TEST_CASE(copyConstructor) {
  int elems[5] = {1,2,3,4,5};
  dynamic_bloom_filter<int> bloom1(elems, elems+5);
  dynamic_bloom_filter<int> bloom2(bloom1);

  BOOST_CHECK_EQUAL(bloom1.count(), bloom2.count());
}


BOOST_AUTO_TEST_CASE(assignment)
{
  dynamic_bloom_filter<int> bloom1(200);
  dynamic_bloom_filter<int> bloom2(200);

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
  dynamic_bloom_filter<size_t> bloom_8(8);
  dynamic_bloom_filter<size_t> bloom_256(256);
  dynamic_bloom_filter<size_t> bloom_2048(2048);
  
  BOOST_CHECK_EQUAL(bloom_8.bit_capacity(), 8ul);
  BOOST_CHECK_EQUAL(bloom_256.bit_capacity(), 256ul);
  BOOST_CHECK_EQUAL(bloom_2048.bit_capacity(), 2048ul);
}

BOOST_AUTO_TEST_CASE(empty) {
  dynamic_bloom_filter<size_t> bloom(8);
  
  BOOST_CHECK_EQUAL(bloom.empty(), true);
  bloom.insert(1);
  BOOST_CHECK_EQUAL(bloom.empty(), false);
  bloom.clear();
  BOOST_CHECK_EQUAL(bloom.empty(), true);
}

BOOST_AUTO_TEST_CASE(numHashFunctions) {
  dynamic_bloom_filter<size_t> bloom_1;
  dynamic_bloom_filter<size_t, boost::mpl::vector<
    boost_hash<size_t, 1>,
    boost_hash<size_t, 2> > > bloom_2;
  dynamic_bloom_filter<size_t, boost::mpl::vector<
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
  dynamic_bloom_filter<size_t> bloom(64);

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

  for (size_t i = 7; i < 5000; ++i)
    bloom.insert(i);
  
  BOOST_CHECK_GE(bloom.false_positive_rate(), 0.6);
  BOOST_CHECK_LE(bloom.false_positive_rate(), 1.0);
}

BOOST_AUTO_TEST_CASE(probably_contains) {
  dynamic_bloom_filter<size_t> bloom(8);

  bloom.insert(1);
  BOOST_CHECK_EQUAL(bloom.probably_contains(1), true);
  BOOST_CHECK_LE(bloom.count(), 3ul);
  BOOST_CHECK_GE(bloom.count(), 1ul);
}

BOOST_AUTO_TEST_CASE(doesNotContain) {
  dynamic_bloom_filter<size_t> bloom(8);

  BOOST_CHECK_EQUAL(bloom.probably_contains(1), false);
}

BOOST_AUTO_TEST_CASE(insertNoFalseNegatives) {
  dynamic_bloom_filter<size_t> bloom(2048);

  for (size_t i = 0; i < 100; ++i) {
    bloom.insert(i);
    BOOST_CHECK_EQUAL(bloom.probably_contains(i), true);
  }
}

BOOST_AUTO_TEST_CASE(rangeInsert) {
  int elems[5] = {1,2,3,4,5};
  dynamic_bloom_filter<size_t> bloom(8);

  bloom.insert(elems, elems+5);
  BOOST_CHECK_EQUAL(bloom.count(), 5ul);
}

BOOST_AUTO_TEST_CASE(clear) {
  dynamic_bloom_filter<size_t> bloom(8);

  for (size_t i = 0; i < 1000; ++i)
    bloom.insert(i);

  bloom.clear();
  BOOST_CHECK_EQUAL(bloom.probably_contains(1), false);
  BOOST_CHECK_EQUAL(bloom.count(), 0ul);
}

struct SwapFixture {
  SwapFixture() 
    : bloom1(10), bloom2(20)
  {
    for (size_t i = 0; i < 5; ++i)
      elems[i] = i+1;
    
    bloom1.insert(elems, elems+2);
    bloom2.insert(elems+2, elems+5);
  }

  dynamic_bloom_filter<size_t> bloom1;
  dynamic_bloom_filter<size_t> bloom2;  
  size_t elems[5];
};

BOOST_FIXTURE_TEST_CASE(memberSwap, SwapFixture) {
  bloom1.swap(bloom2);

  BOOST_CHECK_EQUAL(bloom1.count(), 3ul);
  BOOST_CHECK_EQUAL(bloom1.bit_capacity(), 20ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 2ul);
  BOOST_CHECK_EQUAL(bloom2.bit_capacity(), 10ul);
}

BOOST_FIXTURE_TEST_CASE(globalSwap, SwapFixture) {
  swap(bloom1, bloom2);

  BOOST_CHECK_EQUAL(bloom1.count(), 3ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 2ul);
}

struct PairwiseOpsFixture {
  PairwiseOpsFixture() :
    bloom1(100), bloom2(100), bloom_result(100)
  {
  }

  dynamic_bloom_filter<size_t> bloom1;
  dynamic_bloom_filter<size_t> bloom2;
  dynamic_bloom_filter<size_t> bloom_result;
};

BOOST_FIXTURE_TEST_CASE(testUnion, PairwiseOpsFixture) {
  for (size_t i = 0; i < 50; ++i)
    bloom1.insert(i);

  for (size_t i = 50; i < 100; ++i)
    bloom2.insert(i);

  bloom_result = bloom1 | bloom2;

  for (size_t i = 0; i < 100; ++i)
    BOOST_CHECK_EQUAL(bloom_result.probably_contains(i), true);

  BOOST_CHECK_GE(bloom_result.count(), bloom1.count());
  BOOST_CHECK_GE(bloom_result.count(), bloom2.count());
}

BOOST_FIXTURE_TEST_CASE(testUnionAssign, PairwiseOpsFixture) {
  for (size_t i = 0; i < 100; ++i) 
    bloom1.insert(i);

  bloom_result |= bloom1;

  for (size_t i = 0; i < 100; ++i)
    BOOST_CHECK_EQUAL(bloom_result.probably_contains(i), true);

  BOOST_CHECK_EQUAL(bloom_result.count(), bloom1.count());
}

BOOST_FIXTURE_TEST_CASE(testIntersect, PairwiseOpsFixture) {
  // overlap at 50
  for (size_t i = 0; i < 51; ++i) 
    bloom1.insert(i);
  
  for (size_t i = 50; i < 100; ++i) 
    bloom2.insert(i);

  bloom_result = bloom1 & bloom2;

  BOOST_CHECK_LE(bloom_result.count(), bloom1.count());
  BOOST_CHECK_LE(bloom_result.count(), bloom2.count());
  BOOST_CHECK_LE(bloom_result.count(), 1ul);
  BOOST_CHECK_EQUAL(bloom_result.probably_contains(50), true);
}

BOOST_FIXTURE_TEST_CASE(testIntersectAssign, PairwiseOpsFixture) {
  for (size_t i = 0; i < 100; ++i) 
    bloom1.insert(i);
  
  bloom_result &= bloom1;

  for (size_t i = 0; i < 100; ++i)
    BOOST_CHECK_EQUAL(bloom_result.probably_contains(i), false);
}

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

struct IncompatibleSizeFixture {
  IncompatibleSizeFixture() : 
    bloom1(100), bloom2(101), exception_thrown(false)
  {}

  dynamic_bloom_filter<size_t> bloom1;
  dynamic_bloom_filter<size_t> bloom2;
  dynamic_bloom_filter<size_t> bloom3;  
  bool exception_thrown;
};

BOOST_FIXTURE_TEST_CASE(_intersectException, IncompatibleSizeFixture) {
  try {
    bloom3 = bloom1 & bloom2;
  }
  catch (incompatible_size_exception e) {
    exception_thrown = true;
  }

  BOOST_CHECK_EQUAL(exception_thrown, true);
}

BOOST_FIXTURE_TEST_CASE(_unionException, IncompatibleSizeFixture) {
  try {
    bloom3 = bloom1 | bloom2;
  }
  catch (incompatible_size_exception e) {
    exception_thrown = true;
  }

  BOOST_CHECK_EQUAL(exception_thrown, true);
}

BOOST_FIXTURE_TEST_CASE(_equalityException, IncompatibleSizeFixture) {
  try {
    if (bloom1 == bloom2)
      exception_thrown = false;
  }
  catch (incompatible_size_exception e) {
    exception_thrown = true;
  }

  BOOST_CHECK_EQUAL(exception_thrown, true);
}
