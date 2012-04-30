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

#include "maidsafe/rudp/packets/negative_ack_packet.h"

namespace asio = boost::asio;

namespace maidsafe {

namespace rudp {

namespace detail {

NegativeAckPacket::NegativeAckPacket()
    : sequence_numbers_() {
  SetType(kPacketType);
}

void NegativeAckPacket::AddSequenceNumber(uint32_t n) {
  assert(n <= 0x7fffffff);
  sequence_numbers_.push_back(n);
}

void NegativeAckPacket::AddSequenceNumbers(uint32_t first, uint32_t last) {
  assert(first <= 0x7fffffff);
  assert(last <= 0x7fffffff);
  sequence_numbers_.push_back(first | 0x80000000);
  sequence_numbers_.push_back(last);
}

bool NegativeAckPacket::IsValid(const asio::const_buffer &buffer) {
  return (IsValidBase(buffer, kPacketType) &&
          (asio::buffer_size(buffer) > kHeaderSize) &&
          ((asio::buffer_size(buffer) - kHeaderSize) % 4 == 0));
}

bool NegativeAckPacket::ContainsSequenceNumber(uint32_t n) const {
  assert(n <= 0x7fffffff);
  for (size_t i = 0; i < sequence_numbers_.size(); ++i) {
    if (((sequence_numbers_[i] & 0x80000000) != 0) &&
        (i + 1 < sequence_numbers_.size())) {
      // This is a range.
      uint32_t first = (sequence_numbers_[i] & 0x7fffffff);
      uint32_t last = (sequence_numbers_[i + 1] & 0x7fffffff);
      if (first <= last) {
        if ((first <= n) && (n <= last))
          return true;
      } else {
        // The range wraps around past the maximum sequence number.
        if (((first <= n) && (n <= 0x7fffffff)) || (n <= last))
          return true;
      }
    } else {
      // This is a single sequence number.
      if ((sequence_numbers_[i] & 0x7fffffff) == n)
        return true;
    }
  }
  return false;
}

bool NegativeAckPacket::HasSequenceNumbers() const {
  return !sequence_numbers_.empty();
}

bool NegativeAckPacket::Decode(const asio::const_buffer &buffer) {
  // Refuse to decode if the input buffer is not valid.
  if (!IsValid(buffer))
    return false;

  // Decode the common parts of the control packet.
  if (!DecodeBase(buffer, kPacketType))
    return false;

  const unsigned char *p = asio::buffer_cast<const unsigned char *>(buffer);
  size_t length = asio::buffer_size(buffer) - kHeaderSize;
  p += kHeaderSize;

  sequence_numbers_.clear();
  for (size_t i = 0; i < length; i += 4) {
    uint32_t value = 0;
    DecodeUint32(&value, p + i);
    sequence_numbers_.push_back(value);
  }

  return true;
}

size_t NegativeAckPacket::Encode(const asio::mutable_buffer &buffer) const {
  // Refuse to encode if the output buffer is not big enough.
  if (asio::buffer_size(buffer) < kHeaderSize + sequence_numbers_.size() * 4)
    return 0;

  // Encode the common parts of the control packet.
  if (EncodeBase(buffer) == 0)
    return 0;

  unsigned char *p = asio::buffer_cast<unsigned char *>(buffer);
  p += kHeaderSize;

  for (size_t i = 0; i < sequence_numbers_.size(); ++i)
    EncodeUint32(sequence_numbers_[i], p + i * 4);

  return kHeaderSize + sequence_numbers_.size() * 4;
}

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe
