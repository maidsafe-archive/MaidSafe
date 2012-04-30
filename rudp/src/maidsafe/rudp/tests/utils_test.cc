/*******************************************************************************
 *  Copyright 2012 MaidSafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of MaidSafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of MaidSafe.net. *
 ******************************************************************************/

#include <algorithm>
#include <cstdlib>
#include <set>
#include "boost/scoped_array.hpp"
#include "maidsafe/common/test.h"
#include "maidsafe/rudp/log.h"
#include "maidsafe/rudp/utils.h"

namespace maidsafe {

namespace rudp {

namespace test {

TEST(UtilsTest, BEH_EndpointIsValid) {
  EXPECT_FALSE(IsValid(Endpoint(
      boost::asio::ip::address::from_string("1.1.1.1"), 1024)));
  EXPECT_TRUE(IsValid(Endpoint(
      boost::asio::ip::address::from_string("1.1.1.1"), 1025)));
  EXPECT_TRUE(IsValid(Endpoint(
      boost::asio::ip::address::from_string("1.1.1.1"), 49150)));
  EXPECT_FALSE(IsValid(Endpoint(
      boost::asio::ip::address::from_string("1.1.1.1"), 49151)));
  EXPECT_FALSE(IsValid(Endpoint(
      boost::asio::ip::address::from_string("0.0.0.0"), 49150)));

  boost::system::error_code error_code;
  try {
    boost::asio::ip::address::from_string("Rubbish");
  }
  catch(const boost::system::system_error &system_error) {
    error_code = system_error.code();
  }
#ifdef WIN32
  const int kErrorCodeValue(10022);
#else
  const int kErrorCodeValue(22);
#endif
  EXPECT_EQ(error_code.value(), kErrorCodeValue);
  error_code.clear();

  try {
    boost::asio::ip::address::from_string("256.1.1.1");
  }
  catch(const boost::system::system_error &system_error) {
    error_code = system_error.code();
  }
  EXPECT_EQ(error_code.value(), kErrorCodeValue);
  error_code.clear();

  EXPECT_FALSE(IsValid(Endpoint()));
  EXPECT_FALSE(IsValid(Endpoint(boost::asio::ip::udp::v4(), 1025)));
  EXPECT_FALSE(IsValid(Endpoint(boost::asio::ip::udp::v6(), 1025)));
  EXPECT_FALSE(IsValid(Endpoint(boost::asio::ip::address(), 1025)));
}

}  // namespace test

}  // namespace rudp

}  // namespace maidsafe
