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

#include "maidsafe/rudp/packets/control_packet.h"

#include <cassert>
#include <cstring>

namespace asio = boost::asio;

namespace maidsafe {

namespace rudp {

namespace detail {

ControlPacket::ControlPacket()
  : type_(0),
    additional_info_(0),
    time_stamp_(0),
    destination_socket_id_(0) {
}

ControlPacket::~ControlPacket() {
}

uint16_t ControlPacket::Type() const {
  return type_;
}

void ControlPacket::SetType(uint16_t n) {
  assert(n <= 0x7fff);
  type_ = n;
}

uint32_t ControlPacket::AdditionalInfo() const {
  return additional_info_;
}

void ControlPacket::SetAdditionalInfo(uint32_t n) {
  additional_info_ = n;
}

uint32_t ControlPacket::TimeStamp() const {
  return time_stamp_;
}

void ControlPacket::SetTimeStamp(uint32_t n) {
  time_stamp_ = n;
}

uint32_t ControlPacket::DestinationSocketId() const {
  return destination_socket_id_;
}

void ControlPacket::SetDestinationSocketId(uint32_t n) {
  destination_socket_id_ = n;
}

bool ControlPacket::IsValidBase(const asio::const_buffer &buffer,
                                uint16_t expected_packet_type) {
  const unsigned char *p = asio::buffer_cast<const unsigned char *>(buffer);
  return ((asio::buffer_size(buffer) >= kHeaderSize) &&
          ((p[0] & 0x80) != 0) &&
          ((p[0] & 0x7f) == ((expected_packet_type >> 8) & 0x7f)) &&
          (p[1] == (expected_packet_type & 0xff)));
}

bool ControlPacket::DecodeBase(const asio::const_buffer &buffer,
                                   uint16_t expected_packet_type) {
  // Refuse to decode if the input buffer is not valid.
  if (!IsValidBase(buffer, expected_packet_type))
    return false;

  const unsigned char *p = asio::buffer_cast<const unsigned char *>(buffer);
//  size_t length = asio::buffer_size(buffer);

  type_ = (p[0] & 0x7f);
  type_ = ((type_ << 8) | p[1]);
  DecodeUint32(&additional_info_, p + 4);
  DecodeUint32(&time_stamp_, p + 8);
  DecodeUint32(&destination_socket_id_, p + 12);

  return true;
}

size_t ControlPacket::EncodeBase(const asio::mutable_buffer &buffer) const {
  // Refuse to encode if the output buffer is not big enough.
  if (asio::buffer_size(buffer) < kHeaderSize)
    return 0;

  unsigned char *p = asio::buffer_cast<unsigned char *>(buffer);

  p[0] = ((type_ >> 8) & 0x7f);
  p[0] |= 0x80;
  p[1] = (type_ & 0xff);
  p[2] = p[3] = 0;  // Reserved.
  EncodeUint32(additional_info_, p + 4);
  EncodeUint32(time_stamp_, p + 8);
  EncodeUint32(destination_socket_id_, p + 12);

  return kHeaderSize;
}

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe
