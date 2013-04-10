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

#ifndef BOOST_BLOOM_FILTER_DYNAMIC_BLOOM_FILTER_HPP
#define BOOST_BLOOM_FILTER_DYNAMIC_BLOOM_FILTER_HPP 1

#include <cmath>

#include <boost/config.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/size.hpp>
#include <boost/dynamic_bitset.hpp>

#include <boost/bloom_filter/detail/apply_hash.hpp>
#include <boost/bloom_filter/detail/exceptions.hpp>
#include <boost/bloom_filter/hash/default.hpp>

namespace boost {
  namespace bloom_filters {
    template <typename T,
	      class HashFunctions = mpl::vector<boost_hash<T> >,
	      class Block = size_t,
	      class Allocator = std::allocator<Block> >
    class dynamic_bloom_filter {
    public:
      typedef T value_type;
      typedef T key_type;
      typedef HashFunctions hash_function_type;
      typedef Block block_type;
      typedef Allocator allocator_type;
      typedef dynamic_bitset<block_type, allocator_type> bitset_type;
      typedef dynamic_bloom_filter<T, HashFunctions,
				   Block, Allocator> this_type;

    private:
      typedef detail::apply_hash<mpl::size<HashFunctions>::value - 1,
				 this_type> apply_hash_type;

    public:
      
      //* constructors
      dynamic_bloom_filter() {}
      
      explicit dynamic_bloom_filter(const size_t bit_capacity) : 
	bits(bit_capacity) {}
      
      template <typename InputIterator>
      dynamic_bloom_filter(const InputIterator start, 
			   const InputIterator end) 
	: bits(std::distance(start, end) * 4)
      {
	for (InputIterator i = start; i != end; ++i)
	  this->insert(*i);
      }

      //* query functions
      static BOOST_CONSTEXPR size_t num_hash_functions() {
        return mpl::size<HashFunctions>::value;
      }

      double false_positive_rate() const {
        const double n = static_cast<double>(this->bits.count());
        static const double k = static_cast<double>(num_hash_functions());
        static const double m = static_cast<double>(this->bits.size());
        static const double e =
	  2.718281828459045235360287471352662497757247093699959574966;
        return std::pow(1 - std::pow(e, -k * n / m), k);
      }

      size_t count() const {
        return this->bits.count();
      }

      size_t bit_capacity() const {
	return this->bits.size();
      }

      bool empty() const {
	return this->count() == 0;
      }

      const bitset_type&
      data() const
      {
	return this->bits;
      }

      //* core operations
      void insert(const T& t) {
	apply_hash_type::insert(t, bits);
      }

      template <typename InputIterator>
      void insert(const InputIterator start, const InputIterator end) {
	for (InputIterator i = start; i != end; ++i) {
	  this->insert(*i);
	}
      }

      bool probably_contains(const T& t) const {
	return apply_hash_type::contains(t, bits);
      }

      //* auxilliary operations
      void clear() {
        this->bits.reset();
      }

      void swap(dynamic_bloom_filter& other) {
	dynamic_bloom_filter tmp = other;
	other = *this;
	*this = tmp;
      }

      void resize(const size_t new_capacity) {
	bits.clear();
	bits.resize(new_capacity);
      }

      template <typename _T, typename _HashFunctions, 
		typename _Block, typename _Allocator>
      friend bool operator==(const dynamic_bloom_filter<_T, _HashFunctions, 
							_Block, _Allocator>&, 
			     const dynamic_bloom_filter<_T, _HashFunctions, 
							_Block, _Allocator>&);

      template <typename _T, typename _HashFunctions, 
		typename _Block, typename _Allocator>
      friend bool operator!=(const dynamic_bloom_filter<_T, 
							_HashFunctions, 
							_Block, 
							_Allocator>&, 
			     const dynamic_bloom_filter<_T, 
							_HashFunctions, 
							_Block, 
							_Allocator>&);

      dynamic_bloom_filter& operator|=(const dynamic_bloom_filter& rhs) {
	if(this->bit_capacity() != rhs.bit_capacity()) {
	  throw detail::incompatible_size_exception();
	}

        this->bits |= rhs.bits;
        return *this;
      }

      dynamic_bloom_filter& operator&=(const dynamic_bloom_filter& rhs) {
	if(this->bit_capacity() != rhs.bit_capacity()) {
	  throw detail::incompatible_size_exception();
	}

        this->bits &= rhs.bits;
        return *this;
      }

    private:
      bitset_type bits;
    };

    template<class T, class HashFunctions,
	     class Block, class Allocator>
    dynamic_bloom_filter<T, HashFunctions, Block, Allocator>
    operator|(const dynamic_bloom_filter<T, 
					 HashFunctions, 
					 Block, Allocator>& lhs,
	      const dynamic_bloom_filter<T, 
					 HashFunctions, 
					 Block, Allocator>& rhs)
    {
      if(lhs.bit_capacity() != rhs.bit_capacity()) {
	throw detail::incompatible_size_exception();
      }

      dynamic_bloom_filter<T, HashFunctions, Block, Allocator> ret(lhs);
      ret |= rhs;
      return ret;
    }

    template<class T, class HashFunctions,
	     class Block, class Allocator>
    dynamic_bloom_filter<T, HashFunctions, Block, Allocator>
    operator&(const dynamic_bloom_filter<T, 
					 HashFunctions, 
					 Block, Allocator>& lhs,
	      const dynamic_bloom_filter<T, 
					 HashFunctions, 
					 Block, Allocator>& rhs)
    {
      if(lhs.bit_capacity() != rhs.bit_capacity()) {
	throw detail::incompatible_size_exception();
      }

      dynamic_bloom_filter<T, HashFunctions, Block, Allocator> ret(lhs);
      ret &= rhs;
      return ret;
    }


    template<class T, class HashFunctions,
	     class Block, class Allocator>
    bool
    operator==(const dynamic_bloom_filter<T, 
					  HashFunctions, 
					  Block, Allocator>& lhs,
	       const dynamic_bloom_filter<T, 
					  HashFunctions, 
					  Block, Allocator>& rhs)
    {
      if(lhs.bit_capacity() != rhs.bit_capacity()) {
	throw detail::incompatible_size_exception();
      }

      return lhs.bits == rhs.bits;
    }

    template<class T, class HashFunctions,
	     class Block, class Allocator>
    bool
    operator!=(const dynamic_bloom_filter<T, 
					  HashFunctions, 
					  Block, Allocator>& lhs,
	       const dynamic_bloom_filter<T, 
					  HashFunctions, 
					  Block, Allocator>& rhs)
    {
      return !(lhs == rhs);
    }

    template<class T, class HashFunctions,
	     class Block, class Allocator>
    void
    swap(dynamic_bloom_filter<T, 
			      HashFunctions, 
			      Block, Allocator>& lhs,
	 dynamic_bloom_filter<T, 
			      HashFunctions, 
			      Block, Allocator>& rhs)
    {
      lhs.swap(rhs);
    }
  } // namespace bloom_filter
} // namespace boost
#endif
