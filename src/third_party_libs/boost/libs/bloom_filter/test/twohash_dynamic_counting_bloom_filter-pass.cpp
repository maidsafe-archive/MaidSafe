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

#include <boost/bloom_filter/twohash_dynamic_counting_bloom_filter.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <boost/bloom_filter/detail/exceptions.hpp>

#include <boost/cstdint.hpp>

using boost::bloom_filters::twohash_dynamic_counting_bloom_filter;
using boost::bloom_filters::detail::bin_underflow_exception;
using boost::bloom_filters::detail::bin_overflow_exception;
using boost::bloom_filters::detail::incompatible_size_exception;
using boost::bloom_filters::boost_hash;
using boost::bloom_filters::murmurhash3;
using boost::bloom_filters::detail::zero;

BOOST_AUTO_TEST_CASE(allBitsPerBinCompile)
{
  twohash_dynamic_counting_bloom_filter<size_t, 1> bloom1;
  twohash_dynamic_counting_bloom_filter<size_t, 2> bloom2;
  twohash_dynamic_counting_bloom_filter<size_t, 4> bloom4;
  twohash_dynamic_counting_bloom_filter<size_t, 8> bloom8;
  twohash_dynamic_counting_bloom_filter<size_t, 16> bloom16;
  twohash_dynamic_counting_bloom_filter<size_t, 32> bloom32;
}

BOOST_AUTO_TEST_CASE(allReasonableBlockTypesCompile)
{
  twohash_dynamic_counting_bloom_filter<int, 2, 4, 0, 
				boost_hash<int, 0>, murmurhash3<int>, 
				zero, unsigned char> a;
  twohash_dynamic_counting_bloom_filter<int, 2, 4, 0, 
				boost_hash<int, 0>, murmurhash3<int>, 
				zero, unsigned short> b;
  twohash_dynamic_counting_bloom_filter<int, 2, 4, 0, 
				boost_hash<int, 0>, murmurhash3<int>, 
				zero, unsigned int> c;
  twohash_dynamic_counting_bloom_filter<int, 2, 4, 0, 
				boost_hash<int, 0>, murmurhash3<int>, 
				zero, unsigned long> d;
  twohash_dynamic_counting_bloom_filter<int, 2, 4, 0, 
				boost_hash<int, 0>, murmurhash3<int>, 
				zero, size_t> e;

  twohash_dynamic_counting_bloom_filter<int, 2, 4, 0, 
				boost_hash<int, 0>, murmurhash3<int>, 
				zero, uint8_t> aa;
  twohash_dynamic_counting_bloom_filter<int, 2, 4, 0, 
				boost_hash<int, 0>, murmurhash3<int>, 
				zero, uint16_t> ab;
  twohash_dynamic_counting_bloom_filter<int, 2, 4, 0, 
				boost_hash<int, 0>, murmurhash3<int>, 
				zero, uint32_t> ac;
  twohash_dynamic_counting_bloom_filter<int, 2, 4, 0, 
				boost_hash<int, 0>, murmurhash3<int>, 
				zero, uintmax_t> ad;
}

BOOST_AUTO_TEST_CASE(defaultConstructor) {
  twohash_dynamic_counting_bloom_filter<int> bloom_default;
  twohash_dynamic_counting_bloom_filter<int, 1> bloom_1bit_per_bin;
  twohash_dynamic_counting_bloom_filter<int, 2, 1, 
				9> bloom_9expected_insertions;
  twohash_dynamic_counting_bloom_filter<int, 2, 1, 9,
				boost_hash<int, 0> > bloom_remixed_hash;
  twohash_dynamic_counting_bloom_filter<int, 2, 1, 9,
				boost_hash<int, 0>, 
				murmurhash3<int> > bloom_very_remixed_hash;
  twohash_dynamic_counting_bloom_filter<int, 2, 1, 9,
				boost_hash<int, 0>, 
				murmurhash3<int>,
				zero> bloom_very_reconfigured;  
  twohash_dynamic_counting_bloom_filter<int, 2, 1, 9,
				boost_hash<int, 0>, 
				murmurhash3<int>,
				zero, uint8_t> bloom_fully_reconfigured;  
}

BOOST_AUTO_TEST_CASE(countSingle)
{
  twohash_dynamic_counting_bloom_filter<int, 4, 1> bloom;
  
  bloom.insert(1);
  BOOST_CHECK_EQUAL(bloom.count(), 1ul);
  bloom.remove(1);
  BOOST_CHECK_EQUAL(bloom.count(), 0ul);
}

