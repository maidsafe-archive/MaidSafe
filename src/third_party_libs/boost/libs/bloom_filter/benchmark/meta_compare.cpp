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

#include <iostream>
#include <boost/bloom_filter/basic_bloom_filter.hpp>
#include <boost/bloom_filter/dynamic_bloom_filter.hpp>
#include <boost/bloom_filter/counting_bloom_filter.hpp>
#include <boost/unordered_set.hpp>
#include <set>
using boost::bloom_filters::basic_bloom_filter;
using boost::bloom_filters::dynamic_bloom_filter;
using boost::bloom_filters::counting_bloom_filter;
using boost::unordered_set;

typedef basic_bloom_filter<int, 1000> basic_bloom;
typedef dynamic_bloom_filter<int> basic_dynamic_bloom;
typedef counting_bloom_filter<int, 1000> counting_bloom;

int main()
{
  basic_bloom basic;
  basic_dynamic_bloom dynamic(1000);
  counting_bloom counting;
  boost::unordered_set<int> hash_set;
  std::set<int> set;

  std::cout << "\n============== Static size comparison ==============\n";

  std::cout << "Basic Bloom filter static size: " << sizeof(basic_bloom) << "\n"
            << "Basic dynamic Bloom filter static size: " << sizeof(basic_dynamic_bloom) << "\n"
            << "Counting Bloom filter static size: " << sizeof(counting_bloom) << "\n"
            << "Unordered set static size: " << sizeof(boost::unordered_set<int>) << "\n"
            << "Set static size: " << sizeof(std::set<int>) << std::endl;

  for (size_t i = 0; i < 1000; ++i) {
    basic.insert(i);
    dynamic.insert(i);
    counting.insert(i);
    hash_set.insert(i);
    set.insert(i);
  }

  std::cout << "\n============== Run-time size comparison ==============\n";

  std::cout << "Basic Bloom filter run-time size: " << 0 << "\n"
            << "Basic dynamic Bloom filter run-time size: " << dynamic.bit_capacity() << "\n"
            << "Counting Bloom filter run-time size: " << 0 << "\n"
            << "Unordered set run-time size: " << sizeof(int) * hash_set.size() << " + X\n"
            << "Set run-time size: " << sizeof(int) * set.size() << " + X"  << std::endl;

  std::cout << "\n============== Total size comparison ==============\n";

  std::cout << "Basic Bloom filter total size: " << sizeof(basic_bloom) + 0 << "\n"
            << "Basic dynamic Bloom filter total size: " << sizeof(basic_dynamic_bloom) + dynamic.bit_capacity() << "\n"
            << "Counting Bloom filter total size: " << sizeof(counting_bloom) + 0 << "\n"
            << "Unordered set total size: " << sizeof(boost::unordered_set<int>) + sizeof(int) * hash_set.size() << " + X\n"
            << "Set total size: " << sizeof(std::set<int>) + sizeof(int) * set.size() << " + X"  << std::endl;

  return 0;
}

