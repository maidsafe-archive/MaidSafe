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

#ifndef MAIDSAFE_RUDP_CORE_DISPATCHER_H_
#define MAIDSAFE_RUDP_CORE_DISPATCHER_H_

#include <cstdint>

#include <unordered_map>
#include "boost/asio/buffer.hpp"
#include "boost/asio/ip/udp.hpp"

namespace maidsafe {

namespace rudp {

namespace detail {

class Socket;

class Dispatcher {
 public:
  Dispatcher();

  // Add a socket. Returns a new unique id for the socket.
  uint32_t AddSocket(Socket *socket);

  // Remove the socket corresponding to the given id.
  void RemoveSocket(uint32_t id);

  // Handle a new packet by dispatching to the appropriate socket.
  void HandleReceiveFrom(const boost::asio::const_buffer &data,
                         const boost::asio::ip::udp::endpoint &endpoint);

  boost::asio::ip::udp::endpoint GetAndClearBootstrappingEndpoint();

 private:
  // Disallow copying and assignment.
  Dispatcher(const Dispatcher&);
  Dispatcher &operator=(const Dispatcher&);

  // Map of destination socket id to corresponding socket object.
  typedef std::unordered_map<uint32_t, Socket*> SocketMap;
  SocketMap sockets_;

  // Endpoint of peer attempting to bootstrap off this node.
  boost::asio::ip::udp::endpoint bootstrapping_endpoint_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_CORE_DISPATCHER_H_