BOOST_AUTO_TEST_CASE(countMulti)
{
  twohash_dynamic_counting_bloom_filter<int, 4, 1> bloom_default(400);
  twohash_dynamic_counting_bloom_filter<int, 1, 1> bloom1(400);
  twohash_dynamic_counting_bloom_filter<int, 2, 1> bloom2(400);
  twohash_dynamic_counting_bloom_filter<int, 4, 1> bloom4(400);
  twohash_dynamic_counting_bloom_filter<int, 8, 1> bloom8(400);
  twohash_dynamic_counting_bloom_filter<int, 16, 1> bloom16(400);
  twohash_dynamic_counting_bloom_filter<int, 32, 1> bloom32(400);

  for (size_t i = 0; i < 100; ++i) {
    bloom_default.insert(i);
    bloom1.insert(i);
    bloom2.insert(i);
    bloom4.insert(i);
    bloom8.insert(i);
    bloom16.insert(i);
    bloom32.insert(i);
  }

  BOOST_CHECK_EQUAL(bloom_default.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom1.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom4.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom8.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom16.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom32.count(), 100ul);

  for (size_t i = 0; i < 100; ++i) {
    bloom_default.insert(i);
    bloom2.insert(i);
    bloom4.insert(i);
    bloom8.insert(i);
    bloom16.insert(i);
    bloom32.insert(i);
  }

  BOOST_CHECK_EQUAL(bloom_default.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom4.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom8.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom16.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom32.count(), 100ul);

  for (size_t i = 0; i < 100; ++i) {
    bloom_default.remove(i);
    bloom2.remove(i);
    bloom4.remove(i);
    bloom8.remove(i);
    bloom16.remove(i);
    bloom32.remove(i);
  }

  BOOST_CHECK_EQUAL(bloom_default.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom4.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom8.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom16.count(), 100ul);
  BOOST_CHECK_EQUAL(bloom32.count(), 100ul);

  for (size_t i = 0; i < 100; ++i) {
    bloom_default.remove(i);
    bloom1.remove(i);
    bloom2.remove(i);
    bloom4.remove(i);
    bloom8.remove(i);
    bloom16.remove(i);
    bloom32.remove(i);
  }

  BOOST_CHECK_EQUAL(bloom_default.count(), 0ul);
  BOOST_CHECK_EQUAL(bloom1.count(), 0ul);
  BOOST_CHECK_EQUAL(bloom2.count(), 0ul);
  BOOST_CHECK_EQUAL(bloom4.count(), 0ul);
  BOOST_CHECK_EQUAL(bloom8.count(), 0ul);
  BOOST_CHECK_EQUAL(bloom16.count(), 0ul);
  BOOST_CHECK_EQUAL(bloom32.count(), 0ul);
}

BOOST_AUTO_TEST_CASE(rangeConstructor) {
  int elems[5] = {1,2,3,4,5};
  twohash_dynamic_counting_bloom_filter<int, 4, 1> bloom(elems, elems+5);

  BOOST_CHECK_EQUAL(bloom.count(), 5ul);
}

BOOST_AUTO_TEST_CASE(copyConstructor) {
  int elems[5] = {1,2,3,4,5};
  twohash_dynamic_counting_bloom_filter<int> bloom1(elems, elems+5);
  twohash_dynamic_counting_bloom_filter<int> bloom2(bloom1);

  BOOST_CHECK_EQUAL(bloom1.count(), bloom2.count());
}

BOOST_AUTO_TEST_CASE(assignment)
{
  twohash_dynamic_counting_bloom_filter<int> bloom1(200);
  twohash_dynamic_counting_bloom_filter<int> bloom2(200);

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
  twohash_dynamic_counting_bloom_filter<size_t> bloom_8(8);
  twohash_dynamic_counting_bloom_filter<size_t> bloom_256(256);
  twohash_dynamic_counting_bloom_filter<size_t> bloom_2048(2048);
  
  BOOST_CHECK_EQUAL(bloom_8.bit_capacity(), 
		    8ul * bloom_8.bits_per_bin());
  BOOST_CHECK_EQUAL(bloom_256.bit_capacity(), 
		    256ul * bloom_256.bits_per_bin());
  BOOST_CHECK_EQUAL(bloom_2048.bit_capacity(), 
		    2048ul * bloom_2048.bits_per_bin());
}

