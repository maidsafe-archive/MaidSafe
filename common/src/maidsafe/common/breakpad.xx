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

#include "maidsafe/common/breakpad.h"
#include "boost/filesystem.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/scoped_array.hpp"
#include "maidsafe/common/log.h"
#include "maidsafe/common/platform_config.h"

namespace fs = boost::filesystem;

namespace maidsafe {

namespace crash_report {

#ifdef WIN32
bool DumpCallback(const wchar_t* dump_path,
                  const wchar_t* minidump_id,
                  void* context,
                  EXCEPTION_POINTERS* /*exinfo*/,
                  MDRawAssertionInfo* /*assertion*/,
                  bool succeeded) {
  ProjectInfo* project = reinterpret_cast<ProjectInfo*>(context);
  fs::path full_dump_path(dump_path);
  full_dump_path /= (std::wstring(minidump_id) + L".dmp");
  int current_modulepath_length = 0;
  int max_path_length = MAX_PATH;
  boost::scoped_array<TCHAR> current_path(new TCHAR[max_path_length]);
  while (current_modulepath_length <= max_path_length) {
    current_modulepath_length = GetModuleFileName(NULL, current_path.get(),
                                                  max_path_length);
    if (current_modulepath_length >= max_path_length) {
      max_path_length *= 2;
      current_path.reset(new TCHAR[max_path_length]);
    } else if (current_modulepath_length == 0) {
      DLOG(ERROR) << "Cannot Retrieve Current Path";
      break;
    } else {
      break;
    }
  }
  fs::path current_directory(fs::path(current_path.get()).parent_path());
  fs::path crash_reporter(current_directory / "CrashReporter.exe");
  if (fs::is_regular_file(crash_reporter)) {
    std::string command = crash_reporter.string() + " " +
                          full_dump_path.string() + " " + project->name +
                          " " +
                          boost::lexical_cast<std::string>(project->version);
    std::system(command.c_str());
  } else {
    DLOG(ERROR) << "Crash Reporter Not Found.";
  }
  return succeeded;
}
#else
bool DumpCallback(const char* dump_path,
                  const char* minidump_id,
                  void* context,
                  bool succeeded) {
  ProjectInfo* project = reinterpret_cast<ProjectInfo*>(context);
  fs::path full_dump_path(dump_path);
  full_dump_path /= (std::string(minidump_id) + ".dmp");
  int current_modulepath_length = 0;
  int max_path_length = PATH_MAX;
  boost::scoped_array<char> current_path(new char[max_path_length]);
  while (current_modulepath_length <= max_path_length) {
    current_modulepath_length = readlink("/proc/self/exe",
                                         current_path.get(),
                                         PATH_MAX);
    if (current_modulepath_length >= max_path_length) {
      max_path_length *= 2;
      current_path.reset(new char[max_path_length]);
    } else if (current_modulepath_length == 0) {
      DLOG(ERROR) << "Cannot Retrieve Current Path";
      break;
    } else {
      break;
    }
  }
  fs::path current_directory(fs::path(current_path.get()).parent_path());
  fs::path crash_reporter(current_directory / "CrashReporter");
  fs::path crash_reporter_debug(current_directory / "CrashReporter-d");
  if (fs::is_regular_file(crash_reporter)) {
    std::string command = crash_reporter.string() + " " +
                          full_dump_path.string() + " " + project->name + " " +
                          boost::lexical_cast<std::string>(project->version);
    int result(std::system(command.c_str()));
    if (result != 0)
      DLOG(ERROR) << "Crash Reporter execution failed.";
  } else if (fs::is_regular_file(crash_reporter_debug)) {
    std::string command = crash_reporter_debug.string() + " " +
                          full_dump_path.string() + " " + project->name + " " +
                          boost::lexical_cast<std::string>(project->version);
    int result(std::system(command.c_str()));
    if (result != 0)
      DLOG(ERROR) << "Crash Reporter Debug execution failed.";
  } else {
    DLOG(ERROR) << "Crash Reporter Not Found.";
  }
  return succeeded;
}
#endif

ProjectInfo::ProjectInfo(std::string project_name, std::string project_version)
    : version(project_version),
      name(project_name) {}

}  // namespace crash_report

}  // namespace maidsafe
