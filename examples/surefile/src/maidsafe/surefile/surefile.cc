/*  Copyright 2013 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#include "maidsafe/surefile/surefile.h"

#include <sstream>

#ifdef __MSVC__
#  pragma warning(push, 1)
#endif
# include "boost/spirit/include/karma.hpp"
# include "boost/fusion/include/std_pair.hpp"
#ifdef __MSVC__
#  pragma warning(pop)
#endif

#include "boost/filesystem/operations.hpp"

#include "maidsafe/common/utils.h"
#include "maidsafe/common/crypto.h"
#include "maidsafe/passport/detail/identity_data.h"
#include "maidsafe/drive/utils.h"
#include "maidsafe/drive/config.h"

#include "maidsafe/surefile/surefile.pb.h"

namespace maidsafe {
namespace surefile {

namespace qi = boost::spirit::qi;
namespace karma = boost::spirit::karma;
namespace fs = boost::filesystem;

SureFile::SureFile(Slots slots)
  : slots_(CheckSlots(slots)),
    logged_in_(false),
    password_(),
    confirmation_password_(),
    mount_path_(),
    drive_root_id_(),
    drive_(),
    pending_service_additions_(),
    mutex_(),
    mount_thread_(),
    mount_status_(false) {}

SureFile::~SureFile() {
  if (logged_in_)
    UnmountDrive();
}

void SureFile::InsertInput(uint32_t position, const std::string& characters, InputField input_field) {
  switch (input_field) {
    case kPassword: {
      if (!password_)
        password_.reset(new Password());
      password_->Insert(position, characters);
      break;
    }
    case kConfirmationPassword: {
      if (!confirmation_password_)
        confirmation_password_.reset(new Password());
      confirmation_password_->Insert(position, characters);
      break;
    }
    default:
      ThrowError(CommonErrors::unknown);
  }
}

void SureFile::RemoveInput(uint32_t position, uint32_t length, InputField input_field) {
  switch (input_field) {
    case kPassword: {
      if (!password_)
        ThrowError(CommonErrors::uninitialised);
      password_->Remove(position, length);
      break;
    }
    case kConfirmationPassword: {
      if (!confirmation_password_)
        ThrowError(CommonErrors::uninitialised);
      confirmation_password_->Remove(position, length);
      break;
    }
    default:
      ThrowError(CommonErrors::unknown);
  }
}

bool SureFile::CanCreateUser() const {
  if (logged_in_)
    return false;
  boost::system::error_code ec;
  return !fs::exists(GetUserAppDir() / kSureFile, ec);
}

void SureFile::CreateUser(const std::string& product_id) {
  if (logged_in_)
    return;
  FinaliseInput(false);
  ConfirmInput();
  ResetConfirmationPassword();
  if (fs::exists(GetUserAppDir() / kSureFile))
    ThrowError(CommonErrors::invalid_parameter);
  if (!fs::exists(GetUserAppDir()))
    if (!fs::create_directories(GetUserAppDir()))
      ThrowError(CommonErrors::filesystem_io_error);
  drive_root_id_ = Identity(RandomAlphaNumericString(64));
  MountDrive(product_id, drive_root_id_);
  WriteConfigFile(ServiceMap());
  logged_in_ = true;
}

void SureFile::Login(const std::string& product_id) {
  if (logged_in_)
    return;
  FinaliseInput(true);
  assert(!confirmation_password_);
  ServiceMap service_pairs;
  try {
    service_pairs = ReadConfigFile();
  }
  catch(const surefile_error& exception) {
    if (exception.code() == make_error_code(SureFileErrors::invalid_password))
      ResetPassword();
    boost::throw_exception(exception);
  }
  if (service_pairs.empty()) {
    drive_root_id_ = Identity(RandomAlphaNumericString(64));
    MountDrive(product_id, drive_root_id_);
  } else {
    std::pair<Identity, Identity> ids;
    auto it(service_pairs.begin()), end(service_pairs.end());
    try {
      ids = GetIds(it->first);
    }
    catch(...) {
      ResetPassword();
      ThrowError(SureFileErrors::invalid_password);
    }
    drive_root_id_ = ids.first;
    MountDrive(product_id, drive_root_id_);
    InitialiseService(it->first, it->second, ids.second);
    while (++it != end) {
      ids = GetIds(it->first);
      InitialiseService(it->first, it->second, ids.second);
    }
  }
  logged_in_ = true;
}

void SureFile::AddService(const std::string& storage_path, const std::string& service_alias) {
  if (!logged_in_)
    ThrowError(CommonErrors::uninitialised);
  CheckValid(storage_path, service_alias);
  CheckDuplicate(storage_path, service_alias);
  Identity service_root_id(RandomAlphaNumericString(64));
  drive_->AddService(service_alias, storage_path, service_root_id);
  PutIds(storage_path, drive_root_id_, service_root_id);
  AddConfigEntry(storage_path, service_alias);
}

bool SureFile::RemoveService(const std::string& service_alias) {
  if (!logged_in_)
    ThrowError(CommonErrors::uninitialised);
  if (!fs::exists(boost::filesystem::path(mount_path()) / service_alias))
    return false;
  boost::system::error_code error_code;
  fs::remove_all(boost::filesystem::path(mount_path()) / service_alias, error_code);
  return error_code.value() == 0;
}

bool SureFile::logged_in() const {
  return logged_in_;
}

std::string SureFile::mount_path() const {
#ifdef MAIDSAFE_WIN32
  return mount_path_.string() + "\\";
#else
  return mount_path_.string();
#endif
}

SureFile::ServiceMap SureFile::service_pairs() const {
  return ReadConfigFile();
}

Slots SureFile::CheckSlots(Slots slots) const {
  if (!(slots.configuration_error &&
        slots.on_service_removed &&
        slots.on_service_renamed &&
        slots.on_service_added))
    ThrowError(CommonErrors::uninitialised);
  return slots;
}

void SureFile::InitialiseService(const std::string& storage_path,
                                 const std::string& service_alias,
                                 const Identity& service_root_id) {
  CheckValid(storage_path, service_alias);
  drive_->AddService(service_alias, storage_path, service_root_id);
}

void SureFile::FinaliseInput(bool login) {
  if (!password_) {
    if (confirmation_password_)
      ResetConfirmationPassword();
    ThrowError(SureFileErrors::invalid_password);
  }
  password_->Finalise();
  if (!login) {
    if (!confirmation_password_) {
      ResetPassword();
      ThrowError(SureFileErrors::password_confirmation_failed);
    }
    confirmation_password_->Finalise();
  }
}

void SureFile::ClearInput() {
  if (password_)
    password_->Clear();
  if (confirmation_password_)
    confirmation_password_->Clear();
}

void SureFile::ConfirmInput() {
  if (!password_->IsValid(boost::regex(kCharRegex))) {
    ResetPassword();
    ResetConfirmationPassword();
    ThrowError(SureFileErrors::invalid_password);
  }
  if (password_->string() != confirmation_password_->string()) {
    ResetPassword();
    ResetConfirmationPassword();
    ThrowError(SureFileErrors::password_confirmation_failed);
  }
}

void SureFile::ResetPassword() {
  password_.reset();
}

void SureFile::ResetConfirmationPassword() {
  confirmation_password_.reset();
}

void SureFile::MountDrive(const std::string& product_id, const Identity& drive_root_id) {
  drive::OnServiceAdded on_service_added([this]() {
                                            OnServiceAdded();
                                        });
  drive::OnServiceRemoved on_service_removed([this](const fs::path& service_alias) {
                                                OnServiceRemoved(service_alias.string());
                                            });
  drive::OnServiceRenamed on_service_renamed([this](const fs::path& old_service_alias,
                                                    const fs::path& new_service_alias) {
                                                OnServiceRenamed(old_service_alias.string(),
                                                                 new_service_alias.string());
                                            });
  fs::path drive_name("SureFile Drive");
#ifdef MAIDSAFE_WIN32
  mount_path_ = drive::GetNextAvailableDrivePath();
  drive_.reset(new Drive(drive_root_id,
                         mount_path_,
                         product_id,
                         drive_name,
                         on_service_added,
                         on_service_removed,
                         on_service_renamed));
#else
  static_cast<void>(product_id);
  // TODO() Confirm location of mount point
  mount_path_ = GetUserAppDir() / RandomAlphaNumericString(10);
  boost::system::error_code error_code;
  if (!fs::exists(mount_path_)) {
    fs::create_directories(mount_path_, error_code);
    if (error_code) {
      LOG(kError) << "Failed to create mount dir(" << mount_path_ << "): "
                  << error_code.message();
    }
  }
/*  mount_thread_ = std::move(std::thread([this,
                                           drive_root_id,
                                           product_id,
                                           drive_name,
                                           on_service_added,
                                           on_service_removed,
                                           on_service_renamed] {*/
