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

#include "boost/thread/shared_mutex.hpp"
#include "boost/thread/mutex.hpp"
#include "maidsafe/common/rsa.h"
#include "maidsafe/rudp/managed_connections.h"
#include "maidsafe/routing/cache_manager.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/message_handler.h"
#include "maidsafe/routing/response_handler.h"
#include "maidsafe/routing/routing_pb.h"
#include "maidsafe/routing/routing_table.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/node_id.h"
#include "maidsafe/routing/return_codes.h"
#include "maidsafe/routing/rpcs.h"
#include "maidsafe/routing/service.h"
#include "maidsafe/routing/utils.h"
#include "maidsafe/routing/log.h"


namespace maidsafe {

namespace routing {

class Timer;

MessageHandler::MessageHandler(
                RoutingTable &routing_table,
                rudp::ManagedConnections &rudp,
                Timer &timer_ptr,
                NodeValidationFunctor node_validation_functor) :
                routing_table_(routing_table),
                rudp_(rudp),
                timer_ptr_(timer_ptr),
                cache_manager_(),
                message_received_signal_(),
                node_validation_functor_(node_validation_functor) {}

boost::signals2::signal<void(int, std::string)>
                                     &MessageHandler::MessageReceivedSignal() {
  return message_received_signal_;
}

void MessageHandler::Send(protobuf::Message& message) {
  message.set_routing_failure(false);
  SendOn(message, rudp_, routing_table_);
}

bool MessageHandler::CheckCacheData(protobuf::Message &message) {
  if (message.type() == -100) {
    cache_manager_.AddToCache(message);
  } else  if (message.type() == 100) {
    if (cache_manager_.GetFromCache(message)) {
      message.set_source_id(routing_table_.kKeys().identity);
      SendOn(message, rudp_, routing_table_);
      return true;
    }
  } else {
    return false;  // means this message is not finished processing
  }
  return false;
}


void MessageHandler::RoutingMessage(protobuf::Message& message) {
  switch (message.type()) {
    case -1 :  // ping
      response::Ping(message);
      break;
    case 1 :
      service::Ping(routing_table_, message);
      break;
    case -2 :  // connect
      response::Connect(message, node_validation_functor_);
      break;
    case 2 :
      service::Connect(routing_table_, rudp_, message);
      break;
    case -3 :   // find_nodes
      response::FindNode(routing_table_, rudp_, message);
      break;
    case 3 :
      service::FindNodes(routing_table_, message);
      break;
    default: // unknown (silent drop)
      return;
  }
  SendOn(message, rudp_, routing_table_);
}

void MessageHandler::DirectMessage(protobuf::Message& message) {
  if (message.has_relay()) {
     boost::asio::ip::udp::endpoint send_to_endpoint;
     send_to_endpoint.address().from_string(message.relay().ip());
     send_to_endpoint.port(message.relay().port());
     rudp_.Send(send_to_endpoint, message.SerializeAsString());
     return;
  }
  if ((message.type() < 100) && (message.type() > -100)) {
    RoutingMessage(message);
    return;
  }
  if (message.type() > 100) {  // request
    message_received_signal_(static_cast<int>(-message.type()),
                             message.data());
    DLOG(INFO) << "Routing message detected";
  } else {  // response
    timer_ptr_.ExecuteTaskNow(message);
    DLOG(INFO) << "Response detected";
  }
}

void MessageHandler::CloseNodesMessage(protobuf::Message& message) {
  if (message.has_relay()) {
     boost::asio::ip::udp::endpoint send_to_endpoint;
     send_to_endpoint.address().from_string(message.relay().ip());
     send_to_endpoint.port(message.relay().port());
     rudp_.Send(send_to_endpoint, message.SerializeAsString());
     return;
  }
  if ((message.direct()) &&
      (!routing_table_.AmIClosestNode(NodeId(message.destination_id())))) {
    SendOn(message, rudp_, routing_table_);
    return;
  }
  // I am closest so will send to all my replicant nodes
  message.set_direct(true);
  auto close =
        routing_table_.GetClosestNodes(NodeId(message.destination_id()),
                                static_cast<uint16_t>(message.replication()));
  for (auto i : close) {
    message.set_destination_id(i.String());
    SendOn(message, rudp_, routing_table_);
  }
}


void MessageHandler::ProcessMessage(protobuf::Message &message) {
  // client connected messages -> out
  if (message.source_id().empty()) {  // relay mode
    // if zero state we may be closest
    if(routing_table_.Size() <= Parameters::closest_nodes_size) {
      if (message.type() == 3) {
        service::FindNodes(routing_table_, message);
        SendOn(message, rudp_, routing_table_);
        return;
      }
    }
    message.set_source_id(routing_table_.kKeys().identity);
    SendOn(message, rudp_, routing_table_);
  }
  // message for me !
  if (message.destination_id() == routing_table_.kKeys().identity) {
    DirectMessage(message);
    return;
  }
  // cache response to get data that's cacheable
  if ((message.type() == -100) && (CheckCacheData(message)))
    return;
  // I am in closest proximity to this message
  if (routing_table_.IsMyNodeInRange(NodeId(message.destination_id()),
                                            Parameters::closest_nodes_size)) {
    if ((message.type() < 100) && (message.type() > -100)) {
      RoutingMessage(message);
      return;
    } else {
      CloseNodesMessage(message);
      return;
    }
  }
  // default
  SendOn(message, rudp_, routing_table_);
}

// // TODO(dirvine) implement client handler
// bool MessageHandler::CheckAndSendToLocalClients(protobuf::Message &message) {
//   bool found(false);
// //   NodeId destination_node(message.destination_id());
// //   std::for_each(client_connections_.begin(),
// //                 client_connections_.end(),
// //                 [&destination_node, &found](const NodeInfo &i)->bool
// //                 {
// //                   if (i.node_id ==  destination_node) {
// //                     found = true;
// //                     // rudp send TODO(dirvine)
// //                   }
// //                   return found;  // lambda return
// //                 });
//   return found;
// }



}  // namespace routing

}  // namespace maidsafe