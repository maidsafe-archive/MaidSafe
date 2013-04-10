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

#ifndef BOOST_BLOOM_FILTER_TWOHASH_COUNTING_APPLY_HASH_HPP
#define BOOST_BLOOM_FILTER_TWOHASH_COUNTING_APPLY_HASH_HPP

#include <boost/bloom_filter/detail/exceptions.hpp>

namespace boost {
  namespace bloom_filters {
    namespace detail {

      struct decrement {
	size_t operator()(const size_t val, const size_t limit) {
	  if (val == limit)
	    throw bin_underflow_exception();

	  return val - 1;
	}
      };
 
      struct increment {
	size_t operator()(const size_t val, const size_t limit) {
	  if (val == limit)
	    throw bin_overflow_exception();

	  return val + 1;
	}
      };

      template <size_t N, typename CBF, typename Op = void>
      struct BloomOp {
	typedef typename CBF::hash_function1_type hash_function1_type;
	typedef typename CBF::hash_function2_type hash_function2_type;
	typedef typename CBF::extension_function_type extension_function_type;
	
	BloomOp(const typename CBF::value_type& t)
	  : hash1_val(hash1(t)),
	    hash2_val(hash2(t))
	{
	}

	void update(typename CBF::bucket_type& slots,
		    const size_t num_bins,
		    const size_t limit)
	{
	  static Op op;
	  
	  for (size_t i = 0; i < N; ++i) {
	    const size_t hash = 
	      (hash1_val + i * hash2_val + ext(i)) % num_bins;
	    const size_t pos = hash / CBF::bins_per_slot();
	    const size_t offset_bits = 
	      (hash % CBF::bins_per_slot()) * CBF::bits_per_bin();
	    const size_t target_bits =
	      (slots[pos] >> offset_bits) & CBF::mask();

	    const size_t final_bits = op(target_bits, limit);
	    slots[pos] &= ~(CBF::mask() << offset_bits);
	    slots[pos] |= (final_bits << offset_bits);
	  }
	}

	bool check(const typename CBF::bucket_type& slots,
		   const size_t num_bins)
	{
	  for (size_t i = 0; i < N; ++i) {
	    const size_t hash = 
	      (hash1_val + i * hash2_val + ext(i)) % num_bins;
	    const size_t pos = hash / CBF::bins_per_slot();
	    const size_t offset_bits = 
	      (hash % CBF::bins_per_slot()) * CBF::bits_per_bin();
	    const size_t target_bits =
	      (slots[pos] >> offset_bits) & CBF::mask();
	    
	    if (target_bits == 0)
	      return false;
	  }

	  return true;
	}

	size_t hash1_val;
	size_t hash2_val;
	hash_function1_type hash1;
	hash_function2_type hash2;
	extension_function_type ext;
      };

      // CBF : Counting Bloom Filter
      template <size_t N, 
		class CBF>
      struct twohash_counting_apply_hash
      {
	static void insert(const typename CBF::value_type& t, 
			   typename CBF::bucket_type& slots,
			   const size_t num_bins)
	{
	  BloomOp<N, CBF, increment> inserter(t);
	  inserter.update(slots, num_bins, 
			  (static_cast<size_t>(1) << CBF::bits_per_bin()) - 1);
	}

	static void remove(const typename CBF::value_type& t, 
			   typename CBF::bucket_type& slots,
			   const size_t num_bins)
	{
	  BloomOp<N, CBF, decrement> remover(t);
	  remover.update(slots, num_bins, 0);
	}

	static bool contains(const typename CBF::value_type& t, 
			     const typename CBF::bucket_type& slots,
			     const size_t num_bins)
	{
	  BloomOp<N, CBF> checker(t);
	  return checker.check(slots, num_bins);
		
	}
      };

    } // namespace detail
  } // namespace bloom_filter
} // namespace boost
#endif
