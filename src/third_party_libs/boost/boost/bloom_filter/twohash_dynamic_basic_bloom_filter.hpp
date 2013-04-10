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

#ifndef BOOST_BLOOM_FILTER_TWOHASH_DYNAMIC_BASIC_BLOOM_FILTER_HPP
#define BOOST_BLOOM_FILTER_TWOHASH_DYNAMIC_BASIC_BLOOM_FILTER_HPP 1

#include <cmath>

#include <boost/config.hpp>
#include <boost/dynamic_bitset.hpp>

#include <boost/bloom_filter/hash/default.hpp>
#include <boost/bloom_filter/hash/murmurhash3.hpp>
#include <boost/bloom_filter/detail/extenders.hpp>
#include <boost/bloom_filter/detail/twohash_apply_hash.hpp>
#include <boost/bloom_filter/detail/exceptions.hpp>

namespace boost {
  namespace bloom_filters {
    template <typename T,
	      size_t HashValues = 2,
	      size_t ExpectedInsertionCount = 0,
	      class HashFunction1 = boost_hash<T>,
	      class HashFunction2 = murmurhash3<T>, 
	      typename ExtensionFunction = detail::square,
	      typename Block = size_t,
	      typename Allocator = std::allocator<Block> >// = ???
    class twohash_dynamic_basic_bloom_filter {
    public:
      typedef T value_type;
      typedef T key_type;
      typedef Block block_type;
      typedef Allocator allocator_type;
      typedef boost::dynamic_bitset<Block, Allocator> bitset_type;
      typedef HashFunction1 hash_function1_type;
      typedef HashFunction2 hash_function2_type;
      typedef ExtensionFunction extension_function_type;
      typedef twohash_dynamic_basic_bloom_filter<T, 
						 HashValues,
						 ExpectedInsertionCount,
						 HashFunction1,
						 HashFunction2,
						 ExtensionFunction,
						 Block,
						 Allocator> this_type;

      static const size_t default_size = 32;

    private:
      typedef detail::twohash_apply_hash<HashValues,
					 this_type> apply_hash_type;

    public:
      //* constructors
      twohash_dynamic_basic_bloom_filter()
	: bits(default_size)
      {
      }

      explicit twohash_dynamic_basic_bloom_filter(const size_t size)
	: bits(size)
      {
      }

      template <typename InputIterator>
      twohash_dynamic_basic_bloom_filter(const InputIterator start, 
				 const InputIterator end)
	: bits(std::distance(start, end) * 4)
      {
	for (InputIterator i = start; i != end; ++i)
	  this->insert(*i);
      }

      //* meta-ops
      size_t bit_capacity() const
      {
	return bits.size();
      }

      static BOOST_CONSTEXPR size_t num_hash_functions()
      {
	return HashValues;
      }

      static BOOST_CONSTEXPR size_t expected_insertion_count()
      {
	return ExpectedInsertionCount;
      }

      double false_positive_rate() const
      {
        const double n = static_cast<double>(this->bits.count());
        static const double k = static_cast<double>(HashValues);
        static const double m = static_cast<double>(this->bit_capacity());
        static const double e =
	  2.718281828459045235360287471352662497757247093699959574966;
        return std::pow(1 - std::pow(e, -k * n / m), k);
      }

      size_t count() const
      {
	return this->bits.count();
      }

      bool empty() const
      {
	return this->count() == 0;
      }

      const bitset_type&
      data() const 
      {
	return this->bits;
      }

      //* core ops
      void insert(const T& t)
      {
	apply_hash_type::insert(t, bits);
      }

      template <typename InputIterator>
      void insert(const InputIterator start, 
		  const InputIterator end)
      {
	for (InputIterator i = start; i != end; ++i)
	  this->insert(*i);
      }

      bool probably_contains(const T& t) const
      {
	return apply_hash_type::contains(t, bits);
      }

      void clear()
      {
	this->bits.reset();
      }

      void swap(twohash_dynamic_basic_bloom_filter& other)
      {
	twohash_dynamic_basic_bloom_filter tmp = other;
	other = *this;
	*this = tmp;
      }

      //* pairwise ops
      twohash_dynamic_basic_bloom_filter& 
      operator|=(const twohash_dynamic_basic_bloom_filter& rhs)
      {
	if (this->bit_capacity() != rhs.bit_capacity())
	  throw detail::incompatible_size_exception();

	this->bits |= rhs.bits;
	return *this;
      }

      twohash_dynamic_basic_bloom_filter& 
      operator&=(const twohash_dynamic_basic_bloom_filter& rhs)
      {
	if (this->bit_capacity() != rhs.bit_capacity())
	  throw detail::incompatible_size_exception();

	this->bits &= rhs.bits;
	return *this;
      }

      template<class _T, size_t _HashValues, 
	       size_t _ExpectedInsertionCount,
	       class _HashFunction1,
	       class _HashFunction2,
	       class _ExtensionFunction,
	       typename _Block, class _Allocator>
      friend bool
      operator==(const twohash_dynamic_basic_bloom_filter<_T, 
		                                          _HashValues, 
						          _ExpectedInsertionCount,
						          _HashFunction1,
						          _HashFunction2,
		                                          _ExtensionFunction,
		                                          _Block,
		                                          _Allocator>&,
		 const twohash_dynamic_basic_bloom_filter<_T, 
		                                          _HashValues,
						          _ExpectedInsertionCount,
						          _HashFunction1,
						          _HashFunction2,
		                                          _ExtensionFunction,
		                                          _Block,
		                                          _Allocator>&);
      
