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

#ifndef BOOST_BLOOM_FILTER_DETAIL_EXCEPTIONS_HPP
#define BOOST_BLOOM_FILTER_DETAIL_EXCEPTIONS_HPP

#include <exception>

namespace boost {
  namespace bloom_filters {
    namespace detail {

      class bin_underflow_exception : public std::exception {
	virtual const char *
	what() const throw() {
	  return "boost::bloom_filters::detail::bin_underflow_exception"; 
	}
      };

      class bin_overflow_exception : public std::exception {
	virtual const char *
	what() const throw() {
	  return "boost::bloom_filters::detail::bin_overflow_exception"; 
	}
      };

      class incompatible_size_exception : public std::exception {
	virtual const char *
	what() const throw() {
	  return "boost::bloom_filters::detail::incompatible_size_exception"; 
	}	
      };

    } // namespace detail
  } // namespace bloom_filter
} // namespace boost
#endif
