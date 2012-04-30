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

#ifndef MAIDSAFE_RUDP_CORE_RECEIVER_H_
#define MAIDSAFE_RUDP_CORE_RECEIVER_H_

#include <cstdint>
#include <deque>

#include "boost/asio/buffer.hpp"
#include "boost/asio/deadline_timer.hpp"
#include "boost/asio/ip/udp.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

#include "maidsafe/rudp/packets/ack_packet.h"
#include "maidsafe/rudp/packets/ack_of_ack_packet.h"
#include "maidsafe/rudp/packets/data_packet.h"
#include "maidsafe/rudp/core/sliding_window.h"


namespace bptime = boost::posix_time;

namespace maidsafe {

namespace rudp {

namespace detail {

class CongestionControl;
class Peer;
class TickTimer;

class Receiver {
 public:
  explicit Receiver(Peer &peer,                               // NOLINT (Fraser)
                    TickTimer &tick_timer,
                    CongestionControl &congestion_control);

  // Reset receiver so that it is ready to start receiving data from the
  // specified sequence number.
  void Reset(uint32_t initial_sequence_number);

  // Determine whether all acknowledgements have been processed.
  bool Flushed() const;

  // Reads some application data. Returns number of bytes copied.
  size_t ReadData(const boost::asio::mutable_buffer &data);

  // Handle a data packet.
  void HandleData(const DataPacket &packet);

  // Handle an acknowledgement of an acknowledgement packet.
  void HandleAckOfAck(const AckOfAckPacket &packet);

  // Handle a tick in the system time.
  void HandleTick();

 private:
  // Disallow copying and assignment.
  Receiver(const Receiver&);
  Receiver &operator=(const Receiver&);

  // Helper function to calculate the available buffer size.
  uint32_t AvailableBufferSize() const;

  // Calculate the sequence number which should be sent in an acknowledgement.
  uint32_t AckPacketSequenceNumber() const;

  // The peer with which we are communicating.
  Peer &peer_;

  // The timer used to generate tick events.
  TickTimer &tick_timer_;

  // The congestion control information associated with the connection.
  CongestionControl &congestion_control_;

  struct UnreadPacket {
    UnreadPacket()
        : packet(),
          lost(true),
          bytes_read(0),
          reserve_time(boost::asio::deadline_timer::traits_type::now()) {}
    DataPacket packet;
    bool lost;
    size_t bytes_read;
    bptime::ptime reserve_time;

    bool Missing(bptime::time_duration time_out) {
      bptime::ptime now = boost::asio::deadline_timer::traits_type::now();
      return (lost && ((reserve_time + time_out) < now));
    }
  };

  // The receiver's window of unread packets. If this window fills up, any new
  // data packets are dropped. The application needs to read data regularly to
  // ensure that more data can be received.
  typedef SlidingWindow<UnreadPacket> UnreadPacketWindow;
  UnreadPacketWindow unread_packets_;

  struct Ack {
    Ack() : packet(),
            send_time(boost::asio::deadline_timer::traits_type::now()) {}
    AckPacket packet;
    boost::posix_time::ptime send_time;
  };

  // The receiver's window of acknowledgements. New acks are generated on a
  // regular basis, so if this window fills up the oldest entries are removed.
  typedef SlidingWindow<Ack> AckWindow;
  AckWindow acks_;

  // The last packet sequence number to have been acknowledged.
  uint32_t last_ack_packet_sequence_number_;

  // Next time the ack packet shall be sent
  boost::posix_time::ptime ack_sent_time_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_CORE_RECEIVER_H_
