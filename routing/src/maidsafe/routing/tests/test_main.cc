/*******************************************************************************
 *  Copyright 2012 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ******************************************************************************/

#include "maidsafe/common/test.h"
#include "maidsafe/routing/log.h"

int main(int argc, char **argv) {
  maidsafe::InitLogging(argv[0]);
  FLAGS_logtostderr = true;
  FLAGS_alsologtostderr = false;
  FLAGS_log_prefix = true;
  FLAGS_ms_logging_common = google::FATAL;
  FLAGS_ms_logging_rudp = google::FATAL;
  FLAGS_ms_logging_routing = google::INFO;

  testing::InitGoogleTest(&argc, argv);
  int result(RUN_ALL_TESTS());
  int test_count = testing::UnitTest::GetInstance()->test_to_run_count();
  return (test_count == 0) ? -1 : result;
}
