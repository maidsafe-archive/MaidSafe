#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ > 8) // only if gcc 4.9 or higher
#if CRYPTOPP_BOOL_AESNI_INTRINSICS_AVAILABLE

#ifndef EXCLUDE_FIRST_HALF // This ones are redefined in rijndael.cpp if not excluded
#if !defined(__GNUC__) || defined(__SSSE3__) || defined(__INTEL_COMPILER)
#include <tmmintrin.h>
#else
__inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_shuffle_epi8 (__m128i a, __m128i b)
{
	asm ("pshufb %1, %0" : "+x"(a) : "xm"(b));
  	return a;
}
#endif
#if !defined(__GNUC__) || (defined(__AES__) && defined(__PCLMUL__)) || defined(__INTEL_COMPILER)
#include <wmmintrin.h>
#else
__inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_clmulepi64_si128 (__m128i a, __m128i b, const int i)
{
	asm ("pclmulqdq %2, %1, %0" : "+x"(a) : "xm"(b), "i"(i));
  	return a;
}
#endif
#endif // EXCLUDE_FIRST_HALF

#if !defined(__GNUC__) || defined(__SSE4_1__) || defined(__INTEL_COMPILER)
#include <smmintrin.h>
#else
__inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_extract_epi32 (__m128i a, const int i)
{
	int r;
	asm ("pextrd %2, %1, %0" : "=rm"(r) : "x"(a), "i"(i));
  	return r;
}
__inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_insert_epi32 (__m128i a, int b, const int i)
{
	asm ("pinsrd %2, %1, %0" : "+x"(a) : "rm"(b), "i"(i));
  	return a;
}
#endif
#if !defined(__GNUC__) || (defined(__AES__) && defined(__PCLMUL__)) || defined(__INTEL_COMPILER)
#include <wmmintrin.h>
#else
__inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aeskeygenassist_si128 (__m128i a, const int i)
{
	__m128i r;
	asm ("aeskeygenassist %2, %1, %0" : "=x"(r) : "xm"(a), "i"(i));
  	return r;
}
__inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesimc_si128 (__m128i a)
{
	__m128i r;
	asm ("aesimc %1, %0" : "=x"(r) : "xm"(a));
  	return r;
}
__inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesenc_si128 (__m128i a, __m128i b)
{
	asm ("aesenc %1, %0" : "+x"(a) : "xm"(b));
  	return a;
}
__inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesenclast_si128 (__m128i a, __m128i b)
{
	asm ("aesenclast %1, %0" : "+x"(a) : "xm"(b));
  	return a;
}
__inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesdec_si128 (__m128i a, __m128i b)
{
	asm ("aesdec %1, %0" : "+x"(a) : "xm"(b));
  	return a;
}
__inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesdeclast_si128 (__m128i a, __m128i b)
{
	asm ("aesdeclast %1, %0" : "+x"(a) : "xm"(b));
  	return a;
}
#endif

#endif// CRYPTOPP_BOOL_AESNI_INTRINSICS_AVAILABLE
#endif// gcc 4.9
