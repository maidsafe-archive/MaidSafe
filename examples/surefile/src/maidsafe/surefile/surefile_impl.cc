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

#include "maidsafe/surefile/surefile_impl.h"

#include "maidsafe/data_store/surefile_store.h"

namespace maidsafe {
namespace surefile {

SureFileImpl::SureFileImpl(const lifestuff::Slots& /*slots*/)
  : logged_in_(false),
    password_(),
    confirmation_password_(),
    current_password_(),
    unique_user_id_(),
    root_parent_id_(),
    mount_path_(),
    drive_(),
    mount_thread_() {}

SureFileImpl::~SureFileImpl() {}

void SureFileImpl::InsertUserInput(uint32_t position, const std::string& characters,
                                   lifestuff::InputField input_field) {
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

void SureFileImpl::RemoveUserInput(uint32_t position, uint32_t length, lifestuff::InputField input_field) {
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

void SureFileImpl::ClearUserInput(lifestuff::InputField input_field) {
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

bool SureFileImpl::ConfirmUserInput(lifestuff::InputField input_field) {
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

void SureFileImpl::CreateUser() {
  FinaliseUserInput();
  ResetConfirmationInput();
  unique_user_id_ = Identity(RandomAlphaNumericString(64));
  DiskUsage disk_usage(10737418240);
  storage_.reset(new SureFileStore(boost::filesystem::path(), disk_usage)); // TODO() need a valid path or change store implementation.
  MountDrive();
  PutSession();
  logged_in_ = true;
}

void SureFileImpl::LogIn() {
  FinaliseUserInput();
  GetSession();
  // TODO() from session get storage paths, disk usage and user credentials.
  DiskUsage disk_usage(10737418240);
  storage_.reset(new SureFileStore(boost::filesystem::path(), disk_usage));
  MountDrive();
  logged_in_ = true;
}

void SureFileImpl::LogOut() {
  UnmountDrive();
  PutSession();
  logged_in_ = false;
}

void SureFileImpl::ChangePassword() {
  // TODO() check this!
  if (!ConfirmUserInput(lifestuff::kCurrentPassword))
    ThrowError(CommonErrors::invalid_parameter);
  PutSession();
  confirmation_password_.reset();
  current_password_.reset();
  return;
}

bool SureFileImpl::logged_in() const {
  return logged_in_;
}

boost::filesystem::path SureFileImpl::mount_path() {
  return mount_path_;
}

const lifestuff::Slots& SureFileImpl::CheckSlots(const lifestuff::Slots& slots) {
  /*if (!slots.update_available)
    ThrowError(CommonErrors::uninitialised);
  if (!slots.operations_pending)
    ThrowError(CommonErrors::uninitialised);*/
  return slots;
}

void SureFileImpl::FinaliseUserInput() {
  password_->Finalise();
  return;
}

void SureFileImpl::ResetInput() {
  password_.reset();
}

void SureFileImpl::ResetConfirmationInput() {
  confirmation_password_.reset();
}

void SureFileImpl::MountDrive() {
  if (logged_in_)
    return;
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
  char drive_name[3] = {'A' + static_cast<char>(count), ':', '\0'};
  mount_path_ = drive_name;
  drive_.reset(new Drive(*storage_,
                         unique_user_id_,
                         root_parent_id_,
                         mount_path_,
                         kDriveLogo.string(),
                         storage_->GetMaxDiskUsage(),
                         storage_->GetCurrentDiskUsage()));
  if (root_parent_id_ != drive_->root_parent_id())
    root_parent_id_ = drive_->root_parent_id();
#else
  boost::system::error_code error_code;
  if (!boost::filesystem::exists(mount_path_)) {
    boost::filesystem::create_directories(mount_path_, error_code);
    if (error_code) {
      LOG(kError) << "Failed to create mount dir(" << mount_path_ << "): "
                  << error_code.message();
    }
  }
  drive_.reset(new Drive(*storage_,
                         unique_user_id_,
                         root_parent_id_,
                         mount_path_,
                         kDriveLogo.string(),
                         storage_->GetMaxDiskUsage(),
                         storage_->GetCurrentDiskUsage()));
  mount_thread_ = std::move(std::thread([this] {
                                          drive_->Mount();
                                        }));
  drive_->WaitUntilMounted();  // FIXME(Team): Throw on mount failure?
#endif
}

void SureFileImpl::UnmountDrive() {
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

void SureFileImpl::PutSession() {
  // TODO() write data to file
  return;
}

void SureFileImpl::DeleteSession() {
  // TODO() delete data from file
  return;
}

void SureFileImpl::GetSession() {
  // TODO() get data from file
  return;
}

}  // namespace surefile
}  // namespace maidsafe
