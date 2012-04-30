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

#include "maidsafe/rudp/packets/data_packet.h"

#include <cassert>
#include <cstring>

namespace asio = boost::asio;

namespace maidsafe {

namespace rudp {

namespace detail {

DataPacket::DataPacket()
  : packet_sequence_number_(0),
    first_packet_in_message_(false),
    last_packet_in_message_(false),
    in_order_(false),
    message_number_(0),
    time_stamp_(0),
    destination_socket_id_(0),
    data_() {}

uint32_t DataPacket::PacketSequenceNumber() const {
  return packet_sequence_number_;
}

void DataPacket::SetPacketSequenceNumber(uint32_t n) {
  assert(n <= 0x7fffffff);
  packet_sequence_number_ = n;
}

bool DataPacket::FirstPacketInMessage() const {
  return first_packet_in_message_;
}

void DataPacket::SetFirstPacketInMessage(bool b) {
  first_packet_in_message_ = b;
}

bool DataPacket::LastPacketInMessage() const {
  return last_packet_in_message_;
}

void DataPacket::SetLastPacketInMessage(bool b) {
  last_packet_in_message_ = b;
}

bool DataPacket::InOrder() const {
  return in_order_;
}

void DataPacket::SetInOrder(bool b) {
  in_order_ = b;
}

uint32_t DataPacket::MessageNumber() const {
  return message_number_;
}

void DataPacket::SetMessageNumber(uint32_t n) {
  assert(n <= 0x1fffffff);
  message_number_ = n;
}

uint32_t DataPacket::TimeStamp() const {
  return time_stamp_;
}

void DataPacket::SetTimeStamp(uint32_t n) {
  time_stamp_ = n;
}

uint32_t DataPacket::DestinationSocketId() const {
  return destination_socket_id_;
}

void DataPacket::SetDestinationSocketId(uint32_t n) {
  destination_socket_id_ = n;
}

const std::string &DataPacket::Data() const {
  return data_;
}

void DataPacket::SetData(const std::string &data) {
  data_ = data;
}

bool DataPacket::IsValid(const asio::const_buffer &buffer) {
  return ((asio::buffer_size(buffer) >= 16) &&
          ((asio::buffer_cast<const unsigned char *>(buffer)[0] & 0x80) == 0));
}

bool DataPacket::Decode(const asio::const_buffer &buffer) {
  // Refuse to decode if the input buffer is not valid.
  if (!IsValid(buffer))
    return false;

  const unsigned char *p = asio::buffer_cast<const unsigned char *>(buffer);
  size_t length = asio::buffer_size(buffer);

  packet_sequence_number_ = (p[0] & 0x7f);
  packet_sequence_number_ = ((packet_sequence_number_ << 8) | p[1]);
  packet_sequence_number_ = ((packet_sequence_number_ << 8) | p[2]);
  packet_sequence_number_ = ((packet_sequence_number_ << 8) | p[3]);
  first_packet_in_message_ = ((p[4] & 0x80) != 0);
  last_packet_in_message_ = ((p[4] & 0x40) != 0);
  in_order_ = ((p[4] & 0x20) != 0);
  message_number_ = (p[4] & 0x1f);
  message_number_ = ((message_number_ << 8) | p[5]);
  message_number_ = ((message_number_ << 8) | p[6]);
  message_number_ = ((message_number_ << 8) | p[7]);
  DecodeUint32(&time_stamp_, p + 8);
  DecodeUint32(&destination_socket_id_, p + 12);
  data_.assign(p + 16, p + length);

  return true;
}

size_t DataPacket::Encode(const asio::mutable_buffer &buffer) const {
  // Refuse to encode if the output buffer is not big enough.
  if (asio::buffer_size(buffer) < kHeaderSize + data_.size())
    return 0;

  unsigned char *p = asio::buffer_cast<unsigned char *>(buffer);

  p[0] = ((packet_sequence_number_ >> 24) & 0x7f);
  p[1] = ((packet_sequence_number_ >> 16) & 0xff);
  p[2] = ((packet_sequence_number_ >> 8) & 0xff);
  p[3] = (packet_sequence_number_ & 0xff);
  p[4] = ((message_number_ >> 24) & 0x1f);
  p[4] |= (first_packet_in_message_ ? 0x80 : 0);
  p[4] |= (last_packet_in_message_ ? 0x40 : 0);
  p[4] |= (in_order_ ? 0x20 : 0);
  p[5] = ((message_number_ >> 16) & 0xff);
  p[6] = ((message_number_ >> 8) & 0xff);
  p[7] = (message_number_ & 0xff);
  EncodeUint32(time_stamp_, p + 8);
  EncodeUint32(destination_socket_id_, p + 12);
  std::memcpy(p + kHeaderSize, data_.data(), data_.size());

  return kHeaderSize + data_.size();
}

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe
