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

// #include "boost/filesystem/convenience.hpp"
#include "maidsafe/common/test.h"
#include "maidsafe/common/log.h"

int main(int argc, char **argv) {
  // Initialising logging
  maidsafe::InitLogging(argv[0]);

  // Choose to direct output to stderr instead of logfiles.
  FLAGS_logtostderr = true;

  // Choose to direct output to stderr as well as to logfiles.
  FLAGS_alsologtostderr = false;

  // Log messages at or above this level. Severity levels are INFO, WARNING,
  // ERROR, and FATAL (0 to 3 respectively).
  FLAGS_ms_logging_common = google::FATAL;

  // Prepend the log prefix to the start of each log line
  FLAGS_log_prefix = true;

  // Logfiles are written into this directory instead of the default logging one
//  FLAGS_log_dir = boost::filesystem::temp_directory_path().string();

  // Show all VLOG(m) messages for m <= this.
  FLAGS_v = 0;

  testing::InitGoogleTest(&argc, argv);
  int result(RUN_ALL_TESTS());
  int test_count = testing::UnitTest::GetInstance()->test_to_run_count();
  return (test_count == 0) ? -1 : result;
}
