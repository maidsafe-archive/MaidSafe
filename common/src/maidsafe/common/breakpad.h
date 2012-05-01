/* Copyright (c) 2011 maidsafe.net limited
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

#ifndef MAIDSAFE_COMMON_BREAKPAD_H_
#define MAIDSAFE_COMMON_BREAKPAD_H_

#include <string>

#ifdef WIN32
#  include "breakpad/client/windows/handler/exception_handler.h"
#else
#  include "breakpad/client/linux/handler/exception_handler.h"
#endif

#include "maidsafe/common/version.h"

#if MAIDSAFE_COMMON_VERSION != 1200
#  error This API is not compatible with the installed library.\
    Please update the MaidSafe-Common library.
#endif


namespace maidsafe {

namespace crash_report {

#ifdef WIN32
bool DumpCallback(const wchar_t* dump_path,
                  const wchar_t* minidump_id,
                  void* context,
                  EXCEPTION_POINTERS* /*exinfo*/,
                  MDRawAssertionInfo* /*assertion*/,
                  bool succeeded);
#else
bool DumpCallback(const char* dump_path,
                  const char* minidump_id,
                  void* context,
                  bool succeeded);
#endif

struct ProjectInfo {
  ProjectInfo(std::string project_name, std::string project_version);
  std::string version;
  std::string name;
};

}  // namespace crash_report

}  // namespace maidsafe

#endif  // MAIDSAFE_COMMON_BREAKPAD_H_
