/*******************************************************************************
 *  Copyright 2012 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ******************************************************************************/

#include <memory>
#include <vector>
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"
#include "maidsafe/rudp/managed_connections.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/bootstrap_file_handler.h"
#include "maidsafe/routing/log.h"


namespace maidsafe {
namespace routing {
namespace test {

TEST(BootStrapFileTest1, BEH_ReadValidFile) {
  std::vector<boost::asio::ip::udp::endpoint>vec;
  boost::asio::ip::udp::endpoint endpoint;
  endpoint.address().from_string("192.168.1.1");
  endpoint.port(5000);
  vec.push_back(endpoint);
  boost::filesystem::path good_file
                (fs::unique_path(fs::temp_directory_path() / "test"));
  EXPECT_TRUE(ReadBootstrapFile(good_file).empty());
  std::error_code error;
  EXPECT_TRUE(WriteBootstrapFile(vec, good_file));
  EXPECT_FALSE(error);
  EXPECT_FALSE(ReadBootstrapFile(good_file).empty());
  EXPECT_EQ(ReadBootstrapFile(good_file).size(), vec.size());
  EXPECT_EQ(ReadBootstrapFile(good_file)[0], vec[0]);
  EXPECT_TRUE(boost::filesystem::remove(good_file));
}

}  // namespace test
}  // namespace routing
}  // namespace maidsafe
