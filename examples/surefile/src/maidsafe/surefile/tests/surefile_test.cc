/*  Copyright 2013 MaidSafe.net limited

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

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/surefile/surefile.h"

namespace maidsafe {
namespace surefile {

namespace test {

class SureFileTest : public testing::Test {
 public:
  SureFileTest() {}

 protected:
  void SetUp() {
    slots_.configuration_error = [](){ LOG(kError) << "Configuration error."; };
    slots_.on_service_added = [] { LOG(kInfo) << "Attempt to add a service."; };
  }
  void TearDown() {}

  Slots slots_;

 private:
  SureFileTest(const SureFileTest&);
  SureFileTest& operator=(const SureFileTest&);
};

TEST_F(SureFileTest, BEH_CreateUser) {
  SureFile surefile(slots_);

  surefile.InsertInput(0, "password", kPassword);
  // EXPECT_TRUE(surefile.CanCreateUser());
  // EXPECT_NO_THROW(surefile.CreateUser());

  EXPECT_FALSE(surefile.CanCreateUser());
  EXPECT_NO_THROW(surefile.Login(""));
}

}  // namespace test

}  // namespace surefile
}  // namespace maidsafe

int main(int argc, char **argv) {
  return maidsafe::test::ExecuteMain(argc, argv);
}
