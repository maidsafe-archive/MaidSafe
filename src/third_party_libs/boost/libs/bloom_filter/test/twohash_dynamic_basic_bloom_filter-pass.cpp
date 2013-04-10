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

#include <boost/bloom_filter/twohash_dynamic_basic_bloom_filter.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

using boost::bloom_filters::twohash_dynamic_basic_bloom_filter;
using boost::bloom_filters::boost_hash;
using boost::bloom_filters::murmurhash3;
using boost::bloom_filters::detail::cube;
using boost::bloom_filters::detail::incompatible_size_exception;

BOOST_AUTO_TEST_CASE(defaultConstructor) {
  twohash_dynamic_basic_bloom_filter<int> bloom;
  twohash_dynamic_basic_bloom_filter<int, 16> sixteen_hash_value_bloom;
  twohash_dynamic_basic_bloom_filter<int, 
				     16, 2> two_expected_insertion_count_bloom;
  twohash_dynamic_basic_bloom_filter<int, 16, 0, 
			     murmurhash3<int> > all_murmurhash_bloom;

  twohash_dynamic_basic_bloom_filter<int, 16, 0,
			     murmurhash3<int>,
			     boost_hash<int, 0> > hash_configured_bloom;

  twohash_dynamic_basic_bloom_filter<int, 16, 0,
			     murmurhash3<int>,
			     boost_hash<int, 0>, cube> cube_extender_bloom;
}

BOOST_AUTO_TEST_CASE(checkBlockTypes)
{
  twohash_dynamic_basic_bloom_filter<int, 2, 0,
				     boost_hash<int, 0>,
				     murmurhash3<int>,
				     cube, size_t> size_t_bloom;

  twohash_dynamic_basic_bloom_filter<int, 2, 0,
				     boost_hash<int, 0>,
				     murmurhash3<int>,
				     cube, unsigned char> uchar_bloom;
  
  twohash_dynamic_basic_bloom_filter<int, 2, 0,
				     boost_hash<int, 0>,
				     murmurhash3<int>,
				     cube, unsigned short> ushort_bloom;

  twohash_dynamic_basic_bloom_filter<int, 2, 0,
				     boost_hash<int, 0>,
				     murmurhash3<int>,
				     cube, unsigned int> uint_bloom;

  twohash_dynamic_basic_bloom_filter<int, 2, 0,
				     boost_hash<int, 0>,
				     murmurhash3<int>,
				     cube, unsigned long> ulong_bloom;
}

BOOST_AUTO_TEST_CASE(checkExtenders)
{
  using boost::bloom_filters::detail::zero;
  using boost::bloom_filters::detail::square;
  using boost::bloom_filters::detail::fourth;

  twohash_dynamic_basic_bloom_filter<int, 2, 0, 
			     boost_hash<int, 0>, 
			     murmurhash3<int>, 
			     zero> zero_extend_bloom;

  twohash_dynamic_basic_bloom_filter<int, 2, 0, 
			     boost_hash<int, 0>, 
			     murmurhash3<int>, 
			     square> square_extend_bloom;

  twohash_dynamic_basic_bloom_filter<int, 2, 0, 
			     boost_hash<int, 0>, 
			     murmurhash3<int>, 
			     fourth> fourth_extend_bloom;
}

BOOST_AUTO_TEST_CASE(sizeConstructor)
{
  twohash_dynamic_basic_bloom_filter<int> bloom(1);
  twohash_dynamic_basic_bloom_filter<int> bloom20(20);
  twohash_dynamic_basic_bloom_filter<int> bloom200(200);
}

BOOST_AUTO_TEST_CASE(rangeConstructor) {
  int elems[5] = {1,2,3,4,5};
  twohash_dynamic_basic_bloom_filter<int, 1> bloom(elems, elems+5);

  BOOST_CHECK_EQUAL(bloom.count(), 5ul);
}

BOOST_AUTO_TEST_CASE(copyConstructor) {
  int elems[5] = {1,2,3,4,5};
  twohash_dynamic_basic_bloom_filter<int> bloom1(elems, elems+5);
  twohash_dynamic_basic_bloom_filter<int> bloom2(bloom1);

  BOOST_CHECK_EQUAL(bloom1.count(), bloom2.count());
}

