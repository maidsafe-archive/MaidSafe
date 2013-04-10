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

#ifndef BOOST_BLOOM_FILTER_TWOHASH_BASIC_BLOOM_FILTER_HPP
#define BOOST_BLOOM_FILTER_TWOHASH_BASIC_BLOOM_FILTER_HPP 1

#include <cmath>
#include <bitset>

#include <boost/config.hpp>

#include <boost/bloom_filter/hash/default.hpp>
#include <boost/bloom_filter/hash/murmurhash3.hpp>
#include <boost/bloom_filter/detail/extenders.hpp>
#include <boost/bloom_filter/detail/twohash_apply_hash.hpp>

#ifndef BOOST_NO_0X_HDR_INITIALIZER_LIST
#include <initializer_list>
#endif 

namespace boost {
  namespace bloom_filters {
    template <typename T,
	      size_t Size,
	      size_t HashValues = 2,
	      size_t ExpectedInsertionCount = 0,
	      class HashFunction1 = boost_hash<T>,
	      class HashFunction2 = murmurhash3<T>, 
	      typename ExtensionFunction = detail::square>// = ???
    class twohash_basic_bloom_filter {
    public:
      typedef T value_type;
      typedef T key_type;
      typedef std::bitset<Size> bitset_type;
      typedef HashFunction1 hash_function1_type;
      typedef HashFunction2 hash_function2_type;
      typedef ExtensionFunction extension_function_type;
      typedef twohash_basic_bloom_filter<T, 
					 Size,
					 HashValues,
					 ExpectedInsertionCount,
					 HashFunction1,
					 HashFunction2,
					 ExtensionFunction> this_type;

    private:
      typedef detail::twohash_apply_hash<HashValues,
					 this_type> apply_hash_type;

    public:
      //* constructors
      twohash_basic_bloom_filter()
      {
      }

      template <typename InputIterator>
      twohash_basic_bloom_filter(const InputIterator start, 
				 const InputIterator end)
      {
	for (InputIterator i = start; i != end; ++i)
	  this->insert(*i);
      }

#ifndef BOOST_NO_0X_HDR_INITIALIZER_LIST
      twohash_basic_bloom_filter(const std::initializer_list<T>& ilist)
      {
	typedef typename std::initializer_list<T>::const_iterator citer;
	for (citer i = ilist.begin(), end = ilist.end(); i != end; ++i)
	  this->insert(*i);
      }
#endif

      //* meta-ops
      static BOOST_CONSTEXPR size_t bit_capacity()
      {
	return Size;
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
        static const double m = static_cast<double>(Size);
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

      void swap(twohash_basic_bloom_filter& other)
      {
	twohash_basic_bloom_filter tmp = other;
	other = *this;
	*this = tmp;
      }

      //* pairwise ops
      twohash_basic_bloom_filter& 
      operator|=(const twohash_basic_bloom_filter& rhs)
      {
	this->bits |= rhs.bits;
	return *this;
      }

      twohash_basic_bloom_filter& 
      operator&=(const twohash_basic_bloom_filter& rhs)
      {
	this->bits &= rhs.bits;
	return *this;
      }

      template<class _T, size_t _Size, size_t _HashValues, 
	       size_t _ExpectedInsertionCount,
	       class _HashFunction1,
	       class _HashFunction2,
	       class _ExtensionFunction>
      friend bool
      operator==(const twohash_basic_bloom_filter<_T, 
						  _Size,
		                                  _HashValues, 
						  _ExpectedInsertionCount,
						  _HashFunction1,
						  _HashFunction2,
						  _ExtensionFunction>&,
		 const twohash_basic_bloom_filter<_T, 
						  _Size,
		                                  _HashValues,
						  _ExpectedInsertionCount,
						  _HashFunction1,
						  _HashFunction2,
						  _ExtensionFunction>&);
      
    private:
      bitset_type bits;
    };

