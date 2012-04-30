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

#include "maidsafe/rudp/packets/keepalive_packet.h"

namespace asio = boost::asio;

namespace maidsafe {

namespace rudp {

namespace detail {

KeepalivePacket::KeepalivePacket() {
  SetType(kPacketType);
}

uint32_t KeepalivePacket::SequenceNumber() const {
  return AdditionalInfo();
}

void KeepalivePacket::SetSequenceNumber(uint32_t n) {
  SetAdditionalInfo(n);
}

bool KeepalivePacket::IsValid(const asio::const_buffer &buffer) {
  return (IsValidBase(buffer, kPacketType) &&
          (asio::buffer_size(buffer) == kPacketSize));
}

bool KeepalivePacket::IsRequest() const {
  return (AdditionalInfo() & 0x00000001);
}

bool KeepalivePacket::IsResponse() const {
  return !IsRequest();
}

bool KeepalivePacket::IsResponseOf(const uint32_t &sequence_number) const {
  BOOST_VERIFY(IsResponse() && (sequence_number & 0x00000001));
  return (sequence_number + 1 == SequenceNumber());
}

bool KeepalivePacket::Decode(const asio::const_buffer &buffer) {
  if (!IsValid(buffer))
    return false;
  return DecodeBase(buffer, kPacketType);
}

size_t KeepalivePacket::Encode(const asio::mutable_buffer &buffer) const {
  return EncodeBase(buffer);
}

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe
