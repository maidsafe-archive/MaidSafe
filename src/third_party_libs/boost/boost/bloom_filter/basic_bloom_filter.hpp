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

#ifndef BOOST_BLOOM_FILTER_BLOOM_FILTER_HPP
#define BOOST_BLOOM_FILTER_BLOOM_FILTER_HPP 1

#include <cmath>
#include <bitset>

#include <boost/config.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/size.hpp>

#include <boost/bloom_filter/detail/apply_hash.hpp>
#include <boost/bloom_filter/hash/default.hpp>

#ifndef BOOST_NO_0X_HDR_INITIALIZER_LIST
#include <initializer_list>
#endif 

namespace boost {
  namespace bloom_filters {
    template <typename T,
	      size_t Size,
	      class HashFunctions = mpl::vector<boost_hash<T> > >
    class basic_bloom_filter {
    public:
      typedef T value_type;
      typedef T key_type;
      typedef std::bitset<Size> bitset_type;
      typedef HashFunctions hash_function_type;
      typedef basic_bloom_filter<T, Size,
				 HashFunctions> this_type;

    private:
      typedef detail::apply_hash<mpl::size<HashFunctions>::value - 1,
				 this_type> apply_hash_type;

    public:
      basic_bloom_filter() {}

      template <typename InputIterator>
      basic_bloom_filter(const InputIterator start, const InputIterator end) {
	for (InputIterator i = start; i != end; ++i)
	  this->insert(*i);
      }

#ifndef BOOST_NO_0X_HDR_INITIALIZER_LIST
      basic_bloom_filter(const std::initializer_list<T>& ilist) {
	typedef typename std::initializer_list<T>::const_iterator citer;
	for (citer i = ilist.begin(), end = ilist.end(); i != end; ++i) {
	  this->insert(*i);
	}
      }
#endif

      static BOOST_CONSTEXPR size_t bit_capacity() {
        return Size;
      }

      static BOOST_CONSTEXPR size_t num_hash_functions() {
        return mpl::size<HashFunctions>::value;
      };

      double false_positive_rate() const {
        const double n = static_cast<double>(this->bits.count());
        static const double k = static_cast<double>(num_hash_functions());
        static const double m = static_cast<double>(Size);
        static const double e =
	  2.718281828459045235360287471352662497757247093699959574966;
        return std::pow(1 - std::pow(e, -k * n / m), k);
      };

      size_t count() const {
        return this->bits.count();
      };

      bool empty() const {
	return this->count() == 0;
      }

      const bitset_type&
      data() const
      {
	return this->bits;
      }

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

      void clear() {
        this->bits.reset();
      }

      void swap(basic_bloom_filter& other) {
	basic_bloom_filter tmp = other;
	other = *this;
	*this = tmp;
      }

      basic_bloom_filter& operator|=(const basic_bloom_filter& rhs) {
        this->bits |= rhs.bits;
        return *this;
      }

      basic_bloom_filter& operator&=(const basic_bloom_filter& rhs) {
        this->bits &= rhs.bits;
        return *this;
      }

      template<class _T, size_t _Size, class _HashFunctions>
      friend bool
      operator==(const basic_bloom_filter<_T, _Size, _HashFunctions>&,
		 const basic_bloom_filter<_T, _Size, _HashFunctions>&);

      template<class _T, size_t _Size, class _HashFunctions>
      friend bool
      operator!=(const basic_bloom_filter<_T, _Size, _HashFunctions>&,
		 const basic_bloom_filter<_T, _Size, _HashFunctions>&);
      
    private:
      bitset_type bits;
    };

    template<class _T, size_t _Size, class _HashFunctions>
    bool
    operator==(const basic_bloom_filter<_T, _Size, _HashFunctions>& lhs,
	       const basic_bloom_filter<_T, _Size, _HashFunctions>& rhs)
    {
      return (lhs.bits == rhs.bits);
    }

    template<class _T, size_t _Size, class _HashFunctions>
    bool
    operator!=(const basic_bloom_filter<_T, _Size, _HashFunctions>& lhs,
	       const basic_bloom_filter<_T, _Size, _HashFunctions>& rhs)
    {
      return !(lhs == rhs);
    }

    template<class _T, size_t _Size, class _HashFunctions>
    basic_bloom_filter<_T, _Size, _HashFunctions>
    operator|(const basic_bloom_filter<_T, _Size, _HashFunctions>& lhs,
	      const basic_bloom_filter<_T, _Size, _HashFunctions>& rhs)
    {
      basic_bloom_filter<_T, _Size, _HashFunctions> ret(lhs);
      ret |= rhs;
      return ret;
    }

    template<class _T, size_t _Size, class _HashFunctions>
    basic_bloom_filter<_T, _Size, _HashFunctions>
    operator&(const basic_bloom_filter<_T, _Size, _HashFunctions>& lhs,
	      const basic_bloom_filter<_T, _Size, _HashFunctions>& rhs)
    {
      basic_bloom_filter<_T, _Size, _HashFunctions> ret(lhs);
      ret &= rhs;
      return ret;
    }

    template<class _T, size_t _Size, class _HashFunctions>
    void
    swap(basic_bloom_filter<_T, _Size, _HashFunctions>& lhs,
	 basic_bloom_filter<_T, _Size, _HashFunctions>& rhs)
    {
      lhs.swap(rhs);
    }
  } // namespace bloom_filters
} // namespace boost
#endif
