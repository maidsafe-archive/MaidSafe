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

// Note by original author - Austin Appleby
//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. You can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.

/**
 * Note by Alejandro Cabrera:
 *
 * I've made a few contributions to this implementation of murmurhash3.
 * One makes the implementation more readily portable by using
 * boost/cstdlib and boost/cstdint instead of relying on stdint.h
 * and stdlib.h to be present on a given platform. I've also removed
 * platform-specific macros from this version.
 *
 * I've also placed the implementation in the boost::hash namespace and
 * changed the function names from -> to:
 * - MurmurHash3_x86_32 -> murmurhash3_x86_32
 * - MurmurHash3_x86_128 -> murmurhash3_x86_128
 * - MurmurHash3_x64_128 -> murmurhash3_x64_128
 *
 * Finally, I've provided a unifying Hasher based on Murmur Hash 3:
 * - murmurhash3<T, Seed=0, Use128Mode=True>
 * 
 * This Hasher performs compile-time dispatch to the appropriate
 * murmurhash function based on the template parameters and whether
 * the compilation target is 64-bit or not.
 */
#include <boost/cstdint.hpp>

namespace boost {
  namespace bloom_filters {

    // forward declarations
    namespace detail {
      void murmurhash3_x86_32 ( const void *const key, const size_t len,
				const size_t seed, const void * out );
      void murmurhash3_x86_128 ( const void *const key, const size_t len,
				 const size_t seed, const void * out );
      void murmurhash3_x64_128 ( const void *const key, const size_t len,
				 const size_t seed, const void * out );
      template <bool, bool> 
      struct murmurhash3_dispatch;

      struct Is64Bit {
	static const bool value = (sizeof(size_t) == 8);
      };
    }

    template <typename T, size_t Seed = 0, bool Use128Mode = true>
    struct murmurhash3 {
      typedef detail::murmurhash3_dispatch<detail::Is64Bit::value,
					   Use128Mode> dispatch_type;
      size_t operator()(const T& t) {
	static dispatch_type dispatcher;
	size_t out[2] = {0,0};

	dispatcher(&t, sizeof(T), Seed, &out);
	
	return out[0];
      }
    };

    namespace detail {

      template <bool _64Bit = true, bool Use128Mode = true>
      struct murmurhash3_dispatch {
	void operator()(const void *const data, const size_t len,
			  const size_t seed, const void *out) {
	  murmurhash3_x64_128(data, len, seed, out);
	}
      };

      template <>
      struct murmurhash3_dispatch<true, true> {
	void operator()(const void *const data, const size_t len,
			  const size_t seed, const void *out) {
	  murmurhash3_x64_128(data, len, seed, out);
	}
      };

      template <>
      struct murmurhash3_dispatch<false, false> {
	void operator()(const void *const data, const size_t len,
			  const size_t seed, const void *out) {
	  murmurhash3_x86_32(data, len, seed, out);
	}
      };

      template <>
      struct murmurhash3_dispatch<false, true> {
	void operator()(const void *const data, const size_t len,
			  const size_t seed, const void *out) {
	  murmurhash3_x86_128(data, len, seed, out);
	}
      };

      inline uint32_t rotl32(const uint32_t x, const int8_t r)
      {
	return (x << r) | (x >> (32 - r));
      }

      inline uint64_t rotl64(const uint64_t x, const int8_t r)
      {
	return (x << r) | (x >> (64 - r));
      }

      //-----------------------------------------------------------------------------
      // Block read - if your platform needs to do endian-swapping or can only
      // handle aligned reads, do the conversion here
      
      uint32_t getblock(const uint32_t *const p, const size_t i)
      {
	return p[i];
      }
      
      uint64_t getblock(const uint64_t *const p, const size_t i)
      {
	return p[i];
      }
      
      //-----------------------------------------------------------------------------
      // Finalization mix - force all bits of a hash block to avalanche
      
      uint32_t fmix(const uint32_t val)
      {
	uint32_t h = val;

	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	
	return h;
      }

      //----------
      
      uint64_t fmix(const uint64_t val)
      {
	uint64_t k = val;

	k ^= k >> 33;
	k *= 0xff51afd7ed558ccd;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53;
	k ^= k >> 33;
	
	return k;
      }

      //-----------------------------------------------------------------------------

