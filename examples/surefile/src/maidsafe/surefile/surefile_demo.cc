/*  Copyright 2011 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.novinet.com/license

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

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
#include "boost/thread/thread.hpp"

#include "maidsafe/common/crypto.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/passport/detail/secure_string.h"
#include "maidsafe/data_store/sure_file_store.h"

#include "maidsafe/surefile/surefile.h"

#ifdef MAIDSAFE_WIN32
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
  Slots slots;
  slots.configuration_error = [](){ LOG(kError) << "Configuration error."; };
  slots.on_service_added = [&surefile] {
    // Must be boost::thread not std::thread since MSVC doesn't correctly handle detach().
    boost::thread thread([&surefile] {
      std::this_thread::sleep_for(std::chrono::seconds(3));
      fs::path storage_path(maidsafe::GetUserAppDir().parent_path() /
                            "SureFile" / std::to_string(count));
      while (fs::exists(storage_path))
        storage_path = maidsafe::GetUserAppDir() / std::to_string(++count);
      fs::create_directory(storage_path);
      std::string service_alias(RandomAlphaNumericString(5));
      surefile->AddService(storage_path.string(), service_alias);
    });
    thread.detach();
  };

  surefile.reset(new SureFile(slots));

  std::string password_string(password.string().data(), password.string().size());
  surefile->InsertInput(0, password_string, kPassword);

  try {
    if (surefile->CanCreateUser()) {
      surefile->InsertInput(0, password_string, kConfirmationPassword);
      surefile->CreateUser();
    } else {
      surefile->Login();
    }
  }
  catch(...) {
    LOG(kError) << "User creation/login failed.";
    return 1;
  }

  g_unmount_functor = [&] {};
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
  fs::path logging_dir(maidsafe::GetUserAppDir() / "logs");
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
