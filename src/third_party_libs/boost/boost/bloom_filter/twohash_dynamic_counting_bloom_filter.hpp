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

#ifndef BOOST_BLOOM_FILTER_DYNAMIC_COUNTING_BLOOM_FILTER_HPP
#define BOOST_BLOOM_FILTER_DYNAMIC_COUNTING_BLOOM_FILTER_HPP 1

#include <cmath>
#include <vector>

#include <boost/config.hpp>

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_unsigned.hpp>

#include <boost/bloom_filter/detail/twohash_counting_apply_hash.hpp>
#include <boost/bloom_filter/detail/extenders.hpp>
#include <boost/bloom_filter/hash/default.hpp>
#include <boost/bloom_filter/hash/murmurhash3.hpp>

namespace boost {
  namespace bloom_filters {
    template <typename T,
	      size_t BitsPerBin = 4,
	      size_t HashValues = 2,
	      size_t ExpectedInsertionCount = 0,
	      class HashFunction1 = boost_hash<T>,
	      class HashFunction2 = murmurhash3<T>,
	      class ExtensionFunction = detail::square,
	      typename Block = size_t,
	      typename Allocator = std::allocator<Block> >
    class twohash_dynamic_counting_bloom_filter {

      // Block needs to be an integral type
      BOOST_STATIC_ASSERT( boost::is_integral<Block>::value == true);

      // Block needs to be an unsigned type
      BOOST_STATIC_ASSERT( boost::is_unsigned<Block>::value == true);

      // BitsPerBin needs to be greater than 0
      BOOST_STATIC_ASSERT( BitsPerBin > 0);

      // it doesn't make sense to ever support using a BitsPerBin value larger
      // than the number of bits per Block. In that case, the user shouldn't
      // be using a Bloom filter to represent their data.
      BOOST_STATIC_ASSERT( (BitsPerBin < (sizeof(Block) * 8) ) );

      // because of the nature of this implementation, the Bloom filter
      // can have internal fragmentation if the calculation for 
      // bins_per_slot has a remainder. The severity of the  internal
      // fragmentation is equal to the remainder * the number of slots.
      // This check prevents internal fragmentation.
      // This also necessarily limits to bin sizes to one of:
      // [1,2,4,8,16,32(64-bit system only)] bits
      BOOST_STATIC_ASSERT( ((sizeof(Block) * 8) % BitsPerBin) == 0);

    public:
      typedef T value_type;
      typedef T key_type;
      typedef HashFunction1 hash_function1_type;
      typedef HashFunction2 hash_function2_type;
      typedef ExtensionFunction extension_function_type;
      typedef Block block_type;
      typedef Allocator allocator_type;
      typedef twohash_dynamic_counting_bloom_filter<T, BitsPerBin,
						    HashValues,
						    ExpectedInsertionCount,
						    HashFunction1,
						    HashFunction2,
						    ExtensionFunction,
						    Block, Allocator> this_type;

      typedef std::vector<Block, Allocator> bucket_type;
      typedef typename bucket_type::iterator bucket_iterator;
      typedef typename bucket_type::const_iterator bucket_const_iterator;

      static const size_t default_num_bins = 32;

    private:
      static const size_t slot_bits = sizeof(block_type) * 8;

      size_t bucket_size(const size_t requested_bins) const {
	const size_t bin_bits = requested_bins * BitsPerBin;
	return bin_bits / slot_bits + 1;
      }

      typedef detail::twohash_counting_apply_hash<HashValues,
						  this_type> apply_hash_type;

    public:
      //! constructors
      twohash_dynamic_counting_bloom_filter() 
	: bits(bucket_size(default_num_bins)),
	  _num_bins(default_num_bins)
      {
      }

      explicit twohash_dynamic_counting_bloom_filter(const size_t requested_bins)
	: bits(bucket_size(requested_bins)),
	  _num_bins(requested_bins)
      {
      }

      template <typename InputIterator>
      twohash_dynamic_counting_bloom_filter(const InputIterator start, 
					    const InputIterator end) 
	: bits(bucket_size(std::distance(start, end) * 4)),
	  _num_bins(std::distance(start, end) * 4)
      {
	for (InputIterator i = start; i != end; ++i)
	  this->insert(*i);
      }

      //! meta functions
      size_t num_bins() const
      {
	return this->_num_bins;
      }

      static BOOST_CONSTEXPR size_t expected_insertion_count()
      {
	return ExpectedInsertionCount;
      }

      static BOOST_CONSTEXPR size_t bits_per_bin()
      {
	return BitsPerBin;
      }

      static BOOST_CONSTEXPR size_t bins_per_slot()
      {
	return sizeof(block_type) * 8 / BitsPerBin;
      }

      static BOOST_CONSTEXPR size_t mask()
      {
	return static_cast<Block>(0 - 1) >> (sizeof(Block) * 8 - BitsPerBin);
      }

      size_t bit_capacity() const
      {
        return this->num_bins() * BitsPerBin;
      }

      static BOOST_CONSTEXPR size_t num_hash_functions() 
      {
        return HashValues;
      }

      double false_positive_rate() const 
      {
        const double n = static_cast<double>(this->count());
        static const double k = static_cast<double>(num_hash_functions());
        static const double m = static_cast<double>(this->num_bins());
        static const double e =
	  2.718281828459045235360287471352662497757247093699959574966;
        return std::pow(1 - std::pow(e, -k * n / m), k);
      }

      //? returns the number of bins that have at least 1 bit set
      size_t count() const 
      {
	size_t ret = 0;

	for (bucket_const_iterator i = this->bits.begin(), 
	       end = this->bits.end(); 
	     i != end; ++i) {
	  for (size_t bin = 0; bin < this->bins_per_slot(); ++bin) {
	    const size_t offset_bits = bin * BitsPerBin;
	    const size_t target_bits = (*i >> offset_bits) & this->mask();

	    if (target_bits > 0)
	      ++ret;
	  }
	}

        return ret;
      }

      bool empty() const
      {
	return this->count() == 0;
      }

      const bucket_type&
      data() const 
      {
	return this->bits;
      }

      //! core ops
      void insert(const T& t)
      {
	apply_hash_type::insert(t, 
				this->bits,
				this->num_bins());
      }

      template <typename InputIterator>
      void insert(const InputIterator start, const InputIterator end)
      {
	for (InputIterator i = start; i != end; ++i) {
	  this->insert(*i);
	}
      }

      void remove(const T& t)
      {
	apply_hash_type::remove(t, 
				this->bits,
				this->num_bins());
      }

      template <typename InputIterator>
      void remove(const InputIterator start, const InputIterator end)
      {
	for (InputIterator i = start; i != end; ++i) {
	  this->remove(*i);
	}
      }

      bool probably_contains(const T& t) const
      {
	return apply_hash_type::contains(t,
					 this->bits,
					 this->num_bins());
      }

      //! auxiliary ops
      void clear()
      {
	for (bucket_iterator i = bits.begin(), end = bits.end();
	     i != end; ++i)
	  *i = 0;
      }

      void swap(twohash_dynamic_counting_bloom_filter& other)
      {
	twohash_dynamic_counting_bloom_filter tmp = other;
	other = *this;
	*this = tmp;
      }

      // equality comparison operators
      template <typename _T, size_t _BitsPerBin,
		size_t _HashValues, size_t _ExpectedInsertionCount,
		class _HashFn1, class _HashFn2, class _Extender,
		typename _Block, class _Allocator>
      friend bool
      operator==(const twohash_dynamic_counting_bloom_filter<_T, _BitsPerBin,
							     _HashValues,
							     _ExpectedInsertionCount,
							     _HashFn1,
							     _HashFn2,
							     _Extender,
							     _Block,
							     _Allocator>& lhs,
		 const twohash_dynamic_counting_bloom_filter<_T, _BitsPerBin,
							     _HashValues,
							     _ExpectedInsertionCount,
							     _HashFn1,
							     _HashFn2,
							     _Extender,
							     _Block,
							     _Allocator>& rhs);