#ifdef MAIDSAFE_WIN32
  drive_.reset(new Drive(drive_root_id,
                         mount_path_,
                         product_id,
                         drive_name,
                         on_service_added,
                         on_service_removed,
                         on_service_renamed));
#else
  static_cast<void>(product_id);
  drive_.reset(new Drive(drive_root_id,
                         mount_path_,
                         drive_name,
                         on_service_added,
                         on_service_removed,
                         on_service_renamed));
#endif
                                        //}));
//  mount_thread_.join();
  mount_status_ = drive_->WaitUntilMounted();
#endif
}

void SureFile::UnmountDrive() {
  if (!logged_in_)
    return;
#ifdef MAIDSAFE_WIN32
  drive_->Unmount();
#else
  drive_->Unmount();
  drive_->WaitUntilUnMounted();
  mount_thread_.join();
  boost::system::error_code error_code;
  fs::remove_all(mount_path_, error_code);
#endif
}

SureFile::ServiceMap SureFile::ReadConfigFile() const {
  ServiceMap service_pairs;
  NonEmptyString content(ReadFile(GetUserAppDir() / kSureFile));
  if (content.string().size() > kSureFile.size()) {
    if (content.string().substr(0, kSureFile.size()) == kSureFile) {
      if (!ValidateContent(content.string()))
        ThrowError(SureFileErrors::invalid_password);
      return service_pairs;
    }
  }
  auto it = content.string().begin(), end = content.string().end();
  Grammer<std::string::const_iterator> parser;
  bool result = qi::parse(it, end, parser, service_pairs);
  if (!result || it != end)
    slots_.configuration_error();
  return service_pairs;
}

