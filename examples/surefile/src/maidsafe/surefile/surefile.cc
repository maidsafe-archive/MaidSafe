/* Copyright 2013 MaidSafe.net limited

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

#include "maidsafe/surefile/surefile.h"

#include <sstream>

#pragma warning(disable: 4100 4127)
# include <boost/spirit/include/karma.hpp>
# include <boost/fusion/include/std_pair.hpp>
# include "boost/filesystem/path.hpp"
# include "boost/filesystem/operations.hpp"
#pragma warning(default: 4100 4127)

#include "maidsafe/common/utils.h"
#include "maidsafe/common/crypto.h"
#include "maidsafe/passport/detail/identity_data.h"
#include "maidsafe/drive/utils.h"
#include "maidsafe/drive/config.h"

#include "maidsafe/surefile/surefile.pb.h"

namespace maidsafe {
namespace surefile {

namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace karma = boost::spirit::karma;
namespace ascii = boost::spirit::ascii;
namespace fs = boost::filesystem;

SureFile::SureFile(const lifestuff::Slots& slots)
  : slots_(CheckSlots(slots)),
    logged_in_(false),
    password_(),
    confirmation_password_(),
    current_password_(),
    mount_path_(),
    drive_(),
    pending_service_additions_(),
    mount_thread_() {}

SureFile::~SureFile() {}

void SureFile::InsertInput(uint32_t position, const std::string& characters, lifestuff::InputField input_field) {
  switch (input_field) {
    case lifestuff::kPassword: {
      if (!password_)
        password_.reset(new Password());
      password_->Insert(position, characters);
      break;
    }
    case lifestuff::kConfirmationPassword: {
      if (!confirmation_password_)
        confirmation_password_.reset(new Password());
      confirmation_password_->Insert(position, characters);
      break;
    }
    case lifestuff::kCurrentPassword: {
      if (!current_password_)
        current_password_.reset(new Password());
      current_password_->Insert(position, characters);
      break;
    }
    default:
      ThrowError(CommonErrors::unknown);
  }
}

void SureFile::RemoveInput(uint32_t position, uint32_t length, lifestuff::InputField input_field) {
  switch (input_field) {
    case lifestuff::kPassword: {
      if (!password_)
        ThrowError(CommonErrors::uninitialised);
      password_->Remove(position, length);
      break;
    }
    case lifestuff::kConfirmationPassword: {
      if (!confirmation_password_)
        ThrowError(CommonErrors::uninitialised);
      confirmation_password_->Remove(position, length);
      break;
    }
    case lifestuff::kCurrentPassword: {
      if (!current_password_)
        ThrowError(CommonErrors::uninitialised);
      current_password_->Remove(position, length);
      break;
    }
    default:
      ThrowError(CommonErrors::unknown);
  }
}

void SureFile::ClearInput(lifestuff::InputField input_field) {
  switch (input_field) {
    case lifestuff::kPassword: {
      if (password_)
        password_->Clear();
      break;
    }
    case lifestuff::kConfirmationPassword: {
      if (confirmation_password_)
        confirmation_password_->Clear();
      break;
    }
    case lifestuff::kCurrentPassword: {
      if (current_password_)
        current_password_->Clear();
      break;
    }
    default:
      ThrowError(CommonErrors::unknown);
  }
}

bool SureFile::ConfirmInput(lifestuff::InputField input_field) {
  switch (input_field) {
    case lifestuff::kPassword: {
      if (!password_)
        return false;
      return password_->IsValid(boost::regex(lifestuff::kCharRegex));
    }
    case lifestuff::kConfirmationPassword: {
      if (!password_ || !confirmation_password_)
        return false;
      if (!password_->IsFinalised())
        password_->Finalise();
      if (!confirmation_password_->IsFinalised())
        confirmation_password_->Finalise();
      if (password_->string() != confirmation_password_->string())
        return false;
      return true;
    }
    case lifestuff::kCurrentPassword: {
      if (!current_password_)
        return false;
      if (!current_password_->IsFinalised())
        current_password_->Finalise();
      if (password_) {
        password_->Finalise();
        if (!confirmation_password_)
          return false;
        confirmation_password_->Finalise();
        if (password_->string() != confirmation_password_->string()
            || password_->string() != current_password_->string())
          return false;
      } else {
        if (password_->string() != current_password_->string())
          return false;
      }
      return true;
    }
    default:
      ThrowError(CommonErrors::unknown);
  }
  return false;
}

bool SureFile::CanCreateUser() {
  std::string config_content;
  if (!ReadFile(kConfigFilePath, &config_content))
    ThrowError(CommonErrors::filesystem_io_error);
  if (config_content.empty())
    return true;
  return false;
}

void SureFile::CreateUser() {
  FinaliseInput();
  ResetConfirmationInput();
  std::string config_content;
  if (!ReadFile(kConfigFilePath, &config_content))
    ThrowError(CommonErrors::filesystem_io_error);
  if (!config_content.empty()) {
    ThrowError(CommonErrors::invalid_parameter);
  } else {
    WriteConfigFile(Map());
  }
  Identity drive_root_id = Identity(RandomAlphaNumericString(64));
  MountDrive(drive_root_id);
  logged_in_ = true;
}

void SureFile::LogIn() {
  FinaliseInput();
  ResetConfirmationInput();
  Map service_pairs(ReadConfigFile());
  if (service_pairs.empty()) {
    std::string content;
    if (!ReadFile(kConfigFilePath.string(), &content))
      ThrowError(CommonErrors::filesystem_io_error);
    CheckConfigFileContent(content);
    Identity drive_root_id(RandomAlphaNumericString(64));
    MountDrive(drive_root_id);
  } else {
    std::pair<Identity, Identity> ids;
    bool mounted(false);
    for (const auto& service_pair : service_pairs) {
      ids = GetCredentials(service_pair.first);
      if (!mounted)
        MountDrive(ids.first);
      mounted = true;
      InitialiseService(service_pair.first, service_pair.second, ids.second);
    }
  }
  logged_in_ = true;
}

void SureFile::LogOut() {
  UnmountDrive();
  logged_in_ = false;
}

void SureFile::AddService(const std::string& storage_path, const std::string& service_alias) {
  CheckValid(storage_path, service_alias);
  AddConfigEntry(storage_path, service_alias);
  drive_->AddService(service_alias, storage_path);
  auto it(pending_service_additions_.find(service_alias));
  if (it == pending_service_additions_.end())
    ThrowError(CommonErrors::invalid_parameter);
  PutCredentials(storage_path, it->second.first, it->second.second);
  pending_service_additions_.erase(it);
}

void SureFile::AddServiceFailed(const std::string& service_alias) {
  drive_->RemoveService(service_alias);
}

bool SureFile::logged_in() const {
  return logged_in_;
}

std::string SureFile::mount_path() {
  return mount_path_.string();
}

const lifestuff::Slots& SureFile::CheckSlots(const lifestuff::Slots& slots) {
  if (!slots.configuration_error)
    ThrowError(CommonErrors::uninitialised);
  if (!slots.on_service_added)
    ThrowError(CommonErrors::uninitialised);
  return slots;
}

void SureFile::InitialiseService(const std::string& storage_path,
                                 const std::string& service_alias,
                                 const Identity& service_root_id) {
  CheckValid(storage_path, service_alias);
  drive_->ReInitialiseService(service_alias, storage_path, service_root_id);
}

void SureFile::FinaliseInput() {
  password_->Finalise();
}

void SureFile::ResetInput() {
  password_.reset();
}

void SureFile::ResetConfirmationInput() {
  confirmation_password_.reset();
}

void SureFile::MountDrive(const Identity& drive_root_id) {
  drive::OnServiceAdded on_service_added([this](const fs::path& service_alias,
                                                const Identity& drive_root_id,  
                                                const Identity& service_root_id) {
                                            OnServiceAdded(service_alias.string(),
                                                           drive_root_id,
                                                           service_root_id);
                                        });
  drive::OnServiceRemoved on_service_removed([this](const fs::path& service_alias) {
                                                OnServiceRemoved(service_alias.string());
                                            });
  fs::path drive_name("SureFile Drive");
#ifdef WIN32
  std::uint32_t drive_letters, mask = 0x4, count = 2;
  drive_letters = GetLogicalDrives();
  while ((drive_letters & mask) != 0) {
    mask <<= 1;
    ++count;
  }
  if (count > 25) {
    LOG(kError) << "No available drive letters.";
    return;
  }
  char drive_path[3] = {'A' + static_cast<char>(count), ':', '\0'};
  mount_path_ = drive_path;
  drive_.reset(new Drive(drive_root_id,
                         mount_path_,
                         drive_name,
                         on_service_added,
                         on_service_removed));
#else
  boost::system::error_code error_code;
  if (!boost::filesystem::exists(mount_path_)) {
    boost::filesystem::create_directories(mount_path_, error_code);
    if (error_code) {
      LOG(kError) << "Failed to create mount dir(" << mount_path_ << "): "
                  << error_code.message();
    }
  }
  drive_.reset(new Drive(drive_root_id,
                         mount_path_,
                         drive_name,
                         on_service_added,
                         on_service_removed));
  mount_thread_ = std::move(std::thread([this] {
                                          drive_->Mount();
                                        }));
  mount_status_ = drive_->WaitUntilMounted();
#endif
}

void SureFile::UnmountDrive() {
  if (!logged_in_)
    return;
  int64_t max_space(0), used_space(0);
#ifdef WIN32
  drive_->Unmount(max_space, used_space);
#else
  drive_->Unmount(max_space, used_space);
  drive_->WaitUntilUnMounted();
  mount_thread_.join();
  boost::system::error_code error_code;
  boost::filesystem::remove_all(mount_path_, error_code);
#endif
}

SureFile::Map SureFile::ReadConfigFile() {
  Map service_pairs;
  std::string content;
  if (!ReadFile(kConfigFilePath.string(), &content))
    ThrowError(CommonErrors::filesystem_io_error);
  if (content.empty())
    return service_pairs;
  auto it = content.begin();
  auto end = content.end();
  auto skipper = ascii::space | '#' >> *(qi::char_ - qi::eol) >> qi::eol;
  grammer<std::string::iterator, decltype(skipper)> parser;
  bool result = qi::phrase_parse(it, end, parser, skipper, service_pairs);
  if (!(result && it == end))
    slots_.configuration_error();
  return service_pairs;
}

void SureFile::WriteConfigFile(const Map& service_pairs) {
  std::ostringstream content;
  if (service_pairs.empty())
    content << "#" << EncryptComment() << "\n";
  else
    content << kConfigFileComment
            << karma::format(*(karma::string << '>' << karma::string << ':'), service_pairs);
  if (!WriteFile(kConfigFilePath, content.str()))
    ThrowError(CommonErrors::invalid_parameter);
}

void SureFile::AddConfigEntry(const std::string& storage_path, const std::string& service_alias) {
  Map service_pairs(ReadConfigFile());
  auto find_it(service_pairs.find(storage_path));
  if (find_it != service_pairs.end())
    ThrowError(CommonErrors::invalid_parameter);
  auto insert_it(service_pairs.insert(std::make_pair(storage_path, service_alias)));
  if (!insert_it.second)
    ThrowError(CommonErrors::invalid_parameter);  // ...requires more informative error
  WriteConfigFile(service_pairs);
}

void SureFile::OnServiceAdded(const std::string& service_alias,
                              const Identity& drive_root_id,  
                              const Identity& service_root_id) {
  pending_service_additions_.insert(
    std::make_pair(service_alias, std::make_pair(drive_root_id, service_root_id)));
  slots_.on_service_added(service_alias);
}

void SureFile::OnServiceRemoved(const std::string& service_alias) {
  Map service_pairs(ReadConfigFile());
  for (const auto& service_pair : service_pairs) {
    if (service_pair.second == service_alias) {
      auto result(service_pairs.erase(service_pair.first));
      assert(result == 1);
      WriteConfigFile(service_pairs);
      break;
    }    
  }
}

void SureFile::PutCredentials(const fs::path& storage_path,
                              const Identity& drive_root_id,
                              const Identity& service_root_id) {
  crypto::SecurePassword secure_password(SecurePassword());
  crypto::AES256Key key(SecureKey(secure_password));
  crypto::AES256InitialisationVector iv(SecureIv(secure_password));
  crypto::CipherText cipher_text(crypto::SymmEncrypt(
      Serialise(drive_root_id, service_root_id), key, iv));
  if (!WriteFile(storage_path / kCredentialsFilename, cipher_text.string()))
    ThrowError(CommonErrors::invalid_parameter);
}

void SureFile::DeleteCredentials(const fs::path& storage_path) {
  fs::remove(storage_path / kCredentialsFilename);
}

std::pair<Identity, Identity> SureFile::GetCredentials(const fs::path& storage_path) {
  crypto::SecurePassword secure_password(SecurePassword());
  crypto::AES256Key key(SecureKey(secure_password));
  crypto::AES256InitialisationVector iv(SecureIv(secure_password));
  NonEmptyString cipher_text(ReadFile(storage_path / kCredentialsFilename));
  crypto::PlainText serialised_credentials(crypto::SymmDecrypt(cipher_text, key, iv));
  return Parse(serialised_credentials);
}

void SureFile::CheckValid(const std::string& storage_path, const std::string& service_alias) {
  if (!boost::filesystem::exists(storage_path) || drive::detail::ExcludedFilename(service_alias))
    ThrowError(CommonErrors::invalid_parameter);
}

void SureFile::CheckConfigFileContent(const std::string& content) {
  crypto::SecurePassword secure_password(SecurePassword());
  crypto::AES256Key key(SecureKey(secure_password));
  crypto::AES256InitialisationVector iv(SecureIv(secure_password));
  crypto::CipherText cipher_text(content.substr(1, content.size() - 2));
  crypto::PlainText plain_text(crypto::SymmDecrypt(cipher_text, key, iv));
  if (plain_text.string() != kConfigFileComment)
    ThrowError(CommonErrors::symmetric_decryption_error);
}

NonEmptyString SureFile::Serialise(const Identity& drive_root_id, const Identity& service_root_id) {
  protobuf::Credentials credentials;
  credentials.set_drive_root_id(drive_root_id.string());
  credentials.set_service_root_id(service_root_id.string());
  return NonEmptyString(credentials.SerializeAsString());
}

std::pair<Identity, Identity> SureFile::Parse(const NonEmptyString& serialised_credentials) {
  protobuf::Credentials credentials;
  credentials.ParseFromString(serialised_credentials.string());
  return std::make_pair(Identity(credentials.drive_root_id()),
                        Identity(credentials.service_root_id()));
}

crypto::SecurePassword SureFile::SecurePassword() {
  return crypto::SecurePassword(crypto::Hash<crypto::SHA512>(password_->string()));
}

crypto::AES256Key SureFile::SecureKey(const crypto::SecurePassword& secure_password) {
  return crypto::AES256Key(secure_password.string().substr(0, crypto::AES256_KeySize));
}

crypto::AES256InitialisationVector SureFile::SecureIv(
    const crypto::SecurePassword& secure_password) {
  return crypto::AES256InitialisationVector(
      secure_password.string().substr(crypto::AES256_KeySize, crypto::AES256_IVSize));
}

std::string SureFile::EncryptComment() {
  crypto::SecurePassword secure_password(SecurePassword());
  crypto::AES256Key key(SecureKey(secure_password));
  crypto::AES256InitialisationVector iv(SecureIv(secure_password));
  crypto::PlainText plain_text(kConfigFileComment);
  crypto::CipherText cipher_text(crypto::SymmEncrypt(plain_text, key, iv));
  return cipher_text.string();
}

const fs::path SureFile::kConfigFilePath(GetSystemAppSupportDir() /
                                         "MaidSafe/SureFile/surefile.conf");
const fs::path SureFile::kCredentialsFilename("surefile.dat");
const std::string SureFile::kConfigFileComment("# Please do NOT edit.\n");

}  // namespace surefile
}  // namespace maidsafe
