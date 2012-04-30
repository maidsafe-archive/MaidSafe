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

#include <utility>
#include <chrono>
#include <thread>
#include <future>
#include "boost/filesystem/fstream.hpp"
#include "boost/filesystem/exception.hpp"
#include "maidsafe/common/utils.h"
#include "maidsafe/rudp/managed_connections.h"
#include "maidsafe/rudp/return_codes.h"
#include "maidsafe/routing/return_codes.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/routing_pb.h"
#include "maidsafe/routing/node_id.h"
#include "maidsafe/routing/routing_table.h"
#include "maidsafe/routing/timer.h"
#include "maidsafe/routing/version.h"
#include "maidsafe/routing/bootstrap_file_handler.h"
#include "maidsafe/routing/return_codes.h"
#include "maidsafe/routing/utils.h"
#include "maidsafe/routing/message_handler.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/routing_api_impl.h"

namespace fs = boost::filesystem;
namespace bs2 = boost::signals2;

namespace maidsafe {

namespace routing {

Routing::Routing(const asymm::Keys &keys,
                 const boost::filesystem::path &boostrap_file_path,
                 NodeValidationFunctor node_validation_functor,
                 const bool client_mode)
    : impl_(new RoutingPrivate(keys, boostrap_file_path, node_validation_functor, client_mode)) {
  // test path
  std::string dummy_content;
  // not catching exceptions !!
  fs::ifstream file_in(boostrap_file_path, std::ios::in | std::ios::binary);
  fs::ofstream file_out(boostrap_file_path, std::ios::out | std::ios::binary);
  if(file_in.good()) {
    if (fs::exists(boostrap_file_path)) {
      fs::file_size(boostrap_file_path);  // throws
    } else if (file_out.good()) {
      file_out.put('c');
    fs::file_size(boostrap_file_path);  // throws
    fs::remove(boostrap_file_path);
    } else {
      fs::file_size(boostrap_file_path);  // throws
    }
  } else {
    fs::file_size(boostrap_file_path);  // throws
  }
  if (client_mode) {
    Parameters::max_routing_table_size = Parameters::closest_nodes_size;
  }
  Join();
}

Routing::~Routing() {}

int Routing::GetStatus() {
 if (impl_->routing_table_.Size() == 0) {
    rudp::EndpointPair endpoint;
    if(impl_->rudp_.GetAvailableEndpoint(&endpoint) != rudp::kSuccess) {
      if (impl_->rudp_.GetAvailableEndpoint(&endpoint)
                                          == rudp::kNoneAvailable) 
        return kNotJoined;
    } 
 } else {
  return impl_->routing_table_.Size();
 }
 return 0;
}

// drop existing routing table and restart
// the endpoint is the endpoint to connect to.
bool Routing::BootStrapFromThisEndpoint(const boost::asio::ip::udp::endpoint&
                                                                     endpoint,
                              boost::asio::ip::udp::endpoint local_endpoint) {
  LOG(INFO) << " Entered bootstrap IP address : " << endpoint.address().to_string();
  LOG(INFO) << " Entered bootstrap Port       : " << endpoint.port();
  if (endpoint.address().is_unspecified()) {
    DLOG(ERROR) << "Attempt to boot from unspecified endpoint ! aborted";
    return false;
  }
  for (unsigned int i = 0; i < impl_->routing_table_.Size(); ++i) {
    NodeInfo remove_node =
    impl_->routing_table_.GetClosestNode(NodeId(impl_->routing_table_.kKeys().identity), 0);
    impl_->rudp_.Remove(remove_node.endpoint);
    impl_->routing_table_.DropNode(remove_node.endpoint);
  }
  impl_->network_status_signal_(impl_->routing_table_.Size());
  impl_->bootstrap_nodes_.clear();
  impl_->bootstrap_nodes_.push_back(endpoint);
  return Join(local_endpoint);
}

bool Routing::Join(boost::asio::ip::udp::endpoint local_endpoint) {
  if (impl_->bootstrap_nodes_.empty()) {
    LOG(INFO) << "No bootstrap nodes Aborted Join !!";
    return false;
  }
  rudp::MessageReceivedFunctor message_recieved(std::bind(&Routing::ReceiveMessage,
                                                          this,
                                                          std::placeholders::_1));
  rudp::ConnectionLostFunctor connection_lost(
                                          std::bind(&Routing::ConnectionLost,
                                                  this,
                                                  std::placeholders::_1));
  boost::asio::ip::udp::endpoint bootstrap_endpoint(impl_->rudp_.Bootstrap(
                                                    impl_->bootstrap_nodes_,
                                                    message_recieved,
                                                    connection_lost,
                                                    local_endpoint));

  if (bootstrap_endpoint.address().is_unspecified() &&
      (local_endpoint.address().is_unspecified())) {
    DLOG(ERROR) << "could not get bootstrap address and not zero state";
    return false;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  auto boot = std::async(std::launch::async,
    [&]{ return impl_->rudp_.Send(bootstrap_endpoint,
                rpcs::FindNodes(NodeId(impl_->keys_.identity), local_endpoint).SerializeAsString()); });
  return (boot.get() == 0);
}

int Routing::Send(const std::string destination_id,
                  const std::string data,
                  const uint16_t type,
                  const MessageReceivedFunctor response_functor,
                  const uint16_t timeout_seconds,
                  const bool direct) {
  if (destination_id.empty()) {
    DLOG(ERROR) << "No destination id, aborted send";
    return kInvalidDestinatinId;
  }
  if (data.empty() && (type != 100)) {
    DLOG(ERROR) << "No data, aborted send";
    return kEmptyData;
  }
  protobuf::Message proto_message;
  proto_message.set_id(0);
  // TODO(see if ANONYMOUS and Endpoint required here
  proto_message.set_source_id(impl_->routing_table_.kKeys().identity);
  proto_message.set_destination_id(destination_id);
  proto_message.set_data(data);
  proto_message.set_direct(direct);
  proto_message.set_type(type);
  SendOn(proto_message, impl_->rudp_, impl_->routing_table_);
  return 0;
}

void Routing::ValidateThisNode(const std::string &node_id,
                              const asymm::PublicKey &public_key,
                              const boost::asio::ip::udp::endpoint &their_endpoint,
                              const boost::asio::ip::udp::endpoint &our_endpoint,
                              bool client) {
  NodeInfo node_info;
  // TODO(dirvine) Add Managed Connection  here !!!
  node_info.node_id = NodeId(node_id);
  node_info.public_key = public_key;
  node_info.endpoint = their_endpoint;
  impl_->rudp_.Add(their_endpoint, our_endpoint, node_id);
  if (client) {
    impl_->direct_non_routing_table_connections_.push_back(node_info);
  } else {
    impl_->routing_table_.AddNode(node_info);
    if (impl_->bootstrap_nodes_.size() > 1000) {
    impl_->bootstrap_nodes_.erase(impl_->bootstrap_nodes_.begin());
    }
    impl_->bootstrap_nodes_.push_back(their_endpoint);
    std::error_code error;
    WriteBootstrapFile(impl_->bootstrap_nodes_,
                       impl_->bootstrap_file_path_);
  }
}

bs2::signal<void(int, std::string)> &Routing::MessageReceivedSignal() {
  return impl_->message_received_signal_;
}

bs2::signal<void(int16_t)> &Routing::NetworkStatusSignal() {
  return impl_->network_status_signal_;
}

bs2::signal<void(std::string, std::string)>
                            &Routing::CloseNodeReplacedOldNewSignal() {
  return impl_->routing_table_.CloseNodeReplacedOldNewSignal();
}

  boost::signals2::signal<void(const std::string&,
                           const boost::asio::ip::udp::endpoint&,
                           const bool,
                           const boost::asio::ip::udp::endpoint&,
                           NodeValidatedFunctor &)>
                           &Routing::NodeValidationSignal() {
  return impl_->node_validation_signal_;
                           }

void Routing::ReceiveMessage(const std::string &message) {
  protobuf::Message protobuf_message;
  protobuf::ConnectRequest connection_request;
  if (protobuf_message.ParseFromString(message)) {
    DLOG(INFO) << " Message received, type: " << protobuf_message.type() 
               << " from " << HexSubstr(protobuf_message.source_id())
               << " I am " << HexSubstr(impl_->keys_.identity);
    impl_->message_handler_.ProcessMessage(protobuf_message);
  }
}

void Routing::ConnectionLost(const boost::asio::ip::udp::endpoint
                                                        &lost_endpoint) {
  NodeInfo node_info;
  if ((impl_->routing_table_.GetNodeInfo(lost_endpoint, &node_info) &&
     (impl_->routing_table_.IsMyNodeInRange(node_info.node_id,
                                            Parameters::closest_nodes_size)))) {
    SendOn(rpcs::FindNodes(NodeId(impl_->keys_.identity)),
           impl_->rudp_,
           impl_->routing_table_); // close node, get more
  }
  if (!impl_->routing_table_.DropNode(lost_endpoint))
    return;
  for (auto it = impl_->direct_non_routing_table_connections_.begin();
        it != impl_->direct_non_routing_table_connections_.end(); ++it) {
      if((*it).endpoint ==  lost_endpoint) {
        impl_->direct_non_routing_table_connections_.erase(it);
        return;
      }
  }
  for (auto it = impl_->direct_non_routing_table_connections_.begin();
        it != impl_->direct_non_routing_table_connections_.end(); ++it) {
      if((*it).endpoint ==  lost_endpoint) {
        impl_->direct_non_routing_table_connections_.erase(it);
      SendOn(rpcs::FindNodes(NodeId(impl_->keys_.identity)),
      impl_->rudp_,
      impl_->routing_table_);  // close node, get more
      return;
      }
  }
}


}  // namespace routing

}  // namespace maidsafe
