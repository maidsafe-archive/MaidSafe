/*******************************************************************************
 *  Copyright 2012 MaidSafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of MaidSafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of MaidSafe.net. *
 ******************************************************************************/
// Original author: Christopher M. Kohlhoff (chris at kohlhoff dot com)

#include "maidsafe/common/test.h"
#include "maidsafe/rudp/log.h"
#include "maidsafe/rudp/core/sliding_window.h"

namespace maidsafe {

namespace rudp {

namespace detail {

namespace test {

static const size_t kTestPacketCount = 100000;

static void TestWindowRange(uint32_t first_sequence_number) {
  SlidingWindow<uint32_t> window(first_sequence_number);

  for (size_t i = 0; i < window.MaximumSize(); ++i) {
    uint32_t n = window.Append();
    window[n] = n;
  }

  for (size_t i = 0; i < kTestPacketCount; ++i) {
    ASSERT_EQ(window.Begin(), window[window.Begin()]);
    window.Remove();
    uint32_t n = window.Append();
    window[n] = n;
  }

  for (size_t i = 0; i < window.MaximumSize(); ++i) {
    ASSERT_EQ(window.Begin(), window[window.Begin()]);
    window.Remove();
  }
}

TEST(SlidingWindowTest, BEH_FromZero) {
  TestWindowRange(0);
}

TEST(SlidingWindowTest, BEH_FromN) {
  TestWindowRange(123456);
}

TEST(SlidingWindowTest, BEH_Wraparound) {
  TestWindowRange(SlidingWindow<uint32_t>::kMaxSequenceNumber -
                  kTestPacketCount / 2);
}

}  // namespace test

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe
