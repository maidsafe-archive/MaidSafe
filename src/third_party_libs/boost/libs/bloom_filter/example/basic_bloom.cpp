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

// introductory Boost.BloomFilter program
#include <boost/bloom_filter/basic_bloom_filter.hpp>
#include <iostream>
using namespace boost::bloom_filters;
using namespace std;

int main () {
  static const size_t INSERT_MAX = 5000;
  static const size_t CONTAINS_MAX = 10000;
  static const size_t NUM_BITS = 8192;

  basic_bloom_filter<int, NUM_BITS> bloom;
  size_t collisions = 0;

  for (size_t i = 0; i < INSERT_MAX; ++i) {
    bloom.insert(i);
  }

  for (size_t i = INSERT_MAX; i < CONTAINS_MAX; ++i) {
    if (bloom.probably_contains(i)) ++collisions;
  }

  cout << "collisions: " << collisions << endl;

  return 0;
}
