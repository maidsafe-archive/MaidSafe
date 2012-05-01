/* Copyright (c) 2009 maidsafe.net limited
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

#include <cstddef>
#include "boost/detail/lightweight_test.hpp"
#include "boost/detail/atomic_count.hpp"
#include "boost/make_shared.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "maidsafe/common/test.h"

class X {
 public:
  explicit X(int a1,
             int a2,
             int a3,
             int a4,
             int a5,
             int a6,
             int a7,
             int a8,
             int a9)
      : v(a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9) {
    ++instances;
  }

  ~X() {
    --instances;
  }

  static int instances;
  int v;

 private:
  X(X const &);  // NOLINT

  X & operator=(X const &);

  void * operator new(std::size_t n) {
    // lack of this definition causes link errors on Comeau C++
    BOOST_ERROR("private X::new called");
    return ::operator new(n);
  }

  void operator delete(void * p) {
    // lack of this definition causes link errors on MSVC
    BOOST_ERROR("private X::delete called");
    ::operator delete(p);
  }
};

int X::instances = 0;

TEST(boost, BEH_BOOST_smart_ptr_AtomicCount1) {
  boost::detail::atomic_count n(4);
  ASSERT_EQ(n, 4L);
  ++n;
  ASSERT_EQ(n, 5L);
  ASSERT_NE(--n, 0L);
  boost::detail::atomic_count m(0);
  ASSERT_EQ(m, 0);
  ++m;
  ASSERT_EQ(m, 1);
  ++m;
  ASSERT_EQ(m, 2);
  ASSERT_NE(--m, 0);
  ASSERT_EQ(--m, 0);
}

TEST(boost, BEH_BOOST_smart_ptr_AtomicCount2) {
  boost::detail::atomic_count n(4);
  ASSERT_EQ(n, 4);
  ASSERT_EQ(++n, 5);
  ASSERT_EQ(++n, 6);
  ASSERT_EQ(n, 6);
  ASSERT_EQ(--n, 5);
  ASSERT_EQ(--n, 4);
  ASSERT_EQ(--n, 3);
  boost::detail::atomic_count m(0);
  ASSERT_EQ(m, 0);
  ASSERT_EQ(++m, 1);
  ASSERT_EQ(++m, 2);
  ASSERT_EQ(m, 2);
  ASSERT_EQ(--m, 1);
  ASSERT_EQ(--m, 0);
  ASSERT_EQ(--m, -1);
  ASSERT_EQ(--m, -2);
  ASSERT_EQ(--m, -3);
  ASSERT_EQ(--m, -4);
  ASSERT_EQ(++m, -3);
  ASSERT_EQ(--m, -4);
}

TEST(boost, BEH_BOOST_smart_ptr_make_shared) {
  {
    boost::shared_ptr<int> pi = boost::make_shared<int>();
    bool result = pi.get() != 0;
    ASSERT_TRUE(result);
    ASSERT_EQ(*pi, 0);
  }

  {
    boost::shared_ptr<int> pi = boost::make_shared<int>(5);
    bool result = pi.get() != 0;
    ASSERT_TRUE(result);
    ASSERT_EQ(*pi, 5);
  }
  ASSERT_EQ(X::instances, 0);
  {
    boost::shared_ptr<X> pi = boost::make_shared<X>(0, 0, 0, 0, 0, 0, 0, 0, 0);
    boost::weak_ptr<X> wp(pi);
    ASSERT_EQ(X::instances, 1);
    bool result = pi.get() != 0;
    ASSERT_TRUE(result);
    result = pi->v == 0;
    ASSERT_TRUE(result);
    pi.reset();
    ASSERT_EQ(X::instances, 0);
  }

  {
    boost::shared_ptr<X> pi = boost::make_shared<X>(1, 0, 0, 0, 0, 0, 0, 0, 0);
    boost::weak_ptr<X> wp(pi);
    ASSERT_EQ(X::instances, 1);
    bool result = pi.get() != 0;
    ASSERT_TRUE(result);
    result = pi->v == 1;
    ASSERT_TRUE(result);
    pi.reset();
    ASSERT_EQ(X::instances, 0);
  }
  {
    boost::shared_ptr<X> pi = boost::make_shared<X>(1, 2, 0, 0, 0, 0, 0, 0, 0);
    boost::weak_ptr<X> wp(pi);
    ASSERT_EQ(X::instances, 1);
    bool result = pi.get() != 0;
    ASSERT_TRUE(result);
    result = pi->v == 1 + 2;
    ASSERT_TRUE(result);
    pi.reset();
    ASSERT_EQ(X::instances, 0);
  }
  {
    boost::shared_ptr<X> pi = boost::make_shared<X>(1, 2, 3, 0, 0, 0, 0, 0, 0);
    boost::weak_ptr<X> wp(pi);
    ASSERT_EQ(X::instances, 1);
    bool result = pi.get() != 0;
    ASSERT_TRUE(result);
    ASSERT_EQ(pi->v, 1+2+3);
    pi.reset();
    ASSERT_EQ(X::instances, 0);
  }
  {
    boost::shared_ptr<X> pi = boost::make_shared<X>(1, 2, 3, 4, 0, 0, 0, 0, 0);
    boost::weak_ptr<X> wp(pi);
    ASSERT_EQ(X::instances, 1);
    bool result = pi.get() != 0;
    ASSERT_TRUE(result);
    ASSERT_EQ(pi->v, 1+2+3+4);
    pi.reset();
    ASSERT_EQ(X::instances, 0);
  }
  {
    boost::shared_ptr<X> pi = boost::make_shared<X>(1, 2, 3, 4, 5, 0, 0, 0, 0);
    boost::weak_ptr<X> wp(pi);
    ASSERT_EQ(X::instances, 1);
    bool result = pi.get() != 0;
    ASSERT_TRUE(result);
    ASSERT_EQ(pi->v, 1+2+3+4+5);
    pi.reset();
    ASSERT_EQ(X::instances, 0);
  }
  {
    boost::shared_ptr<X> pi = boost::make_shared<X>(1, 2, 3, 4, 5, 6, 0, 0, 0);
    boost::weak_ptr<X> wp(pi);
    ASSERT_EQ(X::instances, 1);
    bool result = pi.get() != 0;
    ASSERT_TRUE(result);
    ASSERT_EQ(pi->v, 1+2+3+4+5+6);
    pi.reset();
    ASSERT_EQ(X::instances, 0);
  }
  {
    boost::shared_ptr<X> pi = boost::make_shared<X>(1, 2, 3, 4, 5, 6, 7, 0, 0);
    boost::weak_ptr<X> wp(pi);
    ASSERT_EQ(X::instances, 1);
    bool result = pi.get() != 0;
    ASSERT_TRUE(result);
    ASSERT_EQ(pi->v, 1+2+3+4+5+6+7);
    pi.reset();
    ASSERT_EQ(X::instances, 0);
  }
  {
    boost::shared_ptr<X> pi = boost::make_shared<X>(1, 2, 3, 4, 5, 6, 7, 8, 0);
    boost::weak_ptr<X> wp(pi);
    ASSERT_EQ(X::instances, 1);
    bool result = pi.get() != 0;
    ASSERT_TRUE(result);
    ASSERT_EQ(pi->v, 1+2+3+4+5+6+7+8);
    pi.reset();
    ASSERT_EQ(X::instances, 0);
  }
  {
    boost::shared_ptr<X> pi = boost::make_shared<X>(1, 2, 3, 4, 5, 6, 7, 8, 9);
    boost::weak_ptr<X> wp(pi);
    ASSERT_EQ(X::instances, 1);
    bool result = pi.get() != 0;
    ASSERT_TRUE(result);
    ASSERT_EQ(pi->v, 1+2+3+4+5+6+7+8+9);
    pi.reset();
    ASSERT_EQ(X::instances, 0);
  }
}
