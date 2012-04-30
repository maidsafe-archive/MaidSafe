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

#include "maidsafe/rudp/packets/ack_packet.h"

#include <cassert>
#include <cstring>

namespace asio = boost::asio;

namespace maidsafe {

namespace rudp {

namespace detail {

AckPacket::AckPacket()
  : packet_sequence_number_(0),
    has_optional_fields_(false),
    round_trip_time_(0),
    round_trip_time_variance_(0),
    available_buffer_size_(0),
    packets_receiving_rate_(0),
    estimated_link_capacity_(0) {
  SetType(kPacketType);
}

uint32_t AckPacket::AckSequenceNumber() const {
  return AdditionalInfo();
}

void AckPacket::SetAckSequenceNumber(uint32_t n) {
  SetAdditionalInfo(n);
}

uint32_t AckPacket::PacketSequenceNumber() const {
  return packet_sequence_number_;
}

void AckPacket::SetPacketSequenceNumber(uint32_t n) {
  packet_sequence_number_ = n;
}

bool AckPacket::HasOptionalFields() const {
  return has_optional_fields_;
}

void AckPacket::SetHasOptionalFields(bool b) {
  has_optional_fields_ = b;
}

uint32_t AckPacket::RoundTripTime() const {
  return round_trip_time_;
}

void AckPacket::SetRoundTripTime(uint32_t n) {
  round_trip_time_ = n;
}

uint32_t AckPacket::RoundTripTimeVariance() const {
  return round_trip_time_variance_;
}

void AckPacket::SetRoundTripTimeVariance(uint32_t n) {
  round_trip_time_variance_ = n;
}

uint32_t AckPacket::AvailableBufferSize() const {
  return available_buffer_size_;
}

void AckPacket::SetAvailableBufferSize(uint32_t n) {
  available_buffer_size_ = n;
}

uint32_t AckPacket::PacketsReceivingRate() const {
  return packets_receiving_rate_;
}

void AckPacket::SetPacketsReceivingRate(uint32_t n) {
  packets_receiving_rate_ = n;
}

uint32_t AckPacket::EstimatedLinkCapacity() const {
  return estimated_link_capacity_;
}

void AckPacket::SetEstimatedLinkCapacity(uint32_t n) {
  estimated_link_capacity_ = n;
}

bool AckPacket::IsValid(const asio::const_buffer &buffer) {
  return (IsValidBase(buffer, kPacketType) &&
          ((asio::buffer_size(buffer) == kPacketSize) ||
           (asio::buffer_size(buffer) == kOptionalPacketSize)));
}

bool AckPacket::Decode(const asio::const_buffer &buffer) {
  // Refuse to decode if the input buffer is not valid.
  if (!IsValid(buffer))
    return false;

  // Decode the common parts of the control packet.
  if (!DecodeBase(buffer, kPacketType))
    return false;

  const unsigned char *p = asio::buffer_cast<const unsigned char *>(buffer);
//   size_t length = asio::buffer_size(buffer) - kHeaderSize;
  p += kHeaderSize;

  DecodeUint32(&packet_sequence_number_, p + 0);
  if (asio::buffer_size(buffer) == kOptionalPacketSize) {
    has_optional_fields_ = true;
    DecodeUint32(&round_trip_time_, p + 4);
    DecodeUint32(&round_trip_time_variance_, p + 8);
    DecodeUint32(&available_buffer_size_, p + 12);
    DecodeUint32(&packets_receiving_rate_, p + 16);
    DecodeUint32(&estimated_link_capacity_, p + 20);
  }

  return true;
}

size_t AckPacket::Encode(const asio::mutable_buffer &buffer) const {
  // Refuse to encode if the output buffer is not big enough.
  if (asio::buffer_size(buffer) < kPacketSize)
    return 0;

  // Encode the common parts of the control packet.
  if (EncodeBase(buffer) == 0)
    return 0;

  unsigned char *p = asio::buffer_cast<unsigned char *>(buffer);
  p += kHeaderSize;

  EncodeUint32(packet_sequence_number_, p + 0);
  if (has_optional_fields_) {
    EncodeUint32(round_trip_time_, p + 4);
    EncodeUint32(round_trip_time_variance_, p + 8);
    EncodeUint32(available_buffer_size_, p + 12);
    EncodeUint32(packets_receiving_rate_, p + 16);
    EncodeUint32(estimated_link_capacity_, p + 20);
  }

  return has_optional_fields_ ? static_cast<size_t>(kOptionalPacketSize)
      : static_cast<size_t>(kPacketSize);
}

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe
