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

#ifndef BOOST_BLOOM_FILTER_TWOHASH_APPLY_HASH_HPP
#define BOOST_BLOOM_FILTER_TWOHASH_APPLY_HASH_HPP

namespace boost {
  namespace bloom_filters {
    namespace detail {

      template <size_t N,
		typename Container>
      struct twohash_apply_hash
      {

      private:
	typedef typename Container::value_type value_type;
	typedef typename Container::bitset_type bitset_type;
	typedef typename Container::hash_function1_type hash_function1_type;
	typedef typename Container::hash_function2_type hash_function2_type;
	typedef typename Container::extension_function_type extension_function_type;

      public:
        static void insert(const value_type& t, 
			   bitset_type& bits) 
	{
	  static hash_function1_type hasher1;
	  static hash_function2_type hasher2;
	  static extension_function_type extender;

	  const size_t hash1 = hasher1(t);
	  const size_t hash2 = hasher2(t);

	  for (size_t i = 0; i < N; ++i) {
	    const size_t hash_val = hash1 + i * hash2 + extender(i);
	    bits[hash_val % bits.size()] = true;
	  }
        }

        static bool contains(const value_type& t, 
			     const bitset_type& bits)
	{
	  static hash_function1_type hasher1;
	  static hash_function2_type hasher2;
	  static extension_function_type extender;

	  const size_t hash1 = hasher1(t);
	  const size_t hash2 = hasher2(t);
	  
	  for (size_t i = 0; i < N; ++i) {
	    const size_t hash_val = hash1 + i * hash2 + extender(i);
	    if (bits[hash_val % bits.size()] != true)
	      return false;
	  }

	  return true;
        }
      };
    } // namespace detail
  } // namespace bloom_filter
} // namespace boost
#endif
