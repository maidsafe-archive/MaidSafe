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

#include "maidsafe/rudp/parameters.h"

namespace bptime = boost::posix_time;

namespace maidsafe {

namespace rudp {

uint32_t Parameters::thread_count(8);
uint32_t Parameters::default_window_size(16);
uint32_t Parameters::maximum_window_size(512);
uint32_t Parameters::default_size(1480);
uint32_t Parameters::max_size(25980);
uint32_t Parameters::default_data_size(1450);
uint32_t Parameters::max_data_size(25950);
Timeout Parameters::default_send_timeout(bptime::milliseconds(1000));
Timeout Parameters::default_receive_timeout(bptime::milliseconds(200));
Timeout Parameters::default_send_delay(bptime::milliseconds(1000));
Timeout Parameters::default_receive_delay(bptime::milliseconds(100));
Timeout Parameters::default_ack_timeout(bptime::milliseconds(1000));
Timeout Parameters::ack_interval(bptime::milliseconds(100));
Timeout Parameters::speed_calculate_inverval(bptime::milliseconds(1000));
uint32_t Parameters::slow_speed_threshold(1024);
Timeout Parameters::connect_timeout(bptime::milliseconds(5000));
Timeout Parameters::keepalive_interval(bptime::milliseconds(10000));
Timeout Parameters::keepalive_timeout(bptime::milliseconds(1000));
Parameters::ConnectionType Parameters::connection_type(Parameters::kWireless);

}  // namespace rudp

}  // namespace maidsafe

