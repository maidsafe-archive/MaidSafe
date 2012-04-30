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

#ifndef MAIDSAFE_RUDP_CONNECTION_H_
#define MAIDSAFE_RUDP_CONNECTION_H_

#include <memory>
#include <string>
#include <vector>
#include "boost/asio/deadline_timer.hpp"
#include "boost/asio/io_service.hpp"
#include "boost/asio/ip/udp.hpp"
#include "boost/asio/strand.hpp"
#include "maidsafe/rudp/core/socket.h"
#include "maidsafe/rudp/transport.h"

namespace maidsafe {

namespace rudp {

namespace detail {
class Multiplexer;
class Socket;
}  // namespace detail

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Weffc++"
#endif
class Connection : public std::enable_shared_from_this<Connection> {
#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif

 public:
  Connection(const std::shared_ptr<Transport> &transport,
             const boost::asio::io_service::strand &strand,
             const std::shared_ptr<detail::Multiplexer> &multiplexer,
             const boost::asio::ip::udp::endpoint &remote);
  ~Connection();

  detail::Socket &Socket();

  void Close();
  void StartReceiving();
  void StartSending(const std::string &data);
  void StartProbing();

 private:
  Connection(const Connection&);
  Connection &operator=(const Connection&);

  void DoClose();
  void DoStartReceiving();

  void CheckTimeout(const boost::system::error_code &ec);
  bool Stopped() const;

  void StartTick();
  void HandleTick();

  void StartConnect();
  void HandleConnect(const boost::system::error_code &ec);

  void StartReadSize();
  void HandleReadSize(const boost::system::error_code &ec);

  void StartReadData();
  void HandleReadData(const boost::system::error_code &ec, size_t length);

  void StartWrite();
  void HandleWrite(const boost::system::error_code &ec);

  void DoProbe(const boost::system::error_code &ec);
  void HandleProbe(const boost::system::error_code &ec);

  void DispatchMessage();
  void EncodeData(const std::string &data);

  std::weak_ptr<Transport> transport_;
  boost::asio::io_service::strand strand_;
  std::shared_ptr<detail::Multiplexer> multiplexer_;
  detail::Socket socket_;
  boost::asio::deadline_timer timer_;
  boost::asio::deadline_timer probe_interval_timer_;
  boost::posix_time::ptime response_deadline_;
  boost::asio::ip::udp::endpoint remote_endpoint_;
  std::vector<unsigned char> send_buffer_, receive_buffer_;
  size_t data_size_, data_received_;
  uint8_t probe_retry_attempts_;
  Timeout timeout_for_response_;
  enum TimeoutState { kNoTimeout, kConnecting, kSending } timeout_state_;
};

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_CONNECTION_H_
