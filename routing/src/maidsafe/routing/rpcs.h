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

#ifndef MAIDSAFE_ROUTING_RPCS_H_
#define MAIDSAFE_ROUTING_RPCS_H_

#include "maidsafe/routing/node_id.h"
#include "maidsafe/routing/routing_pb.h"

namespace maidsafe {

namespace routing {

namespace rpcs {

const protobuf::Message Ping(const NodeId &node_id,
                       const std::string &identity);
const protobuf::Message Connect(const NodeId &node_id,
                          const rudp::EndpointPair &our_endpoint,
                          const std::string &identity);
const protobuf::Message FindNodes(const NodeId &node_id,
                                  boost::asio::ip::udp::endpoint local_endpoint =
                                  boost::asio::ip::udp::endpoint());

} // namespace rpcs

}  // namespace routing

}  // namespace maidsafe

#endif  // MAIDSAFE_ROUTING_RPCS_H_