      void murmurhash3_x86_32 ( const void *const key, const size_t len,
				const size_t seed, const void * out )
      {
	const uint8_t *const data = static_cast<const uint8_t*>(key);
	const int nblocks = len / 4;

	uint32_t h1 = seed;

	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;

	//----------
	// body

	const uint32_t *const blocks = reinterpret_cast<const uint32_t *>(data + nblocks*4);

	for(int i = -nblocks; i; i++)
	  {
	    uint32_t k1 = getblock(blocks,i);

	    k1 *= c1;
	    k1 = rotl32(k1,15);
	    k1 *= c2;
    
	    h1 ^= k1;
	    h1 = rotl32(h1,13); 
	    h1 = h1*5+0xe6546b64;
	  }

	//----------
	// tail

	const uint8_t *const tail = static_cast<const uint8_t*>(data + nblocks*4);

	uint32_t k1 = 0;

	switch(len & 3)
	  {
	  case 3: k1 ^= tail[2] << 16;
	  case 2: k1 ^= tail[1] << 8;
	  case 1: k1 ^= tail[0];
	    k1 *= c1; k1 = rotl32(k1,15); k1 *= c2; h1 ^= k1;
	  };

	//----------
	// finalization

	h1 ^= len;

	h1 = fmix(h1);

	((uint32_t*)out)[1] = h1;
      } 

      //-----------------------------------------------------------------------------

      void murmurhash3_x86_128(const void *const key, const size_t len,
				 const size_t seed, const void *out )
      {
	const uint8_t *const data = static_cast<const uint8_t*>(key);
	const int nblocks = len / 16;

	uint32_t h1 = seed;
	uint32_t h2 = seed;
	uint32_t h3 = seed;
	uint32_t h4 = seed;

	static const uint32_t c1 = 0x239b961b; 
	static const uint32_t c2 = 0xab0e9789;
	static const uint32_t c3 = 0x38b34ae5; 
	static const uint32_t c4 = 0xa1e38b93;

	//----------
	// body

	const uint32_t *const blocks = reinterpret_cast<const uint32_t *>(data + nblocks*16);

	for(int i = -nblocks; i; i++)
	  {
	    uint32_t k1 = getblock(blocks,i*4+0);
	    uint32_t k2 = getblock(blocks,i*4+1);
	    uint32_t k3 = getblock(blocks,i*4+2);
	    uint32_t k4 = getblock(blocks,i*4+3);

	    k1 *= c1; k1  = rotl32(k1,15); k1 *= c2; h1 ^= k1;

	    h1 = rotl32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;

	    k2 *= c2; k2  = rotl32(k2,16); k2 *= c3; h2 ^= k2;

	    h2 = rotl32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;

	    k3 *= c3; k3  = rotl32(k3,17); k3 *= c4; h3 ^= k3;

	    h3 = rotl32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;

	    k4 *= c4; k4  = rotl32(k4,18); k4 *= c1; h4 ^= k4;

	    h4 = rotl32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
	  }

	//----------
	// tail

	const uint8_t *const tail = static_cast<const uint8_t*>(data + nblocks*16);

	uint32_t k1 = 0;
	uint32_t k2 = 0;
	uint32_t k3 = 0;
	uint32_t k4 = 0;

	switch(len & 15)
	  {
	  case 15: k4 ^= tail[14] << 16;
	  case 14: k4 ^= tail[13] << 8;
	  case 13: k4 ^= tail[12] << 0;
	    k4 *= c4; k4  = rotl32(k4,18); k4 *= c1; h4 ^= k4;

	  case 12: k3 ^= tail[11] << 24;
	  case 11: k3 ^= tail[10] << 16;
	  case 10: k3 ^= tail[ 9] << 8;
	  case  9: k3 ^= tail[ 8] << 0;
	    k3 *= c3; k3  = rotl32(k3,17); k3 *= c4; h3 ^= k3;

	  case  8: k2 ^= tail[ 7] << 24;
	  case  7: k2 ^= tail[ 6] << 16;
	  case  6: k2 ^= tail[ 5] << 8;
	  case  5: k2 ^= tail[ 4] << 0;
	    k2 *= c2; k2  = rotl32(k2,16); k2 *= c3; h2 ^= k2;

	  case  4: k1 ^= tail[ 3] << 24;
	  case  3: k1 ^= tail[ 2] << 16;
	  case  2: k1 ^= tail[ 1] << 8;
	  case  1: k1 ^= tail[ 0] << 0;
	    k1 *= c1; k1  = rotl32(k1,15); k1 *= c2; h1 ^= k1;
	  };

	//----------
	// finalization

	h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;

	h1 += h2; h1 += h3; h1 += h4;
	h2 += h1; h3 += h1; h4 += h1;

	h1 = fmix(h1);
	h2 = fmix(h2);
	h3 = fmix(h3);
	h4 = fmix(h4);

	h1 += h2; h1 += h3; h1 += h4;
	h2 += h1; h3 += h1; h4 += h1;

	((uint32_t*)out)[0] = h1;
	((uint32_t*)out)[1] = h2;
	((uint32_t*)out)[2] = h3;
	((uint32_t*)out)[3] = h4;
      }

