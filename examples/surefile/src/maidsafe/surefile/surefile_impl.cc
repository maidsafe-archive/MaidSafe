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

SureFileImpl::SureFileImpl(const lifestuff::Slots& slots)
  : logged_in_(false),
    password_(),
    confirmation_password_(),
    current_password_(),
    maid_(),
    unique_user_id_() {}

SureFileImpl::~SureFileImpl() {}

void SureFileImpl::InsertUserInput(uint32_t position, const std::string& characters, lifestuff::InputField input_field) {
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
  client_maid_.CreateUser(*keyword_, *pin_, *password_, storage_path, report_progress);

  maid_.reset(new Maid(Maid::signer_type()));
  unique_user_id_.reset(new Identity(RandomAlphaNumericString(64)));
  DiskUsage disk_usage(10737418240);
  storage_.reset(new SureFileStore(boost::filesystem::path(), disk_usage)); // TODO() need a valid path or change store implementation.

  user_storage_.MountDrive(*storage_, session_);

  PutSession(keyword, pin, password);



  logged_in_ = true;
}

void SureFileImpl::LogIn(const boost::filesystem::path& storage_path,
                                ReportProgressFunction& report_progress) {
  FinaliseUserInput();
  client_maid_.LogIn(*keyword_, *pin_, *password_, storage_path, report_progress);
  ResetInput();
  logged_in_ = true;
  return;
}

void SureFileImpl::ChangePassword(ReportProgressFunction& report_progress) {
  report_progress(kChangePassword, kConfirmingUserInput);
  if (!ConfirmUserInput(kCurrentPassword))
    ThrowError(CommonErrors::invalid_parameter);
  client_maid_.ChangePassword(session_.keyword(), session_.pin(), *password_, report_progress);
  password_.reset();
  confirmation_password_.reset();
  current_password_.reset();
  return;
}

bool SureFileImpl::logged_in() const {
  return logged_in_;
}

boost::filesystem::path SureFileImpl::mount_path() {
  return client_maid_.mount_path();
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

}  // namespace surefile
}  // namespace maidsafe
