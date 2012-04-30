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

#ifndef MAIDSAFE_RUDP_CORE_SESSION_H_
#define MAIDSAFE_RUDP_CORE_SESSION_H_

#include <cstdint>
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/asio/ip/udp.hpp"
#include "maidsafe/rudp/packets/handshake_packet.h"

namespace maidsafe {

namespace rudp {

namespace detail {

class Peer;
class TickTimer;

class Session {
 public:
  explicit Session(Peer &peer,                                // NOLINT (Fraser)
                   TickTimer &tick_timer,
                   boost::asio::ip::udp::endpoint &this_external_endpoint);

//  // Open the session as a client or server.
//  enum Mode { kClient, kServer };
//  void Open(uint32_t id, uint32_t sequence_number, Mode mode);

  // Open the session.
  void Open(uint32_t id, uint32_t sequence_number);

  // Get whether the session is already open. May not be connected.
  bool IsOpen() const;

  // Get whether the session is currently connected to the peer.
  bool IsConnected() const;

  // Get the id assigned to the session.
  uint32_t Id() const;

  // Get the first sequence number for packets received.
  uint32_t ReceivingSequenceNumber() const;

  // Get the peer connection type.
  uint32_t PeerConnectionType() const;

  // Close the session. Clears the id.
  void Close();

  // Handle a handshake packet.
  void HandleHandshake(const HandshakePacket &packet);

  // Handle a tick in the system time.
  void HandleTick();

 private:
  // Disallow copying and assignment.
  Session(const Session&);
  Session &operator=(const Session&);

  // Helper functions to send the packets that make up the handshaking process.
  void SendPacket();
  void SendConnectionRequest();
  void SendCookie();
  void SendConnectionAccepted();

  // The peer with which we are communicating.
  Peer &peer_;

  // The timer used to generate tick events.
  TickTimer &tick_timer_;

  // This node's external endpoint as viewed by peer.
  boost::asio::ip::udp::endpoint &this_external_endpoint_;

  // The local socket id.
  uint32_t id_;

  // The initial sequence number for packets sent in this session.
  uint32_t sending_sequence_number_;

  // The initial sequence number for packets received in this session.
  uint32_t receiving_sequence_number_;

  // The peer's connection type
  uint32_t peer_connection_type_;

  // The state of the session.
  enum State { kClosed, kProbing, kHandshaking, kConnected } state_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_CORE_SESSION_H_