BOOST_AUTO_TEST_CASE(assignment)
{
  twohash_dynamic_basic_bloom_filter<int> bloom1(8);
  twohash_dynamic_basic_bloom_filter<int> bloom2(8);

  for (size_t i = 0; i < 8; ++i) {
    bloom1.insert(i);
    BOOST_CHECK_EQUAL(bloom1.probably_contains(i), true);
  }

  bloom2 = bloom1;

  for (size_t i = 0; i < 8; ++i) {
    BOOST_CHECK_EQUAL(bloom2.probably_contains(i), true);
  }
}

BOOST_AUTO_TEST_CASE(bit_capacity) {
  twohash_dynamic_basic_bloom_filter<size_t> bloom_8(8);
  twohash_dynamic_basic_bloom_filter<size_t> bloom_256(256);
  twohash_dynamic_basic_bloom_filter<size_t> bloom_2048(2048);
  
  BOOST_CHECK_EQUAL(bloom_8.bit_capacity(), 8ul);
  BOOST_CHECK_EQUAL(bloom_256.bit_capacity(), 256ul);
  BOOST_CHECK_EQUAL(bloom_2048.bit_capacity(), 2048ul);
}

BOOST_AUTO_TEST_CASE(num_hash_values)
{
  twohash_dynamic_basic_bloom_filter<size_t, 8> bloom_8h;
  twohash_dynamic_basic_bloom_filter<size_t, 256> bloom_256h;
  twohash_dynamic_basic_bloom_filter<size_t, 2048> bloom_2048h;
  
  BOOST_CHECK_EQUAL(bloom_8h.num_hash_functions(), 8ul);
  BOOST_CHECK_EQUAL(bloom_256h.num_hash_functions(), 256ul);
  BOOST_CHECK_EQUAL(bloom_2048h.num_hash_functions(), 2048ul);
}

BOOST_AUTO_TEST_CASE(expected_insertion_count)
{
  twohash_dynamic_basic_bloom_filter<size_t, 2, 8> bloom_8i;
  twohash_dynamic_basic_bloom_filter<size_t, 2, 256> bloom_256i;
  twohash_dynamic_basic_bloom_filter<size_t, 2, 2048> bloom_2048i;
  
  BOOST_CHECK_EQUAL(bloom_8i.expected_insertion_count(), 8ul);
  BOOST_CHECK_EQUAL(bloom_256i.expected_insertion_count(), 256ul);
  BOOST_CHECK_EQUAL(bloom_2048i.expected_insertion_count(), 2048ul);
}

BOOST_AUTO_TEST_CASE(empty) {
  twohash_dynamic_basic_bloom_filter<size_t> bloom(8);
  
  BOOST_CHECK_EQUAL(bloom.empty(), true);
  bloom.insert(1);
  BOOST_CHECK_EQUAL(bloom.empty(), false);
  bloom.clear();
  BOOST_CHECK_EQUAL(bloom.empty(), true);
}

BOOST_AUTO_TEST_CASE(falsePositiveRate) {
  twohash_dynamic_basic_bloom_filter<size_t> bloom(64);

  BOOST_CHECK_EQUAL(bloom.false_positive_rate(), 0.0);

  bloom.insert(1);
  BOOST_CHECK_CLOSE(bloom.false_positive_rate(), 0.0036708, .01);

  bloom.insert(2);
  BOOST_CHECK_CLOSE(bloom.false_positive_rate(), 0.013807, .01); 
  bloom.insert(3);
  BOOST_CHECK_CLOSE(bloom.false_positive_rate(), 0.029231, .01);

  bloom.insert(4);
  BOOST_CHECK_CLOSE(bloom.false_positive_rate(), 0.048929, .01);

  bloom.insert(5);
  BOOST_CHECK_CLOSE(bloom.false_positive_rate(), 0.072030, .01);

  bloom.insert(6);
  BOOST_CHECK_CLOSE(bloom.false_positive_rate(), 0.084619, .01);

  for (size_t i = 7; i < 5000; ++i)
    bloom.insert(i);
  
  BOOST_CHECK_GE(bloom.false_positive_rate(), 0.6);
  BOOST_CHECK_LE(bloom.false_positive_rate(), 1.0);
}

BOOST_AUTO_TEST_CASE(probably_contains) {
  twohash_dynamic_basic_bloom_filter<size_t> bloom(8);

  bloom.insert(1);
  BOOST_CHECK_EQUAL(bloom.probably_contains(1), true);
  BOOST_CHECK_LE(bloom.count(), 3ul);
  BOOST_CHECK_GE(bloom.count(), 1ul);
}

BOOST_AUTO_TEST_CASE(doesNotContain) {
  twohash_dynamic_basic_bloom_filter<size_t> bloom(8);

  BOOST_CHECK_EQUAL(bloom.probably_contains(1), false);
}

BOOST_AUTO_TEST_CASE(insertNoFalseNegatives) {
  twohash_dynamic_basic_bloom_filter<size_t> bloom(2048);

  for (size_t i = 0; i < 100; ++i) {
    bloom.insert(i);
    BOOST_CHECK_EQUAL(bloom.probably_contains(i), true);
  }
}

BOOST_AUTO_TEST_CASE(rangeInsert) {
  int elems[5] = {1,2,3,4,5};
  twohash_dynamic_basic_bloom_filter<size_t, 1> bloom(8);

  bloom.insert(elems, elems+5);
  BOOST_CHECK_EQUAL(bloom.count(), 5ul);
}

BOOST_AUTO_TEST_CASE(clear) {
  twohash_dynamic_basic_bloom_filter<size_t> bloom(8);

  for (size_t i = 0; i < 1000; ++i)
    bloom.insert(i);

  bloom.clear();
  BOOST_CHECK_EQUAL(bloom.probably_contains(1), false);
  BOOST_CHECK_EQUAL(bloom.count(), 0ul);
}

BOOST_AUTO_TEST_CASE(memberSwap) {
  size_t elems[5] = {1,2,3,4,5};
  twohash_dynamic_basic_bloom_filter<size_t, 1> bloom1(elems, elems+2);
  twohash_dynamic_basic_bloom_filter<size_t, 1> bloom2(elems+2, elems+5);

  bloom1.swap(bloom2);

  BOOST_CHECK_EQUAL(bloom1.count(), 3ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 2ul);
}

BOOST_AUTO_TEST_CASE(testUnion) {
  twohash_dynamic_basic_bloom_filter<size_t> bloom_1(300);
  twohash_dynamic_basic_bloom_filter<size_t> bloom_2(300);
  twohash_dynamic_basic_bloom_filter<size_t> bloom_union(300);

  for (size_t i = 0; i < 100; ++i)
    bloom_1.insert(i);

  for (size_t i = 100; i < 200; ++i)
    bloom_2.insert(i);

  bloom_union = bloom_1 | bloom_2;

  for (size_t i = 0; i < 200; ++i)
    BOOST_CHECK_EQUAL(bloom_union.probably_contains(i), true);
  BOOST_CHECK_GE(bloom_union.count(), bloom_1.count());
  BOOST_CHECK_GE(bloom_union.count(), bloom_2.count());
}

BOOST_AUTO_TEST_CASE(testUnionAssign) {
  twohash_dynamic_basic_bloom_filter<size_t> bloom_1(300);
  twohash_dynamic_basic_bloom_filter<size_t> bloom_union(300);

  for (size_t i = 0; i < 100; ++i) 
    bloom_1.insert(i);

  bloom_union |= bloom_1;

  for (size_t i = 0; i < 100; ++i)
    BOOST_CHECK_EQUAL(bloom_union.probably_contains(i), true);
  BOOST_CHECK_EQUAL(bloom_union.count(), bloom_1.count());
}

