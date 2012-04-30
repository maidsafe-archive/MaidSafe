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

#ifndef MAIDSAFE_RUDP_CORE_TICK_TIMER_H_
#define MAIDSAFE_RUDP_CORE_TICK_TIMER_H_

#include "boost/asio/deadline_timer.hpp"

namespace maidsafe {

namespace rudp {

namespace detail {

// Lightweight wrapper around a deadline_timer that avoids modifying the expiry
// time if it would move it further away.
class TickTimer {
 public:
  explicit TickTimer(boost::asio::io_service &asio_service)  // NOLINT (Fraser)
    : timer_(asio_service) {
    Reset();
  }

  static boost::posix_time::ptime Now() {
    return boost::asio::deadline_timer::traits_type::now();
  }

  void Cancel() {
    timer_.cancel();
  }

  void Reset() {
    timer_.expires_at(boost::posix_time::pos_infin);
  }

  bool Expired() const {
    // Infinate time out will be counted as expired
    if (timer_.expires_at() == boost::posix_time::pos_infin)
      return true;
    return Now() >= timer_.expires_at();
  }

  void TickAt(const boost::posix_time::ptime &time) {
    if (time < timer_.expires_at())
      timer_.expires_at(time);
  }

  void TickAfter(const boost::posix_time::time_duration &duration) {
    TickAt(Now() + duration);
  }

  template <typename WaitHandler>
  void AsyncWait(WaitHandler handler) {
    timer_.async_wait(handler);
  }

 private:
  boost::asio::deadline_timer timer_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_CORE_TICK_TIMER_H_
