/*  Copyright 2012 MaidSafe.net limited

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

#include "maidsafe/surefile/error_categories.h"

#include "maidsafe/surefile/error.h"


namespace maidsafe {

namespace surefile {

const char* SureFileCategory::name() const MAIDSAFE_NOEXCEPT {
  return "SureFile Library";
}

std::string SureFileCategory::message(int error_value) const MAIDSAFE_NOEXCEPT {
  switch (static_cast<SureFileErrors>(error_value)) {
    case SureFileErrors::invalid_password:
      return "Invalid Password";
    case SureFileErrors::password_confirmation_failed:
      return "Password Confirmation Failed";
    case SureFileErrors::invalid_service:
      return "Invalid Service Path";
    case SureFileErrors::duplicate_service:
      return "Service Already Exists";
    default:
      return "Unknown error in SureFile";
  }
}

std::error_condition SureFileCategory::default_error_condition(int error_value)
    const MAIDSAFE_NOEXCEPT {
  return std::error_condition(error_value, *this);
}

}  // namespace surefile

}  // namespace maidsafe
