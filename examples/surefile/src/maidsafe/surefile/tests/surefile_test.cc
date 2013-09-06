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
  EXPECT_NO_THROW(surefile.Login());
}

}  // namespace test

}  // namespace surefile
}  // namespace maidsafe

int main(int argc, char **argv) {
  return maidsafe::test::ExecuteMain(argc, argv);
}
