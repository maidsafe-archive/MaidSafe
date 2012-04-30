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

#ifndef MAIDSAFE_RUDP_PARAMETERS_H_
#define MAIDSAFE_RUDP_PARAMETERS_H_

#include <cstdint>
#include "boost/date_time/posix_time/posix_time_duration.hpp"
#include "maidsafe/rudp/version.h"

#if MAIDSAFE_RUDP_VERSION != 100
#  error This API is not compatible with the installed library.\
    Please update the maidsafe_rudp library.
#endif

namespace maidsafe {

namespace rudp {

typedef boost::posix_time::time_duration Timeout;

// This class provides the configurability to all traffic related parameters.
struct Parameters {
 public:
  // Thread count for use of asio::io_service
  static uint32_t thread_count;

  // Window size permitted in RUDP
  static uint32_t default_window_size;
  static uint32_t maximum_window_size;

  // Packet size permitted in RUDP
  // Shall not exceed the UDP payload, which is 65507
  static uint32_t default_size;
  static uint32_t max_size;
  enum { kUDPPayload = 65500 };

  // Data Payload size permitted in RUDP
  // Shall not exceed Packet Size defined
  static uint32_t default_data_size;
  static uint32_t max_data_size;

  // Timeout defined for a packet to be resent
  static Timeout default_send_timeout;

  // Timeout defined for a neg-ack packet to be resent to request resent of an
  // observed missing packet in receiver
  static Timeout default_receive_timeout;

  // Machine dependent parameter of send delay,
  // depending on computation power and I/O speed
  static Timeout default_send_delay;

  // Machine dependent parameter of receive delay,
  // depending on computation power and I/O speed
  static Timeout default_receive_delay;

  // Timeout defined for a Ack packet to be resent
  static Timeout default_ack_timeout;

  // Timeout defined for the fixed interval between Ack packets
  static Timeout ack_interval;

  // Interval to calculate speed
  static Timeout speed_calculate_inverval;

  // Slow speed threshold to force the socket closed, in b/s
  static uint32_t slow_speed_threshold;

  // Timeout during connection establishment
  static Timeout connect_timeout;

  // Timeout defined for the fixed interval between sending Keepalive packets
  static Timeout keepalive_interval;

  // Timeout defined to receive Keepalive response packet.
  static Timeout keepalive_timeout;

  // Defined connection types
  enum ConnectionType {
    kWireless = 0x0fffffff,
    kT1 = 0xf0ffffff,
    kE1 = 0xf1ffffff,
    k10MEthernet = 0xff0fffff,
    k100MEthernet = 0xff1fffff,
    k1GEthernet = 0xff2fffff
  };
  static ConnectionType connection_type;

 private:
  // Disallow copying and assignment.
  Parameters(const Parameters&);
  Parameters &operator=(const Parameters&);
};

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_PARAMETERS_H_
