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
// Original author: Christopher M. Kohlhoff (chris at kohlhoff dot com)

#ifndef MAIDSAFE_RUDP_CORE_PEER_H_
#define MAIDSAFE_RUDP_CORE_PEER_H_

#include <cstdint>

#include "boost/asio/ip/udp.hpp"
#include "maidsafe/rudp/core/multiplexer.h"

namespace maidsafe {

namespace rudp {

namespace detail {

class Peer {
 public:
  explicit Peer(Multiplexer &multiplexer)  // NOLINT (Fraser)
    : multiplexer_(multiplexer), endpoint_(), id_(0) {}

  const boost::asio::ip::udp::endpoint &Endpoint() const { return endpoint_; }
  void SetEndpoint(const boost::asio::ip::udp::endpoint &ep) { endpoint_ = ep; }

  uint32_t Id() const { return id_; }
  void SetId(uint32_t id) { id_ = id; }

  template <typename Packet>
  ReturnCode Send(const Packet &packet) {
    return multiplexer_.SendTo(packet, endpoint_);
  }

 private:
  // Disallow copying and assignment.
  Peer(const Peer&);
  Peer &operator=(const Peer&);

  // The multiplexer used to send and receive UDP packets.
  Multiplexer &multiplexer_;

  // The remote socket's endpoint and identifier.
  boost::asio::ip::udp::endpoint endpoint_;
  uint32_t id_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_CORE_PEER_H_
