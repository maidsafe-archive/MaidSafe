/* Copyright 2012 MaidSafe.net limited

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
      return "Invalid password";
    case SureFileErrors::password_confirmation_failed:
      return "Password Confirmation Failed";
    case SureFileErrors::invalid_service:
      return "Invalid Service Path";
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
