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

#ifndef BOOST_BLOOM_FILTER_APPLY_HASH_HPP
#define BOOST_BLOOM_FILTER_APPLY_HASH_HPP

#include <boost/mpl/at.hpp>

namespace boost {
  namespace bloom_filters {
    namespace detail {

      template <size_t N,
		typename Container>
      struct apply_hash
      {
	typedef typename Container::value_type value_type;
	typedef typename Container::bitset_type bitset_type;
	typedef typename Container::hash_function_type hash_function_type;

        static void insert(const value_type& t, 
			   bitset_type& _bits) 
	{
	  typedef typename boost::mpl::at_c<hash_function_type, N>::type Hash;
	  static Hash hasher;

	  _bits[hasher(t) % _bits.size()] = true;
	  apply_hash<N-1, Container>::insert(t, _bits);
        }

        static bool contains(const value_type& t, 
			     const bitset_type& _bits)
	{
	  typedef typename boost::mpl::at_c<hash_function_type, N>::type Hash;
	  static Hash hasher;

	  return (_bits[hasher(t) % _bits.size()] && 
		  apply_hash<N-1, Container>::contains(t, _bits));
        }
      };

      template <typename Container>
      struct apply_hash<0, Container>
      {
	typedef typename Container::value_type value_type;
	typedef typename Container::bitset_type bitset_type;
	typedef typename Container::hash_function_type hash_function_type;

        static void insert(const value_type& t, 
			   bitset_type& _bits) 
	{
	  typedef typename boost::mpl::at_c<hash_function_type, 0>::type Hash;
	  static Hash hasher;

	  _bits[hasher(t) % _bits.size()] = true;
        }

        static bool contains(const value_type& t, 
			     const bitset_type& _bits)
	{
	  typedef typename boost::mpl::at_c<hash_function_type, 0>::type Hash;
	  static Hash hasher;

	  return (_bits[hasher(t) % _bits.size()]);
        }
      };

    } // namespace detail
  } // namespace bloom_filter
} // namespace boost
#endif
