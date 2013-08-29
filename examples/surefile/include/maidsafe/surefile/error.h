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

#ifndef MAIDSAFE_SUREFILE_ERROR_H_
#define MAIDSAFE_SUREFILE_ERROR_H_

#include <memory>
#include <string>

#include "maidsafe/common/error.h"


namespace maidsafe {
namespace surefile {

enum class SureFileErrors {
  invalid_password = 1,
  password_confirmation_failed,
  invalid_service
};

class surefile_error : public maidsafe_error {
 public:
  surefile_error(std::error_code ec, const std::string& what_arg) : maidsafe_error(ec, what_arg) {}
  surefile_error(std::error_code ec, const char* what_arg) : maidsafe_error(ec, what_arg) {}
  explicit surefile_error(std::error_code ec) : maidsafe_error(ec) {}
  surefile_error(int ev, const std::error_category& ecat, const std::string& what_arg)
      : maidsafe_error(ev, ecat, what_arg) {}
  surefile_error(int ev, const std::error_category& ecat, const char* what_arg)
      : maidsafe_error(ev, ecat, what_arg) {}
  surefile_error(int ev, const std::error_category& ecat) : maidsafe_error(ev, ecat) {}
};

std::error_code make_error_code(SureFileErrors code);
std::error_condition make_error_condition(SureFileErrors code);
const std::error_category& GetSureFileCategory();
surefile_error MakeError(SureFileErrors code);

}  // namespace surefile
}  // namespace maidsafe

#endif  // MAIDSAFE_SUREFILE_ERROR_H_