      //-----------------------------------------------------------------------------

      void murmurhash3_x64_128(const void *const __restrict__ key, const size_t len,
			       const size_t seed, const void *__restrict__ out )
      {
	const uint8_t *const __restrict__ data = static_cast<const uint8_t*>(key);
	const int nblocks = len / 16;

	uint64_t h1 = seed;
	uint64_t h2 = seed;

	static const uint64_t c1 = 0x87c37b91114253d5;
	static const uint64_t c2 = 0x4cf5ad432745937f;

	//----------
	// body

	const uint64_t *const __restrict__ blocks = reinterpret_cast<const uint64_t *>(data);

	for(int i = 0; i < nblocks; i++)
	  {
	    uint64_t k1 = getblock(blocks,i*2+0);
	    uint64_t k2 = getblock(blocks,i*2+1);

	    k1 *= c1; k1  = rotl64(k1,31); k1 *= c2; h1 ^= k1;

	    h1 = rotl64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

	    k2 *= c2; k2  = rotl64(k2,33); k2 *= c1; h2 ^= k2;

	    h2 = rotl64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
	  }

	//----------
	// tail

	const uint8_t *const __restrict__ tail = static_cast<const uint8_t*>(data + nblocks*16);

	uint64_t k1 = 0;
	uint64_t k2 = 0;

	switch(len & 15)
	  {
	  case 15: k2 ^= static_cast<uint64_t>(tail[14]) << 48;
	  case 14: k2 ^= static_cast<uint64_t>(tail[13]) << 40;
	  case 13: k2 ^= static_cast<uint64_t>(tail[12]) << 32;
	  case 12: k2 ^= static_cast<uint64_t>(tail[11]) << 24;
	  case 11: k2 ^= static_cast<uint64_t>(tail[10]) << 16;
	  case 10: k2 ^= static_cast<uint64_t>(tail[ 9]) << 8;
	  case  9: k2 ^= static_cast<uint64_t>(tail[ 8]) << 0;
	    k2 *= c2; k2  = rotl64(k2,33); k2 *= c1; h2 ^= k2;

	  case  8: k1 ^= static_cast<uint64_t>(tail[ 7]) << 56;
	  case  7: k1 ^= static_cast<uint64_t>(tail[ 6]) << 48;
	  case  6: k1 ^= static_cast<uint64_t>(tail[ 5]) << 40;
	  case  5: k1 ^= static_cast<uint64_t>(tail[ 4]) << 32;
	  case  4: k1 ^= static_cast<uint64_t>(tail[ 3]) << 24;
	  case  3: k1 ^= static_cast<uint64_t>(tail[ 2]) << 16;
	  case  2: k1 ^= static_cast<uint64_t>(tail[ 1]) << 8;
	  case  1: k1 ^= static_cast<uint64_t>(tail[ 0]) << 0;
	    k1 *= c1; k1  = rotl64(k1,31); k1 *= c2; h1 ^= k1;
	  };

	//----------
	// finalization

	h1 ^= len; h2 ^= len;

	h1 += h2;
	h2 += h1;

	h1 = fmix(h1);
	h2 = fmix(h2);

	h1 += h2;
	h2 += h1;

	((uint64_t*)out)[0] = h1;
	((uint64_t*)out)[1] = h2;
      }
    }// detail
  }// hash
}// boost