BOOST_AUTO_TEST_CASE(empty) {
  twohash_dynamic_counting_bloom_filter<size_t> bloom;
  
  BOOST_CHECK_EQUAL(bloom.empty(), true);
  bloom.insert(1);
  BOOST_CHECK_EQUAL(bloom.empty(), false);
  bloom.clear();
  BOOST_CHECK_EQUAL(bloom.empty(), true);
}

BOOST_AUTO_TEST_CASE(numHashFunctions) {
  twohash_dynamic_counting_bloom_filter<int, 2> bloom_2h;
  twohash_dynamic_counting_bloom_filter<int, 2, 1> bloom_1h;
  twohash_dynamic_counting_bloom_filter<int, 2, 7> bloom_7h;

  BOOST_CHECK_EQUAL(bloom_1h.num_hash_functions(), 1ul);
  BOOST_CHECK_EQUAL(bloom_2h.num_hash_functions(), 2ul);
  BOOST_CHECK_EQUAL(bloom_7h.num_hash_functions(), 7ul);
}

BOOST_AUTO_TEST_CASE(probably_contains) {
  twohash_dynamic_counting_bloom_filter<size_t, 4, 1> bloom;

  bloom.insert(1);
  BOOST_CHECK_EQUAL(bloom.probably_contains(1), true);
  BOOST_CHECK_EQUAL(bloom.count(), 1ul);
}

BOOST_AUTO_TEST_CASE(doesNotContain) {
  twohash_dynamic_counting_bloom_filter<size_t> bloom;

  BOOST_CHECK_EQUAL(bloom.probably_contains(1), false);
}

BOOST_AUTO_TEST_CASE(insertNoFalseNegatives) {
  twohash_dynamic_counting_bloom_filter<size_t> bloom(100);

  for (size_t i = 0; i < 100; ++i) {
    bloom.insert(i);
    BOOST_CHECK_EQUAL(bloom.probably_contains(i), true);
  }
}

BOOST_AUTO_TEST_CASE(insertOverflowExceptionThrown) {
  twohash_dynamic_counting_bloom_filter<size_t, 1> bloom;
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
  twohash_dynamic_counting_bloom_filter<size_t, 1> bloom;
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
  twohash_dynamic_counting_bloom_filter<size_t, 4> bloom;
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

BOOST_AUTO_TEST_CASE(rangeInsert) {
  int elems[5] = {1,2,3,4,5};
  twohash_dynamic_counting_bloom_filter<size_t> bloom(10);

  bloom.insert(elems, elems+5);
  BOOST_CHECK_EQUAL(bloom.count(), 5ul);
}

BOOST_AUTO_TEST_CASE(_remove) {
  twohash_dynamic_counting_bloom_filter<size_t> bloom;
  
  bloom.insert(1);
  bloom.remove(1);
  BOOST_CHECK_EQUAL(bloom.count(), 0ul);
}

BOOST_AUTO_TEST_CASE(removeMulti) {
  twohash_dynamic_counting_bloom_filter<size_t> bloom(100);

  for (size_t i = 0; i < 100; ++i) {
    bloom.insert(i);
    bloom.remove(i);
    BOOST_CHECK_EQUAL(bloom.count(), 0ul);
  }
}

BOOST_AUTO_TEST_CASE(rangeRemove) {
  size_t arr[] = {1,2,3,4,5};
  twohash_dynamic_counting_bloom_filter<size_t> bloom;

  bloom.insert(arr, arr+5);
  bloom.remove(arr, arr+5);
  BOOST_CHECK_EQUAL(bloom.count(), 0ul);
}

BOOST_AUTO_TEST_CASE(clear) {
  twohash_dynamic_counting_bloom_filter<size_t> bloom(1000);

  for (size_t i = 0; i < 1000; ++i)
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
  twohash_dynamic_counting_bloom_filter<size_t, 4, 1> bloom1;
  twohash_dynamic_counting_bloom_filter<size_t, 4, 1> bloom2;
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

  PairwiseOpsFixture() {}    

  twohash_dynamic_counting_bloom_filter<size_t> bloom1;
  twohash_dynamic_counting_bloom_filter<size_t> bloom2;
  twohash_dynamic_counting_bloom_filter<size_t> bloom_result;
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
