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

#ifndef MAIDSAFE_RUDP_PACKETS_ACK_PACKET_H_
#define MAIDSAFE_RUDP_PACKETS_ACK_PACKET_H_

#include <cstdint>
#include <string>

#include "boost/asio/buffer.hpp"
#include "boost/asio/ip/address.hpp"
#include "boost/system/error_code.hpp"

#include "maidsafe/rudp/packets/control_packet.h"

namespace maidsafe {

namespace rudp {

namespace detail {

class AckPacket : public ControlPacket {
 public:
  enum { kPacketSize = ControlPacket::kHeaderSize + 4 };
  enum { kOptionalPacketSize = ControlPacket::kHeaderSize + 24 };
  enum { kPacketType = 2 };

  AckPacket();
  virtual ~AckPacket() {}

  uint32_t AckSequenceNumber() const;
  void SetAckSequenceNumber(uint32_t n);

  uint32_t PacketSequenceNumber() const;
  void SetPacketSequenceNumber(uint32_t n);

  bool HasOptionalFields() const;
  void SetHasOptionalFields(bool b);

  // The following fields are optional in the encoded packet.

  uint32_t RoundTripTime() const;
  void SetRoundTripTime(uint32_t n);

  uint32_t RoundTripTimeVariance() const;
  void SetRoundTripTimeVariance(uint32_t n);

  uint32_t AvailableBufferSize() const;
  void SetAvailableBufferSize(uint32_t n);

  uint32_t PacketsReceivingRate() const;
  void SetPacketsReceivingRate(uint32_t n);

  uint32_t EstimatedLinkCapacity() const;
  void SetEstimatedLinkCapacity(uint32_t n);

  // End of optional fields.

  static bool IsValid(const boost::asio::const_buffer &buffer);
  bool Decode(const boost::asio::const_buffer &buffer);
  size_t Encode(const boost::asio::mutable_buffer &buffer) const;

 private:
  uint32_t packet_sequence_number_;
  bool has_optional_fields_;
  uint32_t round_trip_time_;
  uint32_t round_trip_time_variance_;
  uint32_t available_buffer_size_;
  uint32_t packets_receiving_rate_;
  uint32_t estimated_link_capacity_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_PACKETS_ACK_PACKET_H_
