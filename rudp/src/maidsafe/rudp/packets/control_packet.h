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

#ifndef MAIDSAFE_RUDP_PACKETS_CONTROL_PACKET_H_
#define MAIDSAFE_RUDP_PACKETS_CONTROL_PACKET_H_

#include <cstdint>

#include "boost/asio/buffer.hpp"
#include "boost/system/error_code.hpp"

#include "maidsafe/rudp/packets/packet.h"

namespace maidsafe {

namespace rudp {

namespace detail {

namespace test {
class ControlPacketTest;
}  // namespace test

class ControlPacket : public Packet {
 public:
  enum { kHeaderSize = 16 };

  ControlPacket();

  uint16_t Type() const;

  uint32_t TimeStamp() const;
  void SetTimeStamp(uint32_t n);

  uint32_t DestinationSocketId() const;
  void SetDestinationSocketId(uint32_t n);

  friend class test::ControlPacketTest;
 protected:
  void SetType(uint16_t n);

  uint32_t AdditionalInfo() const;
  void SetAdditionalInfo(uint32_t n);

  static bool IsValidBase(const boost::asio::const_buffer &buffer,
                          uint16_t expected_packet_type);
  bool DecodeBase(const boost::asio::const_buffer &buffer,
                  uint16_t expected_packet_type);
  size_t EncodeBase(const boost::asio::mutable_buffer &buffer) const;

  // Prevent deletion through this type.
  virtual ~ControlPacket();

 private:
  uint16_t type_;
  uint32_t additional_info_;
  uint32_t time_stamp_;
  uint32_t destination_socket_id_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_PACKETS_CONTROL_PACKET_H_
