/*  Copyright 2012 MaidSafe.net limited

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
