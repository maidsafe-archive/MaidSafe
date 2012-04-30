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

#include "maidsafe/common/utils.h"
#include "maidsafe/routing/node_id.h"
#include "maidsafe/routing/routing_pb.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/log.h"

namespace maidsafe {

namespace routing {

namespace rpcs {

// this is maybe not required and might be removed
const protobuf::Message Ping(const NodeId &node_id,
                       const std::string &identity) {
  protobuf::Message message;
  protobuf::PingRequest ping_request;
  ping_request.set_ping(true);
  ping_request.set_timestamp(GetTimeStamp());
  message.set_destination_id(node_id.String());
  message.set_source_id(identity);
  message.set_data(ping_request.SerializeAsString());
  message.set_direct(true);
  message.set_replication(1);
  message.set_type(1);
  message.set_routing_failure(false);
  message.set_id(0);
  message.set_client_node(false);
  BOOST_ASSERT_MSG(message.IsInitialized(), "unintialised message");
  return message;
}

const protobuf::Message Connect(const NodeId &node_id,
                   const rudp::EndpointPair &our_endpoint,
                   const std::string &identity) {
  protobuf::Message message;
  protobuf::Contact *contact;
  protobuf::Endpoint *public_endpoint;
  protobuf::Endpoint *private_endpoint;
  protobuf::ConnectRequest protobuf_connect_request;
  contact = protobuf_connect_request.mutable_contact();
  public_endpoint = contact->mutable_public_endpoint();
  public_endpoint->set_ip(our_endpoint.external.address().to_string());
  public_endpoint->set_port(our_endpoint.external.port());
  private_endpoint = contact->mutable_private_endpoint();
  private_endpoint->set_ip(our_endpoint.local.address().to_string());
  private_endpoint->set_port(our_endpoint.local.port());
  contact->set_node_id(identity);
  protobuf_connect_request.set_timestamp(GetTimeStamp());
  message.set_destination_id(node_id.String());
  message.set_source_id(identity);
  message.set_data(protobuf_connect_request.SerializeAsString());
  message.set_direct(true);
  message.set_replication(1);
  message.set_type(2);
  message.set_routing_failure(false);
  message.set_id(0);
  message.set_client_node(false);
  BOOST_ASSERT_MSG(message.IsInitialized(), "unintialised message");
  return message;
}

const protobuf::Message FindNodes(const NodeId &node_id, boost::asio::ip::udp::endpoint endpoint) {
  protobuf::Message message;
  protobuf::FindNodesRequest find_nodes;
  find_nodes.set_num_nodes_requested(Parameters::closest_nodes_size);
  find_nodes.set_target_node(node_id.String());
  find_nodes.set_timestamp(GetTimeStamp());
  message.set_destination_id(node_id.String());
  message.set_source_id(node_id.String());
  message.set_data(find_nodes.SerializeAsString());
  message.set_direct(false);
  message.set_replication(1);
  message.set_type(3);
  message.set_routing_failure(false);
  message.set_id(0);
  message.set_client_node(false);
  if (!endpoint.address().is_unspecified()) {
    DLOG(INFO) << "RPC IP Address " << endpoint.address().to_string();
    protobuf::Endpoint *pbendpoint;
    pbendpoint = message.mutable_relay();
    pbendpoint->set_ip(endpoint.address().to_string().c_str());
    pbendpoint->set_port(endpoint.port());
  }
  BOOST_ASSERT_MSG(message.IsInitialized(), "unintialised message");
  return message;
}

} // namespace rpcs

}  // namespace routing

}  // namespace maidsafe
