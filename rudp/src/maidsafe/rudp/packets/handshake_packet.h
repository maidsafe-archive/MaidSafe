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

#ifndef MAIDSAFE_RUDP_PACKETS_HANDSHAKE_PACKET_H_
#define MAIDSAFE_RUDP_PACKETS_HANDSHAKE_PACKET_H_

#include <cstdint>
#include <string>

#include "boost/asio/buffer.hpp"
#include "boost/asio/ip/udp.hpp"
#include "boost/system/error_code.hpp"

#include "maidsafe/rudp/packets/control_packet.h"

namespace maidsafe {

namespace rudp {

namespace detail {

class HandshakePacket : public ControlPacket {
 public:
  enum { kPacketSize = ControlPacket::kHeaderSize + 50 };
  enum { kPacketType = 0 };

  HandshakePacket();
  virtual ~HandshakePacket() {}

  uint32_t RudpVersion() const;
  void SetRudpVersion(uint32_t n);

  static const uint32_t kStreamSocketType = 0;
  static const uint32_t kDatagramSocketType = 1;
  uint32_t SocketType() const;
  void SetSocketType(uint32_t n);

  uint32_t InitialPacketSequenceNumber() const;
  void SetInitialPacketSequenceNumber(uint32_t n);

  uint32_t MaximumPacketSize() const;
  void SetMaximumPacketSize(uint32_t n);

  uint32_t MaximumFlowWindowSize() const;
  void SetMaximumFlowWindowSize(uint32_t n);

  uint32_t ConnectionType() const;
  void SetConnectionType(uint32_t n);

  uint32_t SocketId() const;
  void SetSocketId(uint32_t n);

  uint32_t SynCookie() const;
  void SetSynCookie(uint32_t n);

  boost::asio::ip::udp::endpoint Endpoint() const;
  void SetEndpoint(const boost::asio::ip::udp::endpoint &endpoint);

  static bool IsValid(const boost::asio::const_buffer &buffer);
  bool Decode(const boost::asio::const_buffer &buffer);
  size_t Encode(const boost::asio::mutable_buffer &buffer) const;

 private:
  uint32_t rudp_version_;
  uint32_t socket_type_;
  uint32_t initial_packet_sequence_number_;
  uint32_t maximum_packet_size_;
  uint32_t maximum_flow_window_size_;
  uint32_t connection_type_;
  uint32_t socket_id_;
  uint32_t syn_cookie_;
  boost::asio::ip::udp::endpoint endpoint_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_PACKETS_HANDSHAKE_PACKET_H_
