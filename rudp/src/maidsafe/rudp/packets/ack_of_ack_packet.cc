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

#include "maidsafe/rudp/packets/ack_of_ack_packet.h"

namespace asio = boost::asio;

namespace maidsafe {

namespace rudp {

namespace detail {

AckOfAckPacket::AckOfAckPacket() {
  SetType(kPacketType);
}

uint32_t AckOfAckPacket::AckSequenceNumber() const {
  return AdditionalInfo();
}

void AckOfAckPacket::SetAckSequenceNumber(uint32_t n) {
  SetAdditionalInfo(n);
}

bool AckOfAckPacket::IsValid(const asio::const_buffer &buffer) {
  return (IsValidBase(buffer, kPacketType) &&
          (asio::buffer_size(buffer) == kPacketSize));
}

bool AckOfAckPacket::Decode(const asio::const_buffer &buffer) {
  return DecodeBase(buffer, kPacketType);
}

size_t AckOfAckPacket::Encode(const asio::mutable_buffer &buffer) const {
  return EncodeBase(buffer);
}

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe
