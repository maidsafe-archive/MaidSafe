/* Copyright (c) 2010 maidsafe.net limited
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

#include "maidsafe/common/log.h"
#include <string>
#include "boost/filesystem/path.hpp"
#include "google/protobuf/stubs/common.h"


namespace maidsafe {

namespace {

void ProtobufLogHandler(google::protobuf::LogLevel level,
                        const char *filename,
                        int line,
                        const std::string &message) {
  boost::filesystem::path full(filename);
  boost::filesystem::path parent(full.parent_path().filename());
  boost::filesystem::path slash("/");
  std::string full_message("[... ");
  full_message += (slash.make_preferred() / parent / full.filename()).string();
  full_message += ":" + std::to_string(static_cast<uint64_t>(line)) + "] ";
  full_message += message;

  switch (level) {
    // Protobuf throws an exception containing the message for LOGLEVEL_FATAL
    // level, in which case the message should be received and output in the
    // appropriate catch block, so no need to output message here.
    case google::protobuf::LOGLEVEL_FATAL:
      break;
    case google::protobuf::LOGLEVEL_ERROR:
      DLOG(ERROR) << full_message;
      break;
    case google::protobuf::LOGLEVEL_WARNING:
      DLOG(WARNING) << full_message;
      break;
    case google::protobuf::LOGLEVEL_INFO:
    default:
      DLOG(INFO) << full_message;
      break;
  }
}

}  // unnamed namespace


void InitLogging(const char *argv0) {
  google::InitGoogleLogging(argv0);
  google::protobuf::SetLogHandler(maidsafe::ProtobufLogHandler);
}

}  // namespace maidsafe

int FLAGS_ms_logging_user = 3;
int FLAGS_ms_logging_benchmark = 3;

int FLAGS_ms_logging_common = 3;
int FLAGS_ms_logging_private = 3;
int FLAGS_ms_logging_transport = 3;
int FLAGS_ms_logging_rudp = 3;
int FLAGS_ms_logging_encrypt = 3;
int FLAGS_ms_logging_dht = 3;
int FLAGS_ms_logging_routing = 3;
int FLAGS_ms_logging_pki = 3;
int FLAGS_ms_logging_passport = 3;
int FLAGS_ms_logging_pd = 3;
int FLAGS_ms_logging_lifestuff = 3;
int FLAGS_ms_logging_lifestuff_gui = 3;
int FLAGS_ms_logging_file_browser = 3;
int FLAGS_ms_logging_drive = 3;
int FLAGS_ms_logging_crash_utils = 3;

int FLAGS_ms_logging_sigmoid_storage_director = 3;
int FLAGS_ms_logging_sigmoid_core = 3;
int FLAGS_ms_logging_sigmoid_pro = 3;
