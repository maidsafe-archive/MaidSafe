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

#ifndef MAIDSAFE_RUDP_PACKETS_NEGATIVE_ACK_PACKET_H_
#define MAIDSAFE_RUDP_PACKETS_NEGATIVE_ACK_PACKET_H_

#include <vector>

#include "boost/asio/buffer.hpp"
#include "maidsafe/rudp/packets/control_packet.h"

namespace maidsafe {

namespace rudp {

namespace detail {

class NegativeAckPacket : public ControlPacket {
 public:
  enum { kPacketType = 3 };

  NegativeAckPacket();
  virtual ~NegativeAckPacket() {}

  void AddSequenceNumber(uint32_t n);
  void AddSequenceNumbers(uint32_t first, uint32_t last);
  bool ContainsSequenceNumber(uint32_t n) const;
  bool HasSequenceNumbers() const;

  static bool IsValid(const boost::asio::const_buffer &buffer);
  bool Decode(const boost::asio::const_buffer &buffer);
  size_t Encode(const boost::asio::mutable_buffer &buffer) const;

 private:
  std::vector<uint32_t> sequence_numbers_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_PACKETS_NEGATIVE_ACK_PACKET_H_
