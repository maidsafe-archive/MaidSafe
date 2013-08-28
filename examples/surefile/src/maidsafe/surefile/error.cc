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

#include "maidsafe/surefile/error.h"

#include "boost/throw_exception.hpp"

#include "maidsafe/surefile/error_categories.h"


namespace maidsafe {
namespace surefile {

std::error_code make_error_code(SureFileErrors code) {
  return std::error_code(static_cast<int>(code), GetSureFileCategory());
}

std::error_condition make_error_condition(SureFileErrors code) {
  return std::error_condition(static_cast<int>(code), GetSureFileCategory());
}

surefile_error MakeError(SureFileErrors code) {
  return surefile_error(make_error_code(code));
}

const std::error_category& GetSureFileCategory() {
  static SureFileCategory instance;
  return instance;
}

}  // namespace surefile
}  // namespace maidsafe
