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

#include <bitset>
#include <memory>
#include <vector>
#include "maidsafe/common/test.h"

#include "maidsafe/common/utils.h"
#include "maidsafe/routing/routing_table.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/rudp/managed_connections.h"
#include "maidsafe/routing/tests/test_utils.h"
#include "maidsafe/routing/node_id.h"
#include "maidsafe/routing/log.h"

namespace maidsafe {
namespace routing {
namespace test {

TEST(RoutingTableTest, FUNC_AddCloseNodes) {
    asymm::Keys keys;
    keys.identity = RandomString(64);
  RoutingTable RT(keys);
  NodeInfo node;
  // check the node is useful when false is set
  for (unsigned int i = 0; i < Parameters::closest_nodes_size ; ++i) {
     node.node_id = NodeId(RandomString(64));
     EXPECT_TRUE(RT.CheckNode(node));
  }
  EXPECT_EQ(RT.Size(), 0);
  asymm::PublicKey dummy_key;
  // check we cannot input nodes with invalid public_keys
  for (int i = 0; i < Parameters::closest_nodes_size ; ++i) {
     NodeInfo node(MakeNode());
     node.endpoint.port(1501 + i);  // has to be unique
     node.public_key = dummy_key;
     EXPECT_FALSE(RT.AddNode(node));
  }
  EXPECT_EQ(RT.Size(), 0);

  // everything should be set to go now
  // TODO should we also test for valid enpoints ??
  // TODO we should fail when public keys are the same
  for (int i = 0; i < Parameters::closest_nodes_size ; ++i) {
     node = MakeNode();
     node.endpoint.port(1501 + i);  // has to be unique
     EXPECT_TRUE(RT.AddNode(node));
  }
  EXPECT_EQ(RT.Size(), Parameters::closest_nodes_size);
}

TEST(RoutingTableTest, FUNC_AddTooManyNodes) {
    asymm::Keys keys;
    keys.identity = RandomString(64);
  RoutingTable RT(keys);
  for (int i = 0;
       RT.Size() < Parameters::max_routing_table_size; ++i) {
     NodeInfo node(MakeNode());
     node.endpoint.port(1501 + i);  // has to be unique
     EXPECT_TRUE(RT.AddNode(node));
  }
  EXPECT_EQ(RT.Size(), Parameters::max_routing_table_size);
  size_t count(0);
  for (int i = 0; i < 100; ++i) {
     NodeInfo node(MakeNode());
     node.endpoint.port(1700 + i);  // has to be unique
     if (RT.CheckNode(node)) {
        EXPECT_TRUE(RT.AddNode(node));
       ++count;
     }
  }
  if (count > 0)
     DLOG(INFO) << "made space for " << count << " node(s) in routing table";
  EXPECT_EQ(RT.Size(), Parameters::max_routing_table_size);
}

TEST(RoutingTableTest, BEH_CloseAndInRangeCheck) {
  asymm::Keys keys;
  keys.identity = RandomString(64);
  RoutingTable RT(keys);
  // Add some nodes to RT
  NodeId my_node(keys.identity);
  for (int i = 0;
       RT.Size() < Parameters::max_routing_table_size;
       ++i) {
     NodeInfo node(MakeNode());
     node.endpoint.port(1501 + i);  // has to be unique
     EXPECT_TRUE(RT.AddNode(node));
  }
  EXPECT_EQ(RT.Size(), Parameters::max_routing_table_size);
  std::string my_id_encoded(my_node.ToStringEncoded(NodeId::kBinary));
  my_id_encoded[511] = (my_id_encoded[511] == '0' ? '1' : '0');
  NodeId my_closest_node(NodeId(my_id_encoded, NodeId::kBinary));
  EXPECT_TRUE(RT.AmIClosestNode(my_closest_node));
  EXPECT_TRUE(RT.IsMyNodeInRange(my_closest_node, 2));
  EXPECT_TRUE(RT.IsMyNodeInRange(my_closest_node, 200));
  EXPECT_TRUE(RT.AmIClosestNode(my_closest_node));
  EXPECT_EQ(RT.Size(), Parameters::max_routing_table_size);
  // get closest nodes to me
  std::vector<NodeId> close_nodes(RT.GetClosestNodes(my_node,
                                              Parameters::closest_nodes_size));
  // Check against individually selected close nodes
  for (uint16_t i = 0; i < Parameters::closest_nodes_size; ++i)
    EXPECT_TRUE(std::find(close_nodes.begin(),
                          close_nodes.end(),
                          RT.GetClosestNode(my_node, i).node_id)
                              != close_nodes.end());
  // add the node now
     NodeInfo node(MakeNode());
     node.endpoint.port(1502);  // duplicate endpoint
     node.node_id = my_closest_node;
     EXPECT_FALSE(RT.AddNode(node));
     node.endpoint.port(25000);
     EXPECT_TRUE(RT.AddNode(node));
  // should now be closest node to itself :-)
  EXPECT_EQ(RT.GetClosestNode(my_closest_node, 0).node_id.String(),
            my_closest_node.String());
  EXPECT_EQ(RT.Size(), Parameters::max_routing_table_size);
  EXPECT_TRUE(RT.DropNode(node.endpoint));
  EXPECT_EQ(RT.Size(), Parameters::max_routing_table_size - 1);
  EXPECT_TRUE(RT.AddNode(node));
  EXPECT_EQ(RT.Size(), Parameters::max_routing_table_size);
  EXPECT_FALSE(RT.AddNode(node));
  EXPECT_EQ(RT.Size(), Parameters::max_routing_table_size);
  EXPECT_TRUE(RT.DropNode(node.endpoint));
  EXPECT_EQ(RT.Size(), Parameters::max_routing_table_size -1);
  EXPECT_FALSE(RT.DropNode(node.endpoint));
}

}  // namespace test
}  // namespace routing
}  // namespace maidsafe
