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

#ifndef MAIDSAFE_RUDP_PACKETS_DATA_PACKET_H_
#define MAIDSAFE_RUDP_PACKETS_DATA_PACKET_H_

#include <cstdint>
#include <string>

#include "boost/asio/buffer.hpp"
#include "boost/system/error_code.hpp"

#include "maidsafe/rudp/packets/packet.h"

namespace maidsafe {

namespace rudp {

namespace detail {

class DataPacket : public Packet {
 public:
  enum { kHeaderSize = 16 };

  DataPacket();

  uint32_t PacketSequenceNumber() const;
  void SetPacketSequenceNumber(uint32_t n);

  bool FirstPacketInMessage() const;
  void SetFirstPacketInMessage(bool b);

  bool LastPacketInMessage() const;
  void SetLastPacketInMessage(bool b);

  bool InOrder() const;
  void SetInOrder(bool b);

  uint32_t MessageNumber() const;
  void SetMessageNumber(uint32_t n);

  uint32_t TimeStamp() const;
  void SetTimeStamp(uint32_t n);

  uint32_t DestinationSocketId() const;
  void SetDestinationSocketId(uint32_t n);

  const std::string &Data() const;
  void SetData(const std::string &data);

  template <typename Iterator>
  void SetData(Iterator begin, Iterator end) {
    data_.assign(begin, end);
  }

  static bool IsValid(const boost::asio::const_buffer &buffer);
  bool Decode(const boost::asio::const_buffer &buffer);
  size_t Encode(const boost::asio::mutable_buffer &buffer) const;

 private:
  uint32_t packet_sequence_number_;
  bool first_packet_in_message_;
  bool last_packet_in_message_;
  bool in_order_;
  uint32_t message_number_;
  uint32_t time_stamp_;
  uint32_t destination_socket_id_;
  std::string data_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_PACKETS_DATA_PACKET_H_
