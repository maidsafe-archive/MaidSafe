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

#ifndef MAIDSAFE_ROUTING_ROUTING_TABLE_H_
#define MAIDSAFE_ROUTING_ROUTING_TABLE_H_

#include <thread>
#include <mutex>
#include "maidsafe/routing/routing_pb.h"
#include "maidsafe/routing/node_id.h"



namespace maidsafe {

namespace routing {


struct NodeInfo {
  NodeInfo();
  NodeId node_id;
  asymm::PublicKey public_key;
  int32_t rank;
  int32_t bucket;
  boost::asio::ip::udp::endpoint endpoint;
  int32_t dimension_1;
  int32_t dimension_2;
  int32_t dimension_3;
  int32_t dimension_4;
};

class RoutingTable {
 public:
  explicit RoutingTable(const asymm::Keys &keys);
  bool AddNode(NodeInfo &node);
  bool CheckNode(NodeInfo &node);
  bool DropNode(const boost::asio::ip::udp::endpoint &endpoint);
  bool GetNodeInfo(const boost::asio::ip::udp::endpoint &endpoint,
                   NodeInfo *node_info);
  bool IsMyNodeInRange(const NodeId &node_id, const uint16_t range);
  bool AmIClosestNode(const NodeId &node_id);
  std::vector<NodeId> GetClosestNodes(const NodeId &from,
                                      uint16_t number_to_get);
  NodeInfo GetClosestNode(const NodeId &from, unsigned int node_number);
  uint16_t Size();
  asymm::Keys kKeys() const;
  boost::signals2::signal<void(std::string, std::string)>
                                           &CloseNodeReplacedOldNewSignal();
 private:
  RoutingTable(const RoutingTable&);
  RoutingTable& operator=(const RoutingTable&);
  bool AddOrCheckNode(NodeInfo &node, const bool remove);
  int16_t BucketIndex(const NodeId &rhs) const;
  bool CheckValidParameters(const NodeInfo &node) const;
  bool CheckParametersAreUnique(const NodeInfo &node) const;
  bool MakeSpaceForNodeToBeAdded(NodeInfo &node, const bool remove_and_add);
  void SortFromThisNode(const NodeId &from);
  void PartialSortFromThisNode(const NodeId &from, int16_t number_to_sort);
  bool RemoveClosecontact(const NodeId &node_id);
  bool AddcloseContact(const protobuf::Contact &contact);
  asymm::Keys keys_;
  bool sorted_;
  const NodeId kNodeId_;
  std::vector<NodeInfo> routing_table_nodes_;
  std::mutex mutex_;
  boost::signals2::signal<void(std::string, std::string)>
                                                    close_node_from_to_signal_;
};

}  // namespace routing

}  // namespace maidsafe

#endif  // MAIDSAFE_ROUTING_ROUTING_TABLE_H_