    private:
      bitset_type bits;
    };

    //* global ops
    template<class T, size_t HashValues, 
	     size_t ExpectedInsertionCount,
	     class HashFunction1,
	     class HashFunction2, class ExtensionFunction,
	     typename Block, class Allocator>
    bool
    operator==(const twohash_dynamic_basic_bloom_filter<T, 
	       	       	       	       	       	        HashValues,
	                                                ExpectedInsertionCount,
	                                                HashFunction1,
	                                                HashFunction2,
	                                                ExtensionFunction,
	                                                Block, 
	                                                Allocator>& lhs,
	       const twohash_dynamic_basic_bloom_filter<T, 
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

      return lhs.bits == rhs.bits;
    }

    template<class T, size_t HashValues, 
	     size_t ExpectedInsertionCount,
	     class HashFunction1,
	     class HashFunction2, class ExtensionFunction,
	     typename Block, class Allocator>
    bool
    operator!=(const twohash_dynamic_basic_bloom_filter<T, 
	       	       	       	       	       	        HashValues,
	                                                ExpectedInsertionCount,
	                                                HashFunction1,
	                                                HashFunction2,
	                                                ExtensionFunction,
	                                                Block, 
	                                                Allocator>& lhs,
	       const twohash_dynamic_basic_bloom_filter<T, 
	                                                HashValues,
						        ExpectedInsertionCount,
						        HashFunction1,
						        HashFunction2,
	                                                ExtensionFunction,
	                                                Block,
	                                                Allocator>& rhs)
    {
      return !(lhs == rhs);
    }
    
    template<class T, size_t HashValues, 
	     size_t ExpectedInsertionCount,
	     class HashFunction1,
	     class HashFunction2, class ExtensionFunction,
	     typename Block, class Allocator>
    twohash_dynamic_basic_bloom_filter<T, 
				       HashValues,
				       ExpectedInsertionCount,
				       HashFunction1, 
				       HashFunction2, ExtensionFunction,
				       Block, Allocator>
    operator|(const twohash_dynamic_basic_bloom_filter<T, 
	                                               HashValues,
	                                               ExpectedInsertionCount,
	                                               HashFunction1,
	                                               HashFunction2,
	                                               ExtensionFunction,
	                                               Block,
	                                               Allocator>& lhs,
	      const twohash_dynamic_basic_bloom_filter<T, 
	                                               HashValues,
	                                               ExpectedInsertionCount,
	                                               HashFunction1,
	                                               HashFunction2,
	                                               ExtensionFunction,
	                                               Block,
	                                               Allocator>& rhs)
    {
      twohash_dynamic_basic_bloom_filter<T, HashValues,
					 ExpectedInsertionCount,
					 HashFunction1, HashFunction2,
					 ExtensionFunction,
					 Block,
					 Allocator> result(lhs);
      
      result |= rhs;
      return result;
    }

    template<class T, size_t HashValues, 
	     size_t ExpectedInsertionCount,
	     class HashFunction1,
	     class HashFunction2, class ExtensionFunction,
	     typename Block, class Allocator>
    twohash_dynamic_basic_bloom_filter<T, 
				       HashValues,
				       ExpectedInsertionCount,
				       HashFunction1, 
				       HashFunction2, ExtensionFunction,
				       Block, Allocator>
    operator&(const twohash_dynamic_basic_bloom_filter<T, 
	                                               HashValues,
	                                               ExpectedInsertionCount,
	                                               HashFunction1,
	                                               HashFunction2,
	                                               ExtensionFunction,
	                                               Block,
	                                               Allocator>& lhs,
	      const twohash_dynamic_basic_bloom_filter<T, 
	                                               HashValues,
	                                               ExpectedInsertionCount,
	                                               HashFunction1,
	                                               HashFunction2,
	                                               ExtensionFunction,
	                                               Block,
	                                               Allocator>& rhs)
    {
      twohash_dynamic_basic_bloom_filter<T, HashValues,
					 ExpectedInsertionCount,
					 HashFunction1, HashFunction2,
					 ExtensionFunction,
					 Block,
					 Allocator> result(lhs);
      
      result &= rhs;
      return result;
    }

    template<class T, size_t Size, size_t HashValues, 
	     size_t ExpectedInsertionCount, 
	     class HashFunction1,
	     class HashFunction2, class ExtensionFunction,
	     typename Block, class Allocator>
    void swap(twohash_dynamic_basic_bloom_filter<T, 
	                                         HashValues,
	                                         ExpectedInsertionCount,
	                                         HashFunction1,
	                                         HashFunction2,
	                                         ExtensionFunction,
	                                         Block,
	                                         Allocator>& lhs,
	      twohash_dynamic_basic_bloom_filter<T, 
	                                         HashValues,
	                                         ExpectedInsertionCount,
	                                         HashFunction1,
	                                         HashFunction2,
	                                         ExtensionFunction,
	                                         Block,
	                                         Allocator>& rhs)
    {
      return lhs.swap(rhs);
    }
  } // namespace bloom_filters
} // namespace boost
#endif
 
