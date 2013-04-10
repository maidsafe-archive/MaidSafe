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

#ifndef BOOST_BLOOM_FILTER_COUNTING_APPLY_HASH_HPP
#define BOOST_BLOOM_FILTER_COUNTING_APPLY_HASH_HPP

#include <boost/mpl/at.hpp>

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

	  return val+1;
	}
      };

      template <size_t N, class CBF, class Op = void>
      struct BloomOp {
	typedef typename boost::mpl::at_c<typename CBF::hash_function_type, 
					  N>::type Hash;

      public:
	BloomOp(const typename CBF::value_type& t,
		const typename CBF::bucket_type& slots,
		const size_t num_bins)
	  :
	  hash_val(hasher(t) % num_bins),
	  pos(hash_val / CBF::bins_per_slot()),
	  offset_bits((hash_val % CBF::bins_per_slot()) * CBF::bits_per_bin()),
	  target_bits((slots[pos] >> offset_bits) & CBF::mask())
	{}

	void update(typename CBF::bucket_type& slots,
		    const size_t limit) const {
	  static Op op;

	  const size_t final_bits = op(target_bits, limit);
	  slots[pos] &= ~(CBF::mask() << offset_bits);
	  slots[pos] |= (final_bits << offset_bits);
	}

	bool check() const {
	  return (target_bits != 0);
	}

	Hash hasher;
	const size_t hash_val;
	const size_t pos;
	const size_t offset_bits;
	const size_t target_bits;
      };

      // CBF : Counting Bloom Filter
      template <size_t N, 
		class CBF>
      struct counting_apply_hash
      {
	static void insert(const typename CBF::value_type& t, 
			   typename CBF::bucket_type& slots,
			   const size_t num_bins)
	{
	  BloomOp<N, CBF, increment> inserter(t, slots, num_bins);
	  inserter.update(slots, (1ull << CBF::bits_per_bin()) - 1ull);

	  counting_apply_hash<N-1, CBF>::insert(t, slots);
	}

	static void remove(const typename CBF::value_type& t, 
			   typename CBF::bucket_type& slots,
			   const size_t num_bins)
	{
	  BloomOp<N, CBF, decrement> remover(t, slots, num_bins);
	  remover.update(slots, 0);

	  counting_apply_hash<N-1, CBF>::remove(t, slots);
	}

	static bool contains(const typename CBF::value_type& t, 
			     const typename CBF::bucket_type& slots,
			   const size_t num_bins)
	{
	  BloomOp<N, CBF> checker(t, slots, num_bins);
	  return (checker.check() && 
		  counting_apply_hash<N-1, CBF>::contains(t, slots));
	}
      };

      template <class CBF>
      struct counting_apply_hash<0, CBF>
      {
	static void insert(const typename CBF::value_type& t, 
			   typename CBF::bucket_type& slots,
			   const size_t num_bins)
	{
	  BloomOp<0, CBF, increment> inserter(t, slots, num_bins);
	  inserter.update(slots, (1ull << CBF::bits_per_bin()) - 1ull);
	}

	static void remove(const typename CBF::value_type& t, 
			   typename CBF::bucket_type& slots,
			   const size_t num_bins)
	{
	  BloomOp<0, CBF, decrement> remover(t, slots, num_bins);
	  remover.update(slots, 0);
	}

	static bool contains(const typename CBF::value_type& t, 
			     const typename CBF::bucket_type& slots,
			   const size_t num_bins)
	{
	  BloomOp<0, CBF> checker(t, slots, num_bins);
	  return (checker.check());
	}
      };

    } // namespace detail
  } // namespace bloom_filter
} // namespace boost
#endif
