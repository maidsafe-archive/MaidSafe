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

#ifndef MAIDSAFE_RUDP_OPERATIONS_TICK_OP_H_
#define MAIDSAFE_RUDP_OPERATIONS_TICK_OP_H_

#include "boost/asio/error.hpp"
#include "boost/asio/handler_alloc_hook.hpp"
#include "boost/asio/handler_invoke_hook.hpp"
#include "boost/system/error_code.hpp"
#include "maidsafe/rudp/core/receiver.h"
#include "maidsafe/rudp/core/sender.h"
#include "maidsafe/rudp/core/session.h"
#include "maidsafe/rudp/core/tick_timer.h"

namespace maidsafe {

namespace rudp {

namespace detail {

// Helper class to perform an asynchronous tick operation.
template <typename TickHandler, typename Socket>
class TickOp {
 public:
  TickOp(TickHandler handler, Socket *socket, TickTimer *tick_timer)
    : handler_(handler),
      socket_(socket),
      tick_timer_(tick_timer) {
  }

  TickOp(const TickOp &L)
    : handler_(L.handler_),
      socket_(L.socket_),
      tick_timer_(L.tick_timer_) {
  }

  TickOp & operator=(const TickOp &L) {
    // check for "self assignment" and do nothing in that case
    if (this != &L) {
      delete socket_;
      handler_ = L.handler_;
      socket_ = L.socket_;
      tick_timer_ = L.tick_timer_;
    }
    return *this;
  }

  void operator()(boost::system::error_code) {
    boost::system::error_code ec;
    if (socket_->IsOpen()) {
      if (tick_timer_->Expired()) {
        tick_timer_->Reset();
        DispatchTick(socket_);
      }
    } else {
      ec = boost::asio::error::operation_aborted;
    }
    handler_(ec);
  }

  friend void *asio_handler_allocate(size_t n, TickOp *op) {
    using boost::asio::asio_handler_allocate;
    return asio_handler_allocate(n, &op->handler_);
  }

  friend void asio_handler_deallocate(void *p, size_t n, TickOp *op) {
    using boost::asio::asio_handler_deallocate;
    asio_handler_deallocate(p, n, &op->handler_);
  }

  template <typename Function>
  friend void asio_handler_invoke(const Function &f, TickOp *op) {
    using boost::asio::asio_handler_invoke;
    asio_handler_invoke(f, &op->handler_);
  }

 private:
  TickHandler handler_;
  Socket *socket_;
  TickTimer *tick_timer_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_OPERATIONS_TICK_OP_H_
