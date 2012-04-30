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
#include <chrono>
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"
#include "maidsafe/rudp/managed_connections.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/rpcs.h"
#include "maidsafe/routing/routing_table.h"
#include "maidsafe/routing/tests/test_utils.h"
#include "maidsafe/routing/log.h"


namespace maidsafe {
namespace routing {
namespace test {


TEST(RPC, BEH_PingMessageInitialised) {
  // check with assert in debug mode, should NEVER fail
  std::string destination = RandomString(64);
  ASSERT_TRUE(rpcs::Ping(NodeId(destination), "me").IsInitialized());
}

TEST(RPC, BEH_PingMessageNode) {
  asymm::Keys keys;
  keys.identity = RandomString(64);
  RoutingTable RT(keys);
  NodeInfo node;
  rudp::ManagedConnections rudp;
  std::string destination = RandomString(64);
  protobuf::Message message = rpcs::Ping(NodeId(destination), keys.identity);
  protobuf::PingRequest ping_request;
  EXPECT_TRUE(ping_request.ParseFromString(message.data())); // us
  EXPECT_TRUE(ping_request.ping());
  EXPECT_TRUE(ping_request.has_timestamp());
  EXPECT_TRUE(ping_request.timestamp() > static_cast<int32_t>(GetTimeStamp() - 2));
  EXPECT_TRUE(ping_request.timestamp() < static_cast<int32_t>(GetTimeStamp() + 1));
  EXPECT_EQ(message.destination_id(), destination);
  EXPECT_EQ(message.source_id(), keys.identity);
  EXPECT_FALSE(message.data().empty());
  EXPECT_EQ(message.replication(), 1);
  EXPECT_EQ(message.type(), 1);
  EXPECT_FALSE(message.routing_failure());
  EXPECT_EQ(message.id(), 0);
  EXPECT_FALSE(message.client_node());
  EXPECT_FALSE(message.has_relay());
}

TEST(RPC, BEH_ConnectMessageInitialised) {

  rudp::EndpointPair our_endpoint;
  our_endpoint.external.address().from_string("192.168.1.1");
  our_endpoint.external.port(5000);
  ASSERT_TRUE(rpcs::Connect(NodeId(RandomString(64)), our_endpoint, "id").IsInitialized());
}

TEST(RPC, BEH_ConnectMessageNode) {
  NodeInfo us(MakeNode());
  rudp::EndpointPair endpoint;
  endpoint.local = us.endpoint;
  endpoint.external = us.endpoint;
  std::string destination = RandomString(64);
  protobuf::Message message = rpcs::Connect(NodeId(destination), endpoint, us.node_id.String());
  protobuf::ConnectRequest connect_request;
  EXPECT_TRUE(message.IsInitialized());
  EXPECT_TRUE(connect_request.ParseFromString(message.data())); // us
  EXPECT_FALSE(connect_request.bootstrap());
  EXPECT_TRUE(connect_request.has_timestamp());
  EXPECT_TRUE(connect_request.timestamp() > static_cast<int32_t>(GetTimeStamp() - 2));
  EXPECT_TRUE(connect_request.timestamp() < static_cast<int32_t>(GetTimeStamp() + 1));
  EXPECT_EQ(message.destination_id(), destination);
  EXPECT_EQ(message.source_id(), us.node_id.String());
  EXPECT_FALSE(message.data().empty());
  EXPECT_EQ(message.replication(), 1);
  EXPECT_EQ(message.type(), 2);
  EXPECT_FALSE(message.routing_failure());
  EXPECT_EQ(message.id(), 0);
  EXPECT_FALSE(message.client_node());
  EXPECT_FALSE(message.has_relay());
}

TEST(RPC, BEH_FindNodesMessageInitialised) {
  ASSERT_TRUE(rpcs::FindNodes(NodeId(RandomString(64))).IsInitialized());
}
TEST(RPC, BEH_FindNodesMessageNode) {
  NodeInfo us(MakeNode());
  std::string destination = RandomString(64);
  protobuf::Message message = rpcs::FindNodes(us.node_id, us.endpoint);
  protobuf::FindNodesRequest find_nodes_request;
  EXPECT_TRUE(find_nodes_request.ParseFromString(message.data())); // us
  EXPECT_TRUE(find_nodes_request.num_nodes_requested() == Parameters::closest_nodes_size);
  EXPECT_EQ(find_nodes_request.target_node(), us.node_id.String());
  EXPECT_TRUE(find_nodes_request.has_timestamp());
  EXPECT_TRUE(find_nodes_request.timestamp() > static_cast<int32_t>(GetTimeStamp() - 2));
  EXPECT_TRUE(find_nodes_request.timestamp() < static_cast<int32_t>(GetTimeStamp() + 1));
  EXPECT_EQ(message.destination_id(), us.node_id.String());
  EXPECT_EQ(message.source_id(), us.node_id.String());
  EXPECT_FALSE(message.data().empty());
  EXPECT_EQ(message.replication(), 1);
  EXPECT_EQ(message.type(), 3);
  EXPECT_FALSE(message.routing_failure());
  EXPECT_EQ(message.id(), 0);
  EXPECT_FALSE(message.client_node());
  EXPECT_FALSE(message.has_relay());
}






}  // namespace test
}  // namespace routing
}  // namespace maidsafe
