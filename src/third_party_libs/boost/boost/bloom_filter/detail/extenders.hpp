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

#ifndef BOOST_BLOOM_FILTER_DETAIL_EXTENDERS_HPP
#define BOOST_BLOOM_FILTER_DETAIL_EXTENDERS_HPP

namespace boost {
  namespace bloom_filters {
    namespace detail {
      template <size_t exponent>
      struct Power {
	static size_t power(const size_t val) {
	  return val * Power<exponent-1>::power(val);
	}
      };

      template <>
      struct Power<0> {
	static size_t power(const size_t val) {
	  return 0 * val + 1;
	}
      };

      struct square {
	size_t operator()(const size_t val) {
	  return Power<2>::power(val);
	}
      };

      struct cube {
	size_t operator()(const size_t val) {
	  return Power<3>::power(val);
	}
      };

      struct fourth {
	size_t operator()(const size_t val) {
	  return Power<4>::power(val);
	}
      };

      struct zero {
	size_t operator()(const size_t val) {
	  return val * 0;
	}
      };
    }
  }
}
#endif
