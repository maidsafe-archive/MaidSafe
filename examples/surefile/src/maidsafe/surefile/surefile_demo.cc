/* Copyright 2011 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

#include <signal.h>

#include <functional>
#include <iostream>  // NOLINT
#include <memory>
#include <string>
#include <fstream>  // NOLINT
#include <iterator>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "boost/preprocessor/stringize.hpp"
#include "boost/system/error_code.hpp"

#include "maidsafe/common/crypto.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/passport/detail/secure_string.h"
#include "maidsafe/data_store/surefile_store.h"

#include "maidsafe/surefile/surefile.h"

#ifdef WIN32
#ifndef CBFS_KEY
#  error CBFS_KEY must be defined.
#endif
#endif

namespace fs = boost::filesystem;
namespace po = boost::program_options;

typedef maidsafe::passport::detail::Password Password;

namespace maidsafe {
namespace surefile {

namespace {

std::function<void()> g_unmount_functor;

void CtrlCHandler(int /*value*/) {
  g_unmount_functor();
}

}  // unnamed namespace

typedef std::unique_ptr<SureFile> SureFilePtr;
static uint32_t count = 0;

int Init(const Password& password) {
  SureFilePtr surefile;
  lifestuff::Slots slots;
  slots.configuration_error = [](){ LOG(kError) << "Configuration error."; };
  slots.on_service_added = [&surefile](const std::string& service_alias) {
    boost::filesystem::path storage_path(maidsafe::GetUserAppDir() / std::to_string(count));
    while (boost::filesystem::exists(storage_path))
      storage_path = maidsafe::GetUserAppDir() / std::to_string(count = ++count);
    boost::filesystem::create_directory(storage_path);
    surefile->AddService(storage_path.string(), service_alias);
  };

  surefile.reset(new SureFile(slots));

  std::string content;
  boost::filesystem::path config_file_path(GetSystemAppSupportDir().parent_path() /
                                           "SureFile/surefile.conf");
  if (boost::filesystem::exists(config_file_path)) {
    if (!ReadFile(config_file_path, &content))
      return 1;
  } else {
    return 1;
  }

  std::string password_string(password.string().data(), password.string().size());
  surefile->InsertInput(0, password_string, lifestuff::kPassword);

  try {
    if (content.empty())
      surefile->CreateUser();
    else
      surefile->LogIn();
  }
  catch(...) {
    LOG(kError) << "User creation/login failed.";
    return 1;
  }

  g_unmount_functor = [&] { surefile->LogOut(); };
  signal(SIGINT, CtrlCHandler);
  int x;
  std::cin >> x;
  return 0;
}

}  // namespace surefile
}  // namespace maidsafe

int main(int argc, char *argv[]) {
  maidsafe::log::Logging::Instance().Initialise(argc, argv);
  boost::system::error_code error_code;
#ifdef WIN32
  fs::path logging_dir(maidsafe::GetSystemAppSupportDir().parent_path().parent_path() / "MaidSafe\\SureFile\\logs");
#else
  fs::path logging_dir(fs::temp_directory_path(error_code) / "MaidSafe/SureFile/logs");
  if (error_code) {
    LOG(kError) << error_code.message();
    return 1;
  }
#endif
  if (!fs::exists(logging_dir, error_code))
    fs::create_directories(logging_dir, error_code);
  if (error_code)
    LOG(kError) << error_code.message();
  if (!fs::exists(logging_dir, error_code))
    LOG(kError) << "Couldn't create logging directory at " << logging_dir;
  fs::path log_path(logging_dir / "surefile");
  try {
    std::string password_str;
    std::cout << "Enter password" << std::endl;
    getline(std::cin, password_str);

    if (password_str.empty()) {
      LOG(kError) << "Invalid password";
      return 1;
    }

    Password password(password_str);
    int result(maidsafe::surefile::Init(password));
    return result;
  }
  catch(const std::exception& e) {
    LOG(kError) << "Exception: " << e.what();
    return 1;
  }
  catch(...) {
    LOG(kError) << "Exception of unknown type!";
  }
  return 0;
}
