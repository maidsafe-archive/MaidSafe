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

#ifndef MAIDSAFE_SUREFILE_ERROR_CATEGORIES_H_
#define MAIDSAFE_SUREFILE_ERROR_CATEGORIES_H_

#include <string>
#include <system_error>

#include "maidsafe/common/config.h"


namespace maidsafe {

namespace surefile {

class SureFileCategory : public std::error_category {
 public:
  virtual const char* name() const MAIDSAFE_NOEXCEPT;
  virtual std::string message(int error_value) const MAIDSAFE_NOEXCEPT;
  virtual std::error_condition default_error_condition(int error_value) const MAIDSAFE_NOEXCEPT;
};

}  // namespace surefile

}  // namespace maidsafe

#endif  // MAIDSAFE_SUREFILE_ERROR_CATEGORIES_H_
