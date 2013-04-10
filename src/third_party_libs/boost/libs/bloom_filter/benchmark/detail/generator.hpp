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

#ifndef BOOST_DETAIL_GENERATOR_HPP
#define BOOST_DETAIL_GENERATOR_HPP

#include <boost/random/linear_congruential.hpp>

namespace boost {
  namespace detail {
    template <typename T>
    struct generator {
      T operator()() {
	return gen();
      }

    private:
      boost::minstd_rand gen;
    };
  }
}
#endif