      template <typename _T, size_t _BitsPerBin,
		size_t _HashValues, size_t _ExpectedInsertionCount,
		class _HashFn1, class _HashFn2, class _Extender,
		typename _Block, class _Allocator>
      friend bool
      operator!=(const twohash_dynamic_counting_bloom_filter<_T, _BitsPerBin,
							     _HashValues,
							     _ExpectedInsertionCount,
							     _HashFn1,
							     _HashFn2,
							     _Extender,
							     _Block,
							     _Allocator>& lhs,
		 const twohash_dynamic_counting_bloom_filter<_T, _BitsPerBin,
							     _HashValues,
							     _ExpectedInsertionCount,
							     _HashFn1,
							     _HashFn2,
							     _Extender,
							     _Block,
							     _Allocator>& rhs);

    private:
      bucket_type bits;
      size_t _num_bins;
    };

    template<class T, size_t BitsPerBin, size_t HashValues,
	     size_t ExpectedInsertionCount,
	     class HashFunction1, class HashFunction2,
	     class ExtensionFunction, typename Block,
	     class Allocator>
    void
    swap(twohash_dynamic_counting_bloom_filter<T, BitsPerBin,
					       HashValues,
					       ExpectedInsertionCount,
					       HashFunction1,
					       HashFunction2,
					       ExtensionFunction,
					       Block,
					       Allocator>& lhs,
	 twohash_dynamic_counting_bloom_filter<T, BitsPerBin,
					       HashValues,
					       ExpectedInsertionCount,
					       HashFunction1,
					       HashFunction2,
					       ExtensionFunction,
					       Block,
					       Allocator>& rhs)

    {
      lhs.swap(rhs);
    }

    template<class T, size_t BitsPerBin, size_t HashValues,
	     size_t ExpectedInsertionCount,
	     class HashFunction1, class HashFunction2,
	     class ExtensionFunction, typename Block,
	     class Allocator>
    bool
    operator==(const twohash_dynamic_counting_bloom_filter<T, BitsPerBin,
							   HashValues,
							   ExpectedInsertionCount,
							   HashFunction1,
							   HashFunction2,
							   ExtensionFunction,
							   Block,
							   Allocator>& lhs,
	       const twohash_dynamic_counting_bloom_filter<T, BitsPerBin,
							   HashValues,
							   ExpectedInsertionCount,
							   HashFunction1,
							   HashFunction2,
							   ExtensionFunction,
							   Block,
							   Allocator>& rhs)
    {
      if (lhs.bit_capacity() != rhs.bit_capacity())
	throw detail::incompatible_size_exception();

      return (lhs.bits == rhs.bits);
    }

    template<class T, size_t BitsPerBin, size_t HashValues,
	     size_t ExpectedInsertionCount,
	     class HashFunction1, class HashFunction2,
	     class ExtensionFunction, typename Block,
	     class Allocator>
    bool
    operator!=(const twohash_dynamic_counting_bloom_filter<T, BitsPerBin,
							   HashValues,
							   ExpectedInsertionCount,
							   HashFunction1,
							   HashFunction2,
							   ExtensionFunction,
							   Block,
							   Allocator>& lhs,
	       const twohash_dynamic_counting_bloom_filter<T, BitsPerBin,
							   HashValues,
							   ExpectedInsertionCount,
							   HashFunction1,
							   HashFunction2,
							   ExtensionFunction,
							   Block,
							   Allocator>& rhs)
    {
      if (lhs.bit_capacity() != rhs.bit_capacity())
	throw detail::incompatible_size_exception();

      return !(lhs == rhs);
    }

  } // namespace bloom_filter
} // namespace boost
#endif