BOOST_AUTO_TEST_CASE(testIntersect) {
  twohash_dynamic_basic_bloom_filter<size_t> bloom_1(300);
  twohash_dynamic_basic_bloom_filter<size_t> bloom_2(300);
  twohash_dynamic_basic_bloom_filter<size_t> bloom_intersect(300);

  // overlap at 100
  for (size_t i = 0; i < 101; ++i) 
    bloom_1.insert(i);
  
  for (size_t i = 100; i < 200; ++i) 
    bloom_2.insert(i);

  bloom_intersect = bloom_1 & bloom_2;

  BOOST_CHECK_LE(bloom_intersect.count(), bloom_1.count());
  BOOST_CHECK_LE(bloom_intersect.count(), bloom_2.count());
  BOOST_CHECK_EQUAL(bloom_intersect.probably_contains(100), true);
}

BOOST_AUTO_TEST_CASE(testIntersectAssign) {
  twohash_dynamic_basic_bloom_filter<size_t> bloom_1(300);
  twohash_dynamic_basic_bloom_filter<size_t> bloom_intersect(300);

  for (size_t i = 0; i < 100; ++i) 
    bloom_1.insert(i);
  
  bloom_intersect &= bloom_1;

  for (size_t i = 0; i < 100; ++i)
    BOOST_CHECK_EQUAL(bloom_intersect.probably_contains(i), false);
}

BOOST_AUTO_TEST_CASE(globalSwap) {
  size_t elems[5] = {1,2,3,4,5};
  twohash_dynamic_basic_bloom_filter<size_t, 1> bloom1(elems, elems+2);
  twohash_dynamic_basic_bloom_filter<size_t, 1> bloom2(elems+2, elems+5);

  swap(bloom1, bloom2);

  BOOST_CHECK_EQUAL(bloom1.count(), 3ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 2ul);
}

BOOST_AUTO_TEST_CASE(equalityOperator) {
  twohash_dynamic_basic_bloom_filter<int> bloom1(8);
  twohash_dynamic_basic_bloom_filter<int> bloom2(8);

  BOOST_CHECK_EQUAL(bloom1 == bloom2, true);
  bloom1.insert(1);
  BOOST_CHECK_EQUAL(bloom1 == bloom2, false);
  bloom2.insert(1);
  BOOST_CHECK_EQUAL(bloom1 == bloom2, true);
}

BOOST_AUTO_TEST_CASE(inequalityOperator) {
  twohash_dynamic_basic_bloom_filter<int> bloom1(8);
  twohash_dynamic_basic_bloom_filter<int> bloom2(8);

  BOOST_CHECK_EQUAL(bloom1 != bloom2, false);
  bloom1.insert(1);
  BOOST_CHECK_EQUAL(bloom1 != bloom2, true);
  bloom2.insert(1);
  BOOST_CHECK_EQUAL(bloom1 != bloom2, false);
}

BOOST_AUTO_TEST_CASE(exceptionThrown_union)
{
  twohash_dynamic_basic_bloom_filter<int> left(4);
  twohash_dynamic_basic_bloom_filter<int> right(5);
  bool exceptionThrown = false;

  try {
    left |= right;
  }

  catch (incompatible_size_exception e) {
    exceptionThrown = true;
  }

  BOOST_CHECK_EQUAL(exceptionThrown, true);
}

BOOST_AUTO_TEST_CASE(exceptionThrown_intersect)
{
  twohash_dynamic_basic_bloom_filter<int> left(4);
  twohash_dynamic_basic_bloom_filter<int> right(5);
  bool exceptionThrown = false;

  try {
    left &= right;
  }

  catch (incompatible_size_exception e) {
    exceptionThrown = true;
  }

  BOOST_CHECK_EQUAL(exceptionThrown, true);
}

BOOST_AUTO_TEST_CASE(exceptionThrown_equality)
{
  twohash_dynamic_basic_bloom_filter<int> left(4);
  twohash_dynamic_basic_bloom_filter<int> right(5);
  bool exceptionThrown = false;

  try {
    if (left == right) {
      exceptionThrown = false;
    }
  }

  catch (incompatible_size_exception e) {
    exceptionThrown = true;
  }

  BOOST_CHECK_EQUAL(exceptionThrown, true);
}
