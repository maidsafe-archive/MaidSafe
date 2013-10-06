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

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/surefile/surefile.h"

namespace maidsafe {
namespace surefile {

namespace test {

class SureFileTest {
 public:
  SureFileTest() {
    slots_.configuration_error = [] { LOG(kError) << "Configuration error."; };  // NOLINT
    slots_.on_service_added = [] { LOG(kInfo) << "Attempt to add a service."; };  // NOLINT
  }

  Slots slots_;

 private:
  SureFileTest(const SureFileTest&);
  SureFileTest(SureFileTest&&);
  SureFileTest& operator=(const SureFileTest&);
};

TEST_CASE_METHOD(SureFileTest, "Create user", "[SureFile][Behavioural]") {
  SureFile surefile(slots_);
  std::string product_id;
  surefile.InsertInput(0, "password", kPassword);
  // CHECK(surefile.CanCreateUser());
  // CHECK_NOTHROW(surefile.CreateUser());

  CHECK_FALSE(surefile.CanCreateUser());
  CHECK_NOTHROW(surefile.Login(product_id));
}

}  // namespace test

}  // namespace surefile
}  // namespace maidsafe

int main(int argc, char **argv) {
  return maidsafe::test::ExecuteMain(argc, argv);
}