    //* global ops
    template<class T, size_t Size, size_t HashValues, 
	     size_t ExpectedInsertionCount,
	     class HashFunction1,
	     class HashFunction2, class ExtensionFunction>
    bool
    operator==(const twohash_basic_bloom_filter<T, 
						Size, 
	                                        HashValues,
						ExpectedInsertionCount,
						HashFunction1,
						HashFunction2,
						ExtensionFunction>& lhs,
	       const twohash_basic_bloom_filter<T, 
						Size, 
	                                        HashValues,
						ExpectedInsertionCount,
						HashFunction1,
						HashFunction2,
						ExtensionFunction>& rhs)
    {
      return lhs.bits == rhs.bits;
    }
    
    template<class T, size_t Size, size_t HashValues, 
	     size_t ExpectedInsertionCount,
	     class HashFunction1,
	     class HashFunction2, class ExtensionFunction>
    bool
    operator!=(const twohash_basic_bloom_filter<T, 
						Size, 
	                                        HashValues,
						ExpectedInsertionCount,
						HashFunction1,
						HashFunction2,
						ExtensionFunction>& lhs,
	       const twohash_basic_bloom_filter<T, 
						Size, 
	                                        HashValues,
						ExpectedInsertionCount,
						HashFunction1,
						HashFunction2,
						ExtensionFunction>& rhs)
    {
      return !(lhs == rhs);
    }
    
    template<class T, size_t Size, size_t HashValues,
	     size_t ExpectedInsertionCount,
	     class HashFunction1,
	     class HashFunction2, class ExtensionFunction>
    twohash_basic_bloom_filter<T, Size, 
			       HashValues,
			       ExpectedInsertionCount,
			       HashFunction1, 
			       HashFunction2, ExtensionFunction>
    operator|(const twohash_basic_bloom_filter<T, 
	                                       Size, 
	                                       HashValues,
	                                       ExpectedInsertionCount,
					       HashFunction1,
					       HashFunction2,
					       ExtensionFunction>& lhs,
	      const twohash_basic_bloom_filter<T, 
					       Size, 
	                                       HashValues,
	                                       ExpectedInsertionCount,
					       HashFunction1,
					       HashFunction2,
					       ExtensionFunction>& rhs)
    {
      twohash_basic_bloom_filter<T, Size, HashValues,
				 ExpectedInsertionCount,
				 HashFunction1, HashFunction2,
				 ExtensionFunction> result(lhs);
      
      result |= rhs;
      return result;
    }

    template<class T, size_t Size, size_t HashValues, 
	     size_t ExpectedInsertionCount, 
	     class HashFunction1,
	     class HashFunction2, class ExtensionFunction>
    twohash_basic_bloom_filter<T, Size, HashValues, 
			       ExpectedInsertionCount,
			       HashFunction1, 
			       HashFunction2, ExtensionFunction>
    operator&(const twohash_basic_bloom_filter<T, 
					       Size, 
	                                       HashValues,
					       ExpectedInsertionCount,
					       HashFunction1,
					       HashFunction2,
					       ExtensionFunction>& lhs,
	      const twohash_basic_bloom_filter<T, 
					       Size, 
	                                       HashValues,
					       ExpectedInsertionCount,
					       HashFunction1,
					       HashFunction2,
					       ExtensionFunction>& rhs)
    {
      twohash_basic_bloom_filter<T, Size, HashValues,
				 ExpectedInsertionCount,
				 HashFunction1, HashFunction2,
				 ExtensionFunction> result(lhs);
      
      result &= rhs;
      return result;
    }

    template<class T, size_t Size, size_t HashValues, 
	     size_t ExpectedInsertionCount, 
	     class HashFunction1,
	     class HashFunction2, class ExtensionFunction>
    void swap(twohash_basic_bloom_filter<T, 
					 Size,
					 HashValues,
					 ExpectedInsertionCount,
					 HashFunction1,
					 HashFunction2,
					 ExtensionFunction>& lhs,
	      twohash_basic_bloom_filter<T, 
					 Size, 
					 HashValues,
					 ExpectedInsertionCount,
					 HashFunction1,
					 HashFunction2,
					 ExtensionFunction>& rhs)
    {
      lhs.swap(rhs);
    }
  } // namespace bloom_filters
} // namespace boost
#endif
