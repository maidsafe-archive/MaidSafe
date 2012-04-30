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

#include "maidsafe/rudp/core/receiver.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>

#include "boost/assert.hpp"

#include "maidsafe/common/utils.h"

#include "maidsafe/rudp/core/congestion_control.h"
#include "maidsafe/rudp/packets/negative_ack_packet.h"
#include "maidsafe/rudp/core/peer.h"
#include "maidsafe/rudp/core/tick_timer.h"
#include "maidsafe/rudp/log.h"

namespace asio = boost::asio;
namespace ip = asio::ip;
namespace bptime = boost::posix_time;

namespace maidsafe {

namespace rudp {

namespace detail {

Receiver::Receiver(Peer &peer,                                // NOLINT (Fraser)
                   TickTimer &tick_timer,
                   CongestionControl &congestion_control)
  : peer_(peer),
    tick_timer_(tick_timer),
    congestion_control_(congestion_control),
    unread_packets_(),
    acks_(),
    last_ack_packet_sequence_number_(0),
    ack_sent_time_(tick_timer_.Now()) {
}

void Receiver::Reset(uint32_t initial_sequence_number) {
  unread_packets_.Reset(initial_sequence_number);
  last_ack_packet_sequence_number_ = initial_sequence_number;
}

bool Receiver::Flushed() const {
  uint32_t ack_packet_seqnum = AckPacketSequenceNumber();
  return acks_.IsEmpty() &&
         (ack_packet_seqnum == last_ack_packet_sequence_number_);
}

size_t Receiver::ReadData(const boost::asio::mutable_buffer &data) {
  unsigned char *begin = asio::buffer_cast<unsigned char*>(data);
  unsigned char *ptr = begin;
  unsigned char *end = begin + asio::buffer_size(data);

  for (boost::uint32_t n = unread_packets_.Begin();
       (n != unread_packets_.End()) && (ptr < end);
       n = unread_packets_.Next(n)) {
    UnreadPacket &p = unread_packets_[n];
//        DLOG(ERROR) << "Unread packet: " << p.packet.PacketSequenceNumber() << " size = "
//                    << p.packet.Data().size();
    if (p.lost) {
      break;
    } else if (p.packet.Data().size() > p.bytes_read) {
      size_t length = std::min<size_t>(end - ptr,
                                       p.packet.Data().size() - p.bytes_read);
      std::memcpy(ptr, p.packet.Data().data() + p.bytes_read, length);
      ptr += length;
      p.bytes_read += length;
      if (p.packet.Data().size() == p.bytes_read) {
//                  DLOG(ERROR) << "Removing packet: " << p.packet.PacketSequenceNumber();
        unread_packets_.Remove();
      } else {
//                  DLOG(ERROR) << "NOT Removing packet: " << p.packet.PacketSequenceNumber() << " - length = " << length;
      }
    } else {
//                      DLOG(ERROR) << "Removing packet: " << p.packet.PacketSequenceNumber();
      unread_packets_.Remove();
    }
  }

  return ptr - begin;
}

void Receiver::HandleData(const DataPacket &packet) {
  unread_packets_.SetMaximumSize(congestion_control_.ReceiveWindowSize());

  uint32_t seqnum = packet.PacketSequenceNumber();

  // Make sure there is space in the window for packets that are expected soon.
  // sliding_window will keep appending till reach the current seqnum or full.
  // i.e. any un-received packet, having previous seqnum, will be given an empty
  // reserved slot.
  // Later arrvied packet, having less seqnum, will not affect sliding window
  while (unread_packets_.IsComingSoon(seqnum) && !unread_packets_.IsFull())
    // New entries are marked "lost" by default, and reserve_time set to now
    unread_packets_.Append();

  // Ignore any packet which isn't in the window.
  // The empty slot will got populated here, if the packet arrived later having
  // a seqnum falls in the window
  if (unread_packets_.Contains(seqnum)) {
    UnreadPacket &p = unread_packets_[seqnum];
    // The packet will be ignored if already received
    if (p.lost) {
      congestion_control_.OnDataPacketReceived(seqnum);
      p.packet = packet;
      p.lost = false;
      p.bytes_read = 0;
    }
  } else {
    DLOG(WARNING) << "Ignoring incoming packet with seqnum " << seqnum << " and data " << packet.Data();
  }

  if (seqnum % congestion_control_.AckInterval() == 0) {
    // Send acknowledgement packets immediately.
    HandleTick();
  } else {
    // Schedule generation of acknowledgement packets for later.
    tick_timer_.TickAfter(congestion_control_.AckDelay());
  }
//  if (tick_timer_.Expired()) {
//    tick_timer_.TickAfter(congestion_control_.ReceiveDelay());
//  }
}

void Receiver::HandleAckOfAck(const AckOfAckPacket &packet) {
  uint32_t ack_seqnum = packet.AckSequenceNumber();

  if (acks_.Contains(ack_seqnum)) {
    Ack &a = acks_[ack_seqnum];
    boost::posix_time::time_duration rtt = tick_timer_.Now() - a.send_time;
    uint64_t rtt_us = rtt.total_microseconds();
    if (rtt_us < UINT32_MAX) {
      congestion_control_.OnAckOfAck(static_cast<uint32_t>(rtt_us));
    }
  }

  while (acks_.Contains(ack_seqnum)) {
    acks_.Remove();
  }
}

void Receiver::HandleTick() {
  bptime::ptime now = tick_timer_.Now();

  // Generate an acknowledgement only if the latest sequence number has
  // changed, or if it has been too long since the last unacknowledged
  // acknowledgement.
  uint32_t ack_packet_seqnum = AckPacketSequenceNumber();
  if ((ack_packet_seqnum != last_ack_packet_sequence_number_) ||
      (!acks_.IsEmpty() &&
       (acks_.Back().send_time + congestion_control_.AckTimeout() <= now))) {
    if (acks_.IsFull())
      acks_.Remove();
    congestion_control_.OnGenerateAck(ack_packet_seqnum);
    uint32_t n = acks_.Append();
    Ack& a = acks_[n];
    a.packet.SetDestinationSocketId(peer_.Id());
    a.packet.SetAckSequenceNumber(n);
    a.packet.SetPacketSequenceNumber(ack_packet_seqnum);
    a.packet.SetHasOptionalFields(true);
    a.packet.SetRoundTripTime(congestion_control_.RoundTripTime());
    a.packet.SetRoundTripTimeVariance(
        congestion_control_.RoundTripTimeVariance());
    a.packet.SetAvailableBufferSize(AvailableBufferSize());
    a.packet.SetPacketsReceivingRate(
        congestion_control_.PacketsReceivingRate());
    a.packet.SetEstimatedLinkCapacity(
        congestion_control_.EstimatedLinkCapacity());
    a.send_time = now;
    peer_.Send(a.packet);
    last_ack_packet_sequence_number_ = ack_packet_seqnum;
  }

  if (!acks_.IsEmpty()) {
    if (acks_.Back().send_time + congestion_control_.AckTimeout() > now) {
      tick_timer_.TickAt(acks_.Back().send_time +
                         congestion_control_.AckTimeout());
    }
  }

  // Generate a negative acknowledgement packet to request missing packets.
  NegativeAckPacket negative_ack;
  negative_ack.SetDestinationSocketId(peer_.Id());
  uint32_t n = unread_packets_.Begin();
  while (n != unread_packets_.End()) {
    if (unread_packets_[n].lost) {
      uint32_t begin = n;
      uint32_t end;
      do {
        end = n;
        n = unread_packets_.Next(n);
      } while (n != unread_packets_.End() && unread_packets_[n].lost);
      if (begin == end)
        negative_ack.AddSequenceNumber(begin);
      else
        negative_ack.AddSequenceNumbers(begin, end);
    } else {
      n = unread_packets_.Next(n);
    }
  }
  if (negative_ack.HasSequenceNumbers()) {
    peer_.Send(negative_ack);
    tick_timer_.TickAt(now + congestion_control_.AckTimeout());
  }
}

uint32_t Receiver::AvailableBufferSize() const {
  size_t free_packets = unread_packets_.IsFull() ?
                        0 : unread_packets_.MaximumSize() -
                            unread_packets_.Size();
  BOOST_ASSERT(free_packets * Parameters::max_data_size <
               std::numeric_limits<uint32_t>::max());
  return static_cast<uint32_t>(free_packets * Parameters::max_data_size);
}

uint32_t Receiver::AckPacketSequenceNumber() const {
  // Work out what sequence number we need to acknowledge up to.
  uint32_t ack_packet_seqnum = unread_packets_.Begin();
  while (ack_packet_seqnum != unread_packets_.End() &&
         !unread_packets_[ack_packet_seqnum].lost)
    ack_packet_seqnum = unread_packets_.Next(ack_packet_seqnum);
  return ack_packet_seqnum;
}

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe
