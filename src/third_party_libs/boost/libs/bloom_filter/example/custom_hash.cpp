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

// example demonstrating how to overload default hash function
// in order to support user-defined type
#include <boost/bloom_filter/basic_bloom_filter.hpp>
#include <sstream>
#include <string>
#include <iostream>
using namespace boost::bloom_filters;
using namespace std;

class URL {
public:
  URL() : _data() {}
  explicit URL(const string& url) : _data(url) {}

  const string data() const {
    return _data;
  }
private:
  string _data;
};

// provide an overload for your class
// alternatively, implement own Hasher that can handle your type
namespace boost {
  namespace bloom_filters {
    template <size_t Seed>
    struct boost_hash<URL, Seed> {
      size_t operator()(const URL& t) {
	return boost::hash_value(t.data()) + Seed;
      }
    };
  }
}

const URL gen_url(const size_t num)
{
  static const string start_url("https://www.");
  static const string end_url(".com/");
  static stringstream stringer;
  string result;

  stringer << num;
  stringer >> result;
  stringer.clear();

  return URL(start_url + result + end_url);
}

int main () {
  static const size_t INSERT_MAX = 5000;
  static const size_t CONTAINS_MAX = 10000;
  static const size_t NUM_BITS = 32768; // 8KB

  basic_bloom_filter<URL, NUM_BITS> bloom;
  size_t collisions = 0;

  for (size_t i = 0; i < INSERT_MAX; ++i) {
    bloom.insert(gen_url(i));
  }

  for (size_t i = INSERT_MAX; i < CONTAINS_MAX; ++i) {
    if (bloom.probably_contains(gen_url(i))) ++collisions;
  }

  cout << "collisions: " << collisions << endl;

  return 0;
}