void SureFile::WriteConfigFile(const ServiceMap& service_pairs) const {
  std::ostringstream content;
  if (service_pairs.empty())
    content << kSureFile << EncryptSureFile();
  else
    content << karma::format(*(karma::string << '>' << karma::string << ':'), service_pairs);
  if (!WriteFile(GetUserAppDir() / kSureFile, content.str()))
    ThrowError(CommonErrors::invalid_parameter);
}

void SureFile::AddConfigEntry(const std::string& storage_path, const std::string& service_alias) const {
  ServiceMap service_pairs(ReadConfigFile());
  auto find_it(service_pairs.find(storage_path));
  if (find_it != service_pairs.end())
    ThrowError(CommonErrors::invalid_parameter);
  auto insert_it(service_pairs.insert(std::make_pair(storage_path, service_alias)));
  if (!insert_it.second)
    ThrowError(CommonErrors::invalid_parameter);
  WriteConfigFile(service_pairs);
}

void SureFile::OnServiceAdded() const {
  slots_.on_service_added();
}

void SureFile::OnServiceRemoved(const std::string& service_alias) const {
  ServiceMap service_pairs(ReadConfigFile());
  for (const auto& service_pair : service_pairs) {
    if (service_pair.second == service_alias) {
      boost::system::error_code error_code;
      fs::remove_all(service_pair.first, error_code);
      assert(error_code.value() == 0);
      service_pairs.erase(service_pair.first);
      WriteConfigFile(service_pairs);
      slots_.on_service_removed(service_alias);
      break;
    }
  }
}

void SureFile::OnServiceRenamed(const std::string& old_service_alias,
                                const std::string& new_service_alias) const {
  ServiceMap service_pairs(ReadConfigFile());
  for (auto& service_pair : service_pairs) {
    if (service_pair.second == old_service_alias) {
      service_pair.second = new_service_alias;
      WriteConfigFile(service_pairs);
      slots_.on_service_renamed(old_service_alias, new_service_alias);
      break;
    }
  }
}

