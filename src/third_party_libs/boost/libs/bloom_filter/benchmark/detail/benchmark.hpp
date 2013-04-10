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

#ifndef BOOST_DETAIL_BENCHMARK_HPP
#define BOOST_DETAIL_BENCHMARK_HPP

#include <boost/timer.hpp>
#include <cstddef>

namespace boost {
  namespace detail {
    template <typename T, 
	      size_t OpCount,
	      class Generator,
	      class Container>
    class benchmark {
    public:
      benchmark() : test_time(0.0) {}
      
      double time() const { return test_time; }
      
      void run() {
	boost::timer timer;
	for (size_t i = 0; i < OpCount; ++i) {
	  container.insert(gen());
	}
	test_time = timer.elapsed();
      }

      void reset() {
        test_time = 0.0;
      }
      
    private:
      Container container;
      Generator gen;
      double test_time;
    };
  }
}
#endif
