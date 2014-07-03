/*  Copyright 2009 MaidSafe.net limited

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

extern "C" {
#include <sqlite3.h>
}

#include <string>

#include "boost/filesystem/path.hpp"

#include "maidsafe/common/error.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

namespace maidsafe {

namespace test {

TEST(SQLiteTest, BEH_Basic) {
  sqlite3 *db;
  int rc;
  const boost::filesystem::path db_file_path("testsqlite.db");
  rc = sqlite3_open(db_file_path.string().c_str(), &db);
  EXPECT_EQ(rc, 0) << "Can't open database: ", sqlite3_errmsg(db);
  sqlite3_close(db);
 }

}  // namespace test

}  // namespace maidsafe

int main(int argc, char** argv) { return maidsafe::test::ExecuteMain(argc, argv); }