void SureFile::PutIds(const fs::path& storage_path,
                      const Identity& drive_root_id,
                      const Identity& service_root_id) const {
  crypto::SecurePassword secure_password(SecurePassword());
  crypto::AES256Key key(SecureKey(secure_password));
  crypto::AES256InitialisationVector iv(SecureIv(secure_password));
  crypto::PlainText serialised_credentials(Serialise(drive_root_id, service_root_id));
  crypto::CipherText cipher_text(crypto::SymmEncrypt(serialised_credentials, key, iv));
  if (!WriteFile(storage_path / kSureFile, cipher_text.string()))
    ThrowError(CommonErrors::invalid_parameter);
}

void SureFile::DeleteIds(const fs::path& storage_path) const {
  fs::remove(storage_path / kSureFile);
}

std::pair<Identity, Identity> SureFile::GetIds(const fs::path& storage_path) const {
  crypto::SecurePassword secure_password(SecurePassword());
  crypto::AES256Key key(SecureKey(secure_password));
  crypto::AES256InitialisationVector iv(SecureIv(secure_password));
  crypto::CipherText cipher_text(ReadFile(storage_path / kSureFile));
  crypto::PlainText serialised_credentials(crypto::SymmDecrypt(cipher_text, key, iv));
  return Parse(serialised_credentials);
}

void SureFile::CheckValid(const std::string& storage_path, const std::string& service_alias) const {
  if (!fs::exists(storage_path) || drive::detail::ExcludedFilename(service_alias) ||
      service_alias.empty())
    ThrowError(SureFileErrors::invalid_service);
}

void SureFile::CheckDuplicate(const std::string& storage_path, const std::string& service_alias) const {
  ServiceMap service_pairs(ReadConfigFile());
  for (const auto& service_pair : service_pairs)
    if (service_pair.first == storage_path || service_pair.second == service_alias)
      ThrowError(SureFileErrors::duplicate_service);
}

bool SureFile::ValidateContent(const std::string& content) const {
  crypto::SecurePassword secure_password(SecurePassword());
  crypto::AES256Key key(SecureKey(secure_password));
  crypto::AES256InitialisationVector iv(SecureIv(secure_password));
  auto size(kSureFile.size());
  crypto::CipherText cipher_text(content.substr(size, content.size() - size));
  crypto::PlainText plain_text(crypto::SymmDecrypt(cipher_text, key, iv));
  if (plain_text.string() == kSureFile)
    return true;
  return false;
}

NonEmptyString SureFile::Serialise(const Identity& drive_root_id,
                                   const Identity& service_root_id) const {
  protobuf::Credentials credentials;
  credentials.set_drive_root_id(drive_root_id.string());
  credentials.set_service_root_id(service_root_id.string());
  return NonEmptyString(credentials.SerializeAsString());
}

std::pair<Identity, Identity> SureFile::Parse(const NonEmptyString& serialised_credentials) const {
  protobuf::Credentials credentials;
  credentials.ParseFromString(serialised_credentials.string());
  return std::make_pair(Identity(credentials.drive_root_id()),
                        Identity(credentials.service_root_id()));
}

crypto::SecurePassword SureFile::SecurePassword() const {
  return crypto::SecurePassword(crypto::Hash<crypto::SHA512>(password_->string()));
}

crypto::AES256Key SureFile::SecureKey(const crypto::SecurePassword& secure_password) const {
  return crypto::AES256Key(secure_password.string().substr(0, crypto::AES256_KeySize));
}

crypto::AES256InitialisationVector SureFile::SecureIv(
    const crypto::SecurePassword& secure_password) const {
  return crypto::AES256InitialisationVector(
      secure_password.string().substr(crypto::AES256_KeySize, crypto::AES256_IVSize));
}

std::string SureFile::EncryptSureFile() const {
  crypto::SecurePassword secure_password(SecurePassword());
  crypto::AES256Key key(SecureKey(secure_password));
  crypto::AES256InitialisationVector iv(SecureIv(secure_password));
  crypto::PlainText plain_text(kSureFile);
  crypto::CipherText cipher_text(crypto::SymmEncrypt(plain_text, key, iv));
  return cipher_text.string();
}

const std::string SureFile::kSureFile("surefile");

}  // namespace surefile
}  // namespace maidsafe
