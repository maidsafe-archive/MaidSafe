/* Copyright (c) 2009 maidsafe.net limited
//  Copyright (c) 2001, 2002 Peter Dimov and Multi Media Ltd.
//  Copyright (c) 2001 David Abrahams
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "boost/config.hpp"
#include "maidsafe/common/test.h"

#if defined(BOOST_MSVC)
#pragma warning(disable: 4786)  // identifier truncated in debug info
#pragma warning(disable: 4710)  // function not inlined
#pragma warning(disable: 4711)  // function selected for automatic inline expnsn
#pragma warning(disable: 4514)  // unreferenced inline removed
#endif

#include "boost/bind.hpp"
#include "boost/ref.hpp"

#if defined(BOOST_MSVC) && (BOOST_MSVC < 1300)
#pragma warning(push, 3)
#endif

#if defined(BOOST_MSVC) && (BOOST_MSVC < 1300)
#pragma warning(pop)
#endif

#include "boost/detail/lightweight_test.hpp"

#include <iostream>  // NOLINT

long f_0() {  // NOLINT
  return 17041L;
}

long f_1(long a) {  // NOLINT
  return a;
}

long f_2(long a, long b) {  // NOLINT
  return a + 10 * b;
}

long f_3(long a, long b, long c) {  // NOLINT
  return a + 10 * b + 100 * c;
}

long f_4(long a, long b, long c, long d) {  // NOLINT
  return a + 10 * b + 100 * c + 1000 * d;
}

long f_5(long a, long b, long c, long d, long e) {  // NOLINT
  return a + 10 * b + 100 * c + 1000 * d + 10000 * e;
}

long f_6(long a, long b, long c, long d, long e, long f) {  // NOLINT
  return a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f;
}

long f_7(long a, long b, long c, long d, long e, long f, long g) {  // NOLINT
  return a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f + 1000000 * g;
}

long f_8(long a, long b, long c, long d, long e, long f, long g, long h) {  // NOLINT
  return a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f + 1000000 * g
         + 10000000 * h;
}

long f_9(long a, long b, long c, long d, long e, long f, long g, long h, long i) {  // NOLINT
  return a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f + 1000000 * g
         + 10000000 * h + 100000000 * i;
}

long global_result;  // NOLINT

void fv_0() {
  global_result = 17041L;
}

void fv_1(long a) {  // NOLINT
  global_result = a;
}

void fv_2(long a, long b) {  // NOLINT
  global_result = a + 10 * b;
}

void fv_3(long a, long b, long c) {  // NOLINT
  global_result = a + 10 * b + 100 * c;
}

void fv_4(long a, long b, long c, long d) {  // NOLINT
  global_result = a + 10 * b + 100 * c + 1000 * d;
}

void fv_5(long a, long b, long c, long d, long e) {  // NOLINT
  global_result = a + 10 * b + 100 * c + 1000 * d + 10000 * e;
}

void fv_6(long a, long b, long c, long d, long e, long f) {  // NOLINT
  global_result = a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f;
}

void fv_7(long a, long b, long c, long d, long e, long f, long g) {  // NOLINT
  global_result = a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f +
                  1000000 * g;
}

void fv_8(long a, long b, long c, long d, long e, long f, long g, long h) {  // NOLINT
  global_result = a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f +
                  1000000 * g + 10000000 * h;
}

void fv_9(long a, long b, long c, long d, long e, long f, long g, long h, long i) {  // NOLINT
  global_result = a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f +
                  1000000 * g + 10000000 * h + 100000000 * i;
}

TEST(boost, BEH_BOOST_bind_function_simple) {
// Simple check that bound functions are executing with correct results
  int const i = 1;

  EXPECT_EQ(boost::bind(f_0)(i), 17041L);
  EXPECT_EQ(boost::bind(f_1, _1)(i), 1L);
  EXPECT_EQ(boost::bind(f_2, _1, 2)(i), 21L);
  EXPECT_EQ(boost::bind(f_3, _1, 2, 3)(i), 321L);
  EXPECT_EQ(boost::bind(f_4, _1, 2, 3, 4)(i), 4321L);
  EXPECT_EQ(boost::bind(f_5, _1, 2, 3, 4, 5)(i), 54321L);
  EXPECT_EQ(boost::bind(f_6, _1, 2, 3, 4, 5, 6)(i), 654321L);
  EXPECT_EQ(boost::bind(f_7, _1, 2, 3, 4, 5, 6, 7)(i), 7654321L);
  EXPECT_EQ(boost::bind(f_8, _1, 2, 3, 4, 5, 6, 7, 8)(i), 87654321L);
  EXPECT_EQ(boost::bind(f_9, _1, 2, 3, 4, 5, 6, 7, 8, 9)(i), 987654321L);

  boost::bind(fv_0)(i);
  EXPECT_EQ(global_result, 17041L);
  boost::bind(fv_1, _1)(i);
  EXPECT_EQ(global_result, 1L);
  boost::bind(fv_2, _1, 2)(i);
  EXPECT_EQ(global_result, 21L);
  boost::bind(fv_3, _1, 2, 3)(i);
  EXPECT_EQ(global_result, 321L);
  boost::bind(fv_4, _1, 2, 3, 4)(i);
  EXPECT_EQ(global_result, 4321L);
  boost::bind(fv_5, _1, 2, 3, 4, 5)(i);
  EXPECT_EQ(global_result, 54321L);
  boost::bind(fv_6, _1, 2, 3, 4, 5, 6)(i);
  EXPECT_EQ(global_result, 654321L);
  boost::bind(fv_7, _1, 2, 3, 4, 5, 6, 7)(i);
  EXPECT_EQ(global_result, 7654321L);
  boost::bind(fv_8, _1, 2, 3, 4, 5, 6, 7, 8)(i);
  EXPECT_EQ(global_result, 87654321L);
  boost::bind(fv_9, _1, 2, 3, 4, 5, 6, 7, 8, 9)(i);
  EXPECT_EQ(global_result, 987654321L);
}

struct Y {
  short operator()(short & r) const {  // NOLINT
    return ++r;
  }
  int operator()(int a, int b) const {
    return a + 10 * b;
  }
  long operator() (long a, long b, long c) const {  // NOLINT
    return a + 10 * b + 100 * c;
  }
  void operator() (long a, long b, long c, long d) const {  // NOLINT
    global_result = a + 10 * b + 100 * c + 1000 * d;
  }
};

TEST(boost, BEH_BOOST_bind_function_object1) {
// Test boost::binding a function object (struct Y in this case)
// Doc
// The expression boost::ref(x) returns a boost::reference_wrapper<X>(x)
// where X is the type of x. Similarly, boost::cref(x) returns a
// boost::reference_wrapper<X const>(x).

  short i(6);  // NOLINT
  int const k = 3;
  EXPECT_EQ(boost::bind<short>(Y(), boost::ref(i))(), 7);  // NOLINT
  EXPECT_EQ(boost::bind<short>(Y(), boost::ref(i))(), 8);  // NOLINT
  EXPECT_EQ(boost::bind<int>(Y(), i, _1)(k), 38);
  EXPECT_EQ(boost::bind<long>(Y(), i, _1, 9)(k), 938);  // NOLINT

// Fails for this version of the compiler.
#if !defined(__MWERKS__) || (__MWERKS__ > 0x2407)
  global_result = 0;
  boost::bind<void>(Y(), i, _1, 9, 4)(k);
  EXPECT_EQ(global_result, 4938);
#endif
}

TEST(boost, BEH_BOOST_bind_function_object2) {
// Same test really using the () operator and boost::type
// to indicate the function type as oppsed to simple boost::bind<type> as above
// Doc
// The Boost type-traits library contains a set of very specific traits classes,
// each of which encapsulate a single trait from the C++ type system; for
// example, is a type a pointer or a boost::reference type? Or does a type have
// a trivial constructor, or a const-qualifier?
  short i(6);  // NOLINT
  int const k = 3;

  EXPECT_EQ(boost::bind(boost::type<short>(), Y(), boost::ref(i))(), 7);  // NOLINT
  EXPECT_EQ(boost::bind(boost::type<short>(), Y(), boost::ref(i))(), 8);  // NOLINT
  EXPECT_EQ(boost::bind(boost::type<int>(), Y(), i, _1)(k), 38);
  EXPECT_EQ(boost::bind(boost::type<long>(), Y(), i, _1, 9)(k), 938);  // NOLINT
  global_result = 0;
  boost::bind(boost::type<void>(), Y(), i, _1, 9, 4)(k);
  EXPECT_EQ(global_result, 4938);
}

#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(BOOST_NO_FUNCTION_TEMPLATE_ORDERING)  // NOLINT

struct Z {
  typedef int result_type;
  int operator()(int a, int b) const { return a + 10 * b; }
};

void adaptable_function_object_test() {
  EXPECT_EQ(boost::bind(Z(), 7, 4)(), 47);
}

#endif

struct X {
  mutable unsigned int hash;
// The keyword mutable is used to allow a particular data member of const object
// to be modified.
  X(): hash(0) {}

  int f0() {
    f1(17);
    return 0;
  }
  int g0() const {
    g1(17);
    return 0;
  }

  int f1(int a1) {
    hash = (hash * 17041 + a1) % 32768;
    return 0;
  }
  int g1(int a1) const {
    hash = (hash * 17041 + a1 * 2) % 32768;
    return 0;
  }

  int f2(int a1, int a2) {
    f1(a1);
    f1(a2);
    return 0;
  }
  int g2(int a1, int a2) const {
    g1(a1);
    g1(a2);
    return 0;
  }

  int f3(int a1, int a2, int a3) {
    f2(a1, a2);
    f1(a3);
    return 0;
  }
  int g3(int a1, int a2, int a3) const {
    g2(a1, a2);
    g1(a3);
    return 0;
  }

  int f4(int a1, int a2, int a3, int a4) {
    f3(a1, a2, a3);
    f1(a4);
    return 0;
  }
  int g4(int a1, int a2, int a3, int a4) const {
    g3(a1, a2, a3);
    g1(a4);
    return 0;
  }

  int f5(int a1, int a2, int a3, int a4, int a5) {
    f4(a1, a2, a3, a4);
    f1(a5);
    return 0;
  }
  int g5(int a1, int a2, int a3, int a4, int a5) const {
    g4(a1, a2, a3, a4);
    g1(a5);
    return 0;
  }

  int f6(int a1, int a2, int a3, int a4, int a5, int a6) {
    f5(a1, a2, a3, a4, a5);
    f1(a6);
    return 0;
  }
  int g6(int a1, int a2, int a3, int a4, int a5, int a6) const {
    g5(a1, a2, a3, a4, a5);
    g1(a6);
    return 0;
  }

  int f7(int a1, int a2, int a3, int a4, int a5, int a6, int a7) {
    f6(a1, a2, a3, a4, a5, a6);
    f1(a7);
    return 0;
  }
  int g7(int a1, int a2, int a3, int a4, int a5, int a6, int a7) const {
    g6(a1, a2, a3, a4, a5, a6);
    g1(a7);
    return 0;
  }

  int f8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) {
    f7(a1, a2, a3, a4, a5, a6, a7);
    f1(a8);
    return 0;
  }
  int g8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) const {
    g7(a1, a2, a3, a4, a5, a6, a7);
    g1(a8);
    return 0;
  }
};

struct V {
  mutable unsigned int hash;
// The keyword mutable is used to allow a particular data member of const object
// to be modified.
  V(): hash(0) {}

  void f0() {
    f1(17);
  }
  void g0() const {
    g1(17);
  }

  void f1(int a1) {
    hash = (hash * 17041 + a1) % 32768;
  }
  void g1(int a1) const {
    hash = (hash * 17041 + a1 * 2) % 32768;
  }

  void f2(int a1, int a2) {
    f1(a1);
    f1(a2);
  }
  void g2(int a1, int a2) const {
    g1(a1);
    g1(a2);
  }

  void f3(int a1, int a2, int a3) {
    f2(a1, a2);
    f1(a3);
  }
  void g3(int a1, int a2, int a3) const {
    g2(a1, a2);
    g1(a3);
  }

  void f4(int a1, int a2, int a3, int a4) {
    f3(a1, a2, a3);
    f1(a4);
  }
  void g4(int a1, int a2, int a3, int a4) const {
    g3(a1, a2, a3);
    g1(a4);
  }

  void f5(int a1, int a2, int a3, int a4, int a5) {
    f4(a1, a2, a3, a4);
    f1(a5);
  }
  void g5(int a1, int a2, int a3, int a4, int a5) const {
    g4(a1, a2, a3, a4);
    g1(a5);
  }

  void f6(int a1, int a2, int a3, int a4, int a5, int a6) {
    f5(a1, a2, a3, a4, a5);
    f1(a6);
  }
  void g6(int a1, int a2, int a3, int a4, int a5, int a6) const {
    g5(a1, a2, a3, a4, a5);
    g1(a6);
  }

  void f7(int a1, int a2, int a3, int a4, int a5, int a6, int a7) {
    f6(a1, a2, a3, a4, a5, a6);
    f1(a7);
  }
  void g7(int a1, int a2, int a3, int a4, int a5, int a6, int a7) const {
    g6(a1, a2, a3, a4, a5, a6);
    g1(a7);
  }

  void f8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) {
    f7(a1, a2, a3, a4, a5, a6, a7);
    f1(a8);
  }
  void g8(int a1, int a2, int a3, int a4, int a5, int a6, int a7,
          int a8) const {
    g7(a1, a2, a3, a4, a5, a6, a7);
    g1(a8);
  }
};


TEST(boost, BEH_BOOST_bind_member_function_non_void) {
  X x;

  // 0

  boost::bind(&X::f0, &x)();
  boost::bind(&X::f0, boost::ref(x))();

  boost::bind(&X::g0, &x)();
  boost::bind(&X::g0, x)();
  boost::bind(&X::g0, boost::ref(x))();

  // 1

  boost::bind(&X::f1, &x, 1)();
  boost::bind(&X::f1, boost::ref(x), 1)();

  boost::bind(&X::g1, &x, 1)();
  boost::bind(&X::g1, x, 1)();
  boost::bind(&X::g1, boost::ref(x), 1)();

  // 2

  boost::bind(&X::f2, &x, 1, 2)();
  boost::bind(&X::f2, boost::ref(x), 1, 2)();

  boost::bind(&X::g2, &x, 1, 2)();
  boost::bind(&X::g2, x, 1, 2)();
  boost::bind(&X::g2, boost::ref(x), 1, 2)();

  // 3

  boost::bind(&X::f3, &x, 1, 2, 3)();
  boost::bind(&X::f3, boost::ref(x), 1, 2, 3)();

  boost::bind(&X::g3, &x, 1, 2, 3)();
  boost::bind(&X::g3, x, 1, 2, 3)();
  boost::bind(&X::g3, boost::ref(x), 1, 2, 3)();

  // 4

  boost::bind(&X::f4, &x, 1, 2, 3, 4)();
  boost::bind(&X::f4, boost::ref(x), 1, 2, 3, 4)();

  boost::bind(&X::g4, &x, 1, 2, 3, 4)();
  boost::bind(&X::g4, x, 1, 2, 3, 4)();
  boost::bind(&X::g4, boost::ref(x), 1, 2, 3, 4)();

  // 5

  boost::bind(&X::f5, &x, 1, 2, 3, 4, 5)();
  boost::bind(&X::f5, boost::ref(x), 1, 2, 3, 4, 5)();

  boost::bind(&X::g5, &x, 1, 2, 3, 4, 5)();
  boost::bind(&X::g5, x, 1, 2, 3, 4, 5)();
  boost::bind(&X::g5, boost::ref(x), 1, 2, 3, 4, 5)();

  // 6

  boost::bind(&X::f6, &x, 1, 2, 3, 4, 5, 6)();
  boost::bind(&X::f6, boost::ref(x), 1, 2, 3, 4, 5, 6)();

  boost::bind(&X::g6, &x, 1, 2, 3, 4, 5, 6)();
  boost::bind(&X::g6, x, 1, 2, 3, 4, 5, 6)();
  boost::bind(&X::g6, boost::ref(x), 1, 2, 3, 4, 5, 6)();

  // 7

  boost::bind(&X::f7, &x, 1, 2, 3, 4, 5, 6, 7)();
  boost::bind(&X::f7, boost::ref(x), 1, 2, 3, 4, 5, 6, 7)();

  boost::bind(&X::g7, &x, 1, 2, 3, 4, 5, 6, 7)();
  boost::bind(&X::g7, x, 1, 2, 3, 4, 5, 6, 7)();
  boost::bind(&X::g7, boost::ref(x), 1, 2, 3, 4, 5, 6, 7)();

  // 8

  boost::bind(&X::f8, &x, 1, 2, 3, 4, 5, 6, 7, 8)();
  boost::bind(&X::f8, boost::ref(x), 1, 2, 3, 4, 5, 6, 7, 8)();

  boost::bind(&X::g8, &x, 1, 2, 3, 4, 5, 6, 7, 8)();
  boost::bind(&X::g8, x, 1, 2, 3, 4, 5, 6, 7, 8)();
  boost::bind(&X::g8, boost::ref(x), 1, 2, 3, 4, 5, 6, 7, 8)();

  EXPECT_EQ(x.hash, 23558);
}


TEST(boost, BEH_BOOST_bind_member_function_void) {
  V v;

  // 0

  boost::bind(&V::f0, &v)();
  boost::bind(&V::f0, boost::ref(v))();

  boost::bind(&V::g0, &v)();
  boost::bind(&V::g0, v)();
  boost::bind(&V::g0, boost::ref(v))();

  // 1

  boost::bind(&V::f1, &v, 1)();
  boost::bind(&V::f1, boost::ref(v), 1)();

  boost::bind(&V::g1, &v, 1)();
  boost::bind(&V::g1, v, 1)();
  boost::bind(&V::g1, boost::ref(v), 1)();

  // 2

  boost::bind(&V::f2, &v, 1, 2)();
  boost::bind(&V::f2, boost::ref(v), 1, 2)();

  boost::bind(&V::g2, &v, 1, 2)();
  boost::bind(&V::g2, v, 1, 2)();
  boost::bind(&V::g2, boost::ref(v), 1, 2)();

  // 3

  boost::bind(&V::f3, &v, 1, 2, 3)();
  boost::bind(&V::f3, boost::ref(v), 1, 2, 3)();

  boost::bind(&V::g3, &v, 1, 2, 3)();
  boost::bind(&V::g3, v, 1, 2, 3)();
  boost::bind(&V::g3, boost::ref(v), 1, 2, 3)();

  // 4

  boost::bind(&V::f4, &v, 1, 2, 3, 4)();
  boost::bind(&V::f4, boost::ref(v), 1, 2, 3, 4)();

  boost::bind(&V::g4, &v, 1, 2, 3, 4)();
  boost::bind(&V::g4, v, 1, 2, 3, 4)();
  boost::bind(&V::g4, boost::ref(v), 1, 2, 3, 4)();

  // 5

  boost::bind(&V::f5, &v, 1, 2, 3, 4, 5)();
  boost::bind(&V::f5, boost::ref(v), 1, 2, 3, 4, 5)();

  boost::bind(&V::g5, &v, 1, 2, 3, 4, 5)();
  boost::bind(&V::g5, v, 1, 2, 3, 4, 5)();
  boost::bind(&V::g5, boost::ref(v), 1, 2, 3, 4, 5)();

  // 6

  boost::bind(&V::f6, &v, 1, 2, 3, 4, 5, 6)();
  boost::bind(&V::f6, boost::ref(v), 1, 2, 3, 4, 5, 6)();

  boost::bind(&V::g6, &v, 1, 2, 3, 4, 5, 6)();
  boost::bind(&V::g6, v, 1, 2, 3, 4, 5, 6)();
  boost::bind(&V::g6, boost::ref(v), 1, 2, 3, 4, 5, 6)();

  // 7

  boost::bind(&V::f7, &v, 1, 2, 3, 4, 5, 6, 7)();
  boost::bind(&V::f7, boost::ref(v), 1, 2, 3, 4, 5, 6, 7)();

  boost::bind(&V::g7, &v, 1, 2, 3, 4, 5, 6, 7)();
  boost::bind(&V::g7, v, 1, 2, 3, 4, 5, 6, 7)();
  boost::bind(&V::g7, boost::ref(v), 1, 2, 3, 4, 5, 6, 7)();

  // 8

  boost::bind(&V::f8, &v, 1, 2, 3, 4, 5, 6, 7, 8)();
  boost::bind(&V::f8, boost::ref(v), 1, 2, 3, 4, 5, 6, 7, 8)();

  boost::bind(&V::g8, &v, 1, 2, 3, 4, 5, 6, 7, 8)();
  boost::bind(&V::g8, v, 1, 2, 3, 4, 5, 6, 7, 8)();
  boost::bind(&V::g8, boost::ref(v), 1, 2, 3, 4, 5, 6, 7, 8)();

  EXPECT_EQ(v.hash, 23558);
}


TEST(boost, BEH_BOOST_nested_bind) {
  int const x = 1;
  int const y = 2;

  EXPECT_EQ(boost::bind(f_1, boost::bind(f_1, _1))(x), 1L);
  EXPECT_EQ(boost::bind(f_1, boost::bind(f_2, _1, _2))(x, y), 21L);
  EXPECT_EQ(boost::bind(f_2, boost::bind(f_1, _1), boost::bind(f_1, _1))(x),
            11L);
  EXPECT_EQ(boost::bind(f_2, boost::bind(f_1, _1), boost::bind(f_1, _2))(x, y),
            21L);
  EXPECT_EQ(boost::bind(f_1, boost::bind(f_0))(), 17041L);

  boost::bind(fv_1, boost::bind(f_1, _1))(x);
  EXPECT_EQ(global_result, 1L);
  boost::bind(fv_1, boost::bind(f_2, _1, _2))(x, y);
  EXPECT_EQ(global_result, 21L);
  boost::bind(fv_2, boost::bind(f_1, _1), boost::bind(f_1, _1))(x);
  EXPECT_EQ(global_result, 11L);
  boost::bind(fv_2, boost::bind(f_1, _1), boost::bind(f_1, _2))(x, y);
  EXPECT_EQ(global_result, 21L);
  boost::bind(fv_1, boost::bind(f_0))();
  EXPECT_EQ(global_result, 17041L);
}
