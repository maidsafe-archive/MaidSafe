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

#include "boost/filesystem.hpp"
#include "maidsafe/common/test.h"
#include "maidsafe/rudp/log.h"


int main(int argc, char **argv) {
  // Initialising logging
  maidsafe::InitLogging(argv[0]);

  // Choose to direct output to stderr instead of logfiles.
  FLAGS_logtostderr = true;

  // Choose to direct output to stderr as well as to logfiles.
  FLAGS_alsologtostderr = false;

  // Prepend the log prefix to the start of each log line
  FLAGS_log_prefix = true;

  // Logfiles are written into this directory instead of the default logging one
  boost::system::error_code error_code;
  FLAGS_log_dir = boost::filesystem::temp_directory_path(error_code).string();

  // Show all VLOG(m) messages for m <= this.
  FLAGS_v = 0;

  // Log messages at or above this level. Severity levels are INFO, WARNING,
  // ERROR, and FATAL (0 to 3 respectively).
  FLAGS_ms_logging_common = google::INFO;
  FLAGS_ms_logging_rudp = google::INFO;

  testing::InitGoogleTest(&argc, argv);
  int result(RUN_ALL_TESTS());
  int test_count = testing::UnitTest::GetInstance()->test_to_run_count();
  return (test_count == 0) ? -1 : result;
}
