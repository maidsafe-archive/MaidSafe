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

#ifndef MAIDSAFE_RUDP_CORE_SENDER_H_
#define MAIDSAFE_RUDP_CORE_SENDER_H_

#include <cstdint>

#include "boost/asio/buffer.hpp"
#include "boost/asio/ip/udp.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

#include "maidsafe/rudp/packets/ack_packet.h"
#include "maidsafe/rudp/packets/data_packet.h"
#include "maidsafe/rudp/packets/keepalive_packet.h"
#include "maidsafe/rudp/packets/negative_ack_packet.h"
#include "maidsafe/rudp/packets/shutdown_packet.h"
#include "maidsafe/rudp/core/sliding_window.h"
#include "maidsafe/rudp/return_codes.h"

namespace maidsafe {

namespace rudp {

namespace detail {

class CongestionControl;
class Peer;
class TickTimer;

class Sender {
 public:
  explicit Sender(Peer &peer,                                 // NOLINT (Fraser)
                  TickTimer &tick_timer,
                  CongestionControl &congestion_control);

  // Get the sequence number that will be used for the next packet.
  uint32_t GetNextPacketSequenceNumber() const;

  // Determine whether all data has been transmitted to the peer.
  bool Flushed() const;

  // Adds some application data to be sent. Returns number of bytes copied.
  size_t AddData(const boost::asio::const_buffer &data);

  // Notify the other side that the current connection is to be dropped
  void NotifyClose();

  // Handle an acknowlegement packet.
  void HandleAck(const AckPacket &packet);

  // Handle an negative acknowlegement packet.
  void HandleNegativeAck(const NegativeAckPacket &packet);

  // Handle a tick in the system time.
  void HandleTick();

  // Handle a keepalive packet.
  void HandleKeepalive(const KeepalivePacket &packet);

  // Send a keepalive packet to the other side.
  ReturnCode SendKeepalive(const KeepalivePacket &keepalive_packet);

 private:
  // Disallow copying and assignment.
  Sender(const Sender&);
  Sender &operator=(const Sender&);

  // Send waiting packets.
  void DoSend();

  // The peer with which we are communicating.
  Peer &peer_;

  // The timer used to generate tick events.
  TickTimer &tick_timer_;

  // The congestion control information associated with the connection.
  CongestionControl &congestion_control_;

  struct UnackedPacket {
    UnackedPacket() : packet(),
                      lost(false),
                      last_send_time() {}
    DataPacket packet;
    bool lost;
    boost::posix_time::ptime last_send_time;
  };

  // The sender's window of unacknowledged packets.
  typedef SlidingWindow<UnackedPacket> UnackedPacketWindow;
  UnackedPacketWindow unacked_packets_;

  // The next time at which all unacked packets will be considered lost.
  boost::posix_time::ptime send_timeout_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_CORE_SENDER_H_
