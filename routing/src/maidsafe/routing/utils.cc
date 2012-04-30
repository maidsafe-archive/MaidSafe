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
#include "maidsafe/rudp/return_codes.h"
#include "maidsafe/routing/routing_pb.h"
#include "maidsafe/routing/routing_table.h"
#include "maidsafe/routing/parameters.h"

namespace maidsafe {

namespace routing {

void SendOn(protobuf::Message message,
            rudp::ManagedConnections &rudp,
            RoutingTable &routing_table,
            Endpoint endpoint) {
  std::string signature;
  asymm::Sign(message.data(), routing_table.kKeys().private_key, &signature);
  message.set_signature(signature);
  if (endpoint.address().is_unspecified()) {
    if ((message.has_relay()) && (routing_table.AmIClosestNode(NodeId(message.destination_id())))) {
      endpoint = Endpoint(boost::asio::ip::address::from_string(message.relay().ip()),
                          message.relay().port());
      DLOG(INFO) << "Sending to non routing table node message type : "
                 << message.type() << " message"
                 << " to " << HexSubstr(message.source_id())
                 << " From " << HexSubstr(routing_table.kKeys().identity);
    }else if (routing_table.Size() > 0) {
      endpoint = routing_table.GetClosestNode(NodeId(message.destination_id()), 0).endpoint;
    } else {
      DLOG(ERROR) << " No Endpoint to send to, Aborting Send!"
                  << " Attempt to send a type : " << message.type() << " message"
                  << " to " << HexSubstr(message.source_id())
                  << " From " << HexSubstr(routing_table.kKeys().identity);
    return;
    }
  }
  int send_status = rudp.Send(endpoint, message.SerializeAsString());
  if (send_status != rudp::kSuccess)
   DLOG(ERROR) << " Send error !!! = " << send_status;
}


}  // namespace routing

}  // namespace maidsafe
