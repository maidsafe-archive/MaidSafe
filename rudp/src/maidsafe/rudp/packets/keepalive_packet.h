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

#ifndef MAIDSAFE_RUDP_PACKETS_KEEPALIVE_PACKET_H_
#define MAIDSAFE_RUDP_PACKETS_KEEPALIVE_PACKET_H_

#include "boost/asio/buffer.hpp"
#include "maidsafe/rudp/packets/control_packet.h"

namespace maidsafe {

namespace rudp {

namespace detail {

class KeepalivePacket : public ControlPacket {
 public:
  enum { kPacketSize = ControlPacket::kHeaderSize };
  enum { kPacketType = 1 };

  KeepalivePacket();
  virtual ~KeepalivePacket() {}

  void SetSequenceNumber(uint32_t n);
  uint32_t SequenceNumber() const;
  // Request will have odd sequence number
  bool IsRequest() const;
  // Response will have even sequence number
  bool IsResponse() const;
  // Checks if this is a response to provided request sequence number (odd).
  bool IsResponseOf(const uint32_t &sequence_number) const;

  static bool IsValid(const boost::asio::const_buffer &buffer);
  bool Decode(const boost::asio::const_buffer &buffer);
  size_t Encode(const boost::asio::mutable_buffer &buffer) const;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_PACKETS_KEEPALIVE_PACKET_H_
