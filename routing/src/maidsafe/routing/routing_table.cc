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

#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/routing_table.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/node_id.h"
#include "maidsafe/routing/log.h"

namespace maidsafe {
namespace routing {

NodeInfo::NodeInfo()  :
  node_id(), public_key(), rank(), bucket(99999), endpoint(), dimension_1(),
  dimension_2(), dimension_3(), dimension_4() {}

RoutingTable::RoutingTable(const asymm::Keys &keys)
    : keys_(keys),
      sorted_(false),
      kNodeId_(NodeId(keys_.identity)),
      routing_table_nodes_(),
      mutex_(),
      close_node_from_to_signal_()
      {}

boost::signals2::signal<void(std::string, std::string)>
                            &RoutingTable::CloseNodeReplacedOldNewSignal() {
  return close_node_from_to_signal_;
}

bool RoutingTable::CheckNode(NodeInfo& node) {
  return AddOrCheckNode(node, false);
}

bool RoutingTable::AddNode(NodeInfo& node) {
  return AddOrCheckNode(node, true);
}

bool RoutingTable::AddOrCheckNode(maidsafe::routing::NodeInfo& node,
                                  const bool remove) {
  std::lock_guard<std::mutex> lock(mutex_);

  if (node.node_id == kNodeId_) {
    return false;
  }
  // if we already have node return false
  if (std::find_if(routing_table_nodes_.begin(),
                   routing_table_nodes_.end(),
                   [&node](const NodeInfo &i)->bool
                   { return i.node_id ==  node.node_id; })
                 != routing_table_nodes_.end())
    return false;
  if (MakeSpaceForNodeToBeAdded(node, remove)) {
      if (remove)
        routing_table_nodes_.push_back(node);
      return true;
  }
  return false;
}

uint16_t RoutingTable::Size() {
    return static_cast<uint16_t>(routing_table_nodes_.size());
}

asymm::Keys RoutingTable::kKeys() const {
  return keys_;
}

bool RoutingTable::DropNode(const boost::asio::ip::udp::endpoint &endpoint) {
    for (auto it = routing_table_nodes_.begin();
         it != routing_table_nodes_.end(); ++it) {
       if(((*it).endpoint ==  endpoint)){
          routing_table_nodes_.erase(it);
          return true;
       }
    }
   return false;
}

bool RoutingTable::GetNodeInfo(const boost::asio::ip::udp::endpoint &endpoint,
                               NodeInfo *node_info) {
    for (auto it = routing_table_nodes_.begin();
         it != routing_table_nodes_.end(); ++it) {
       if(((*it).endpoint ==  endpoint)){
          *node_info = (*it);
          return true;
       }
    }
   return false;
}


bool RoutingTable::AmIClosestNode(const NodeId& node_id) {
  if (!node_id.IsValid()) {
    DLOG(ERROR) << "Invalid node_id passed";
    return false;
  }
  if (routing_table_nodes_.empty())
    return true;
  SortFromThisNode(node_id);
  return ((kNodeId_ ^ node_id) <
          (node_id ^ routing_table_nodes_[0].node_id));
}

// checks paramters are real
bool RoutingTable::CheckValidParameters(const NodeInfo& node)const {
  if ((!asymm::ValidateKey(node.public_key, 0))) {
    DLOG(INFO) << "invalid public key";
    return false;
  }

  if (node.bucket == 99999) {
        DLOG(INFO) << "invalid bucket index";
    return false;
  }

  return CheckParametersAreUnique(node);
}

bool RoutingTable::CheckParametersAreUnique(const NodeInfo& node) const {

    // if we already have a duplicate public key return false
  if (std::find_if(routing_table_nodes_.begin(),
                   routing_table_nodes_.end(),
                   [&node](const NodeInfo &i)->bool
                   { return  asymm::MatchingPublicKeys(i.public_key,
                                                       node.public_key);})
                 != routing_table_nodes_.end()) {
    DLOG(INFO) << "Already have node with this public key";
    return false;
  }

  // if we already have a duplicate endpoint return false
    if (std::find_if(routing_table_nodes_.begin(),
                   routing_table_nodes_.end(),
                   [&node](const NodeInfo &i)->bool
                   { return (i.endpoint == node.endpoint); })
                 != routing_table_nodes_.end()) {
     DLOG(INFO) << "Already have node with this endpoint";
     return false;
    }
    // node_id was checked in AddNode() so if were here then were unique
  return true;
}

bool RoutingTable::MakeSpaceForNodeToBeAdded(maidsafe::routing::NodeInfo& node,
                                             const bool remove) {
  node.bucket = BucketIndex(node.node_id);
  if ((remove) && (!CheckValidParameters(node))) {
    DLOG(INFO) << "Invalid Parameters";
    return false;
  }

  if (Size() < Parameters::max_routing_table_size)
    return true;

  SortFromThisNode(kNodeId_);
  NodeInfo furthest_close_node =
           routing_table_nodes_[Parameters::closest_nodes_size];
  const auto not_found = routing_table_nodes_.end();
  const auto furthest_close_node_iter =
       routing_table_nodes_.begin() + Parameters::closest_nodes_size;

  if ((furthest_close_node.node_id ^ kNodeId_) >
     (kNodeId_ ^ node.node_id)) {
     BOOST_ASSERT_MSG(node.bucket <= furthest_close_node.bucket,
                       "close node replacement to a larger bucket");

     if (remove) {
      close_node_from_to_signal_((*furthest_close_node_iter).node_id.String(),
                                 node.node_id.String());
      routing_table_nodes_.erase(furthest_close_node_iter);
     }
    return true;
  }

  for (auto it = furthest_close_node_iter; it != not_found; ++it) {
    if (node.bucket >= (*it).bucket) {
      // stop searching as it's worthless
      return false;
    }
    // safety net
    if ((not_found - it) < (Parameters::bucket_target_size + 1)) {
      // reached end of checkable area
      return false;
    }

    if ((*it).bucket == (*(it + Parameters::bucket_target_size + 1)).bucket) {
      // here we know the node should fit into a bucket if
      // the bucket has too many nodes AND node to add
      // has a lower bucketindex
      BOOST_ASSERT(node.bucket < (*it).bucket); //,
                       //"node replacement to a larger bucket");
      if (remove) {
        routing_table_nodes_.erase(it);
      }
      return true;
    }
  }
  return false;
}

void RoutingTable::SortFromThisNode(const NodeId &from) {
  if ((!sorted_)  || (from != kNodeId_)) {
    std::sort(routing_table_nodes_.begin(),
              routing_table_nodes_.end(),
    [this, from](const NodeInfo &i, const NodeInfo &j) {
    return (i.node_id ^ from) < (j.node_id ^ from);
    } ); // NOLINT
  }
  if (from != kNodeId_)
    sorted_ = false;
}

bool RoutingTable::IsMyNodeInRange(const NodeId& node_id, const uint16_t range)  {
  if (routing_table_nodes_.size() < range)
    return true;

  SortFromThisNode(kNodeId_);

  return (routing_table_nodes_[range].node_id ^ kNodeId_) >
         (node_id ^ kNodeId_);
}

// bucket 0 is us, 511 is furthest bucket (should fill first)
int16_t RoutingTable::BucketIndex(const NodeId &rhs) const {
  uint16_t bucket = kKeySizeBits - 1;  // (n-1)
  std::string this_id_binary = kNodeId_.ToStringEncoded(NodeId::kBinary);
  std::string rhs_id_binary = rhs.ToStringEncoded(NodeId::kBinary);
  auto this_it = this_id_binary.begin();
  auto rhs_it = rhs_id_binary.begin();

  for (;this_it != this_id_binary.end(); ++this_it, ++rhs_it) {
    if (*this_it != *rhs_it)
      return bucket;
    --bucket;
  }
  return bucket;
}

NodeInfo RoutingTable::GetClosestNode(const NodeId &from,
                                    unsigned int node_number) {
  SortFromThisNode(from);
  return routing_table_nodes_[node_number];
}

std::vector<NodeId> RoutingTable::GetClosestNodes(const NodeId &from,
                                                  uint16_t number_to_get) {
  std::vector<NodeId>close_nodes;
  std::lock_guard<std::mutex> lock(mutex_);
  unsigned int count = std::min(number_to_get, Size());
  SortFromThisNode(from);
  close_nodes.resize(count);

  for (unsigned int i = 0; i < count; ++i) {
    close_nodes.push_back(routing_table_nodes_[i].node_id);
  }
  return close_nodes;
}

}  // namespace routing
}  // namespace maidsafe
