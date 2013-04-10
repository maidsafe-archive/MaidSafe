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

#ifndef BOOST_DETAIL_POW_HPP
#define BOOST_DETAIL_POW_HPP

#include <cstddef>

namespace boost {
  namespace detail {
    template <size_t Base, size_t Exponent>
    struct Pow {
      static const size_t val = Base * Pow<Base, Exponent - 1>::val;
    };
    
    template <size_t Base>
    struct Pow<Base, 1> {
      static const size_t val = Base;
    };
  }
}
#endif
