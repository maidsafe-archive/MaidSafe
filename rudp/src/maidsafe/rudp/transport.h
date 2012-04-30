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

#ifndef MAIDSAFE_RUDP_TRANSPORT_H_
#define MAIDSAFE_RUDP_TRANSPORT_H_

#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "boost/asio/strand.hpp"
#include "boost/signals2/signal.hpp"
#include "boost/thread/mutex.hpp"

#include "maidsafe/common/asio_service.h"

#include "maidsafe/rudp/common.h"
#include "maidsafe/rudp/parameters.h"


namespace maidsafe {

namespace rudp {

class ManagedConnections;
class Connection;

namespace detail {
class Multiplexer;
class Socket;
}  // namespace detail


#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Weffc++"
#endif
class Transport : public std::enable_shared_from_this<Transport> {
#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif

 public:
  typedef boost::signals2::signal<void(const std::string&)> OnMessage;
  typedef boost::signals2::signal<void(const Endpoint&,
                                       std::shared_ptr<Transport>)>
                                          OnConnectionAdded, OnConnectionLost;

  explicit Transport(std::shared_ptr<AsioService> asio_service);  // NOLINT (Fraser)
  virtual ~Transport();

  void Bootstrap(const std::vector<Endpoint> &bootstrap_endpoints,
                 Endpoint local_endpoint,
                 const OnMessage::slot_type &on_message_slot,
                 const OnConnectionAdded::slot_type &on_connection_added_slot,
                 const OnConnectionLost::slot_type &on_connection_lost_slot,
                 Endpoint *chosen_endpoint,
                 boost::signals2::connection *on_message_connection,
                 boost::signals2::connection *on_connection_added_connection,
                 boost::signals2::connection *on_connection_lost_connection);
  void Connect(const Endpoint &peer_endpoint,
               const std::string &validation_data);
  // Returns kSuccess if the connection existed and was closed.  Returns
  // kInvalidConnection if the connection didn't exist.  If this causes the
  // size of connected_endpoints_ to drop to 0, this transport will remove
  // itself from ManagedConnections which will cause it to be destroyed.
  int CloseConnection(const Endpoint &peer_endpoint);
  int Send(const Endpoint &peer_endpoint, const std::string &message);
  Endpoint external_endpoint() const; 
  Endpoint local_endpoint() const; 
  size_t ConnectionsCount() const;
  static uint32_t kMaxConnections() { return 50; }

 private:
  Transport(const Transport&);
  Transport &operator=(const Transport&);

  typedef std::shared_ptr<ManagedConnections> ManagedConnectionsPtr;
  typedef std::shared_ptr<detail::Multiplexer> MultiplexerPtr;
  typedef std::shared_ptr<Connection> ConnectionPtr;
  typedef std::set<ConnectionPtr> ConnectionSet;

  void DoConnect(const Endpoint &peer_endpoint,
                 const std::string &validation_data);
  void DoCloseConnection(ConnectionPtr connection);
  void DoSend(ConnectionPtr connection, const std::string &message);

  static void CloseMultiplexer(MultiplexerPtr multiplexer);

  void StartDispatch();
  void HandleDispatch(MultiplexerPtr multiplexer,
                      const boost::system::error_code &ec);

  friend class Connection;
  void SignalMessageReceived(const std::string &message);
  void DoSignalMessageReceived(const std::string &message);
  void InsertConnection(ConnectionPtr connection);
  void DoInsertConnection(ConnectionPtr connection);
  void RemoveConnection(ConnectionPtr connection);
  void DoRemoveConnection(ConnectionPtr connection);

  std::shared_ptr<AsioService> asio_service_;
  boost::asio::io_service::strand strand_;
  MultiplexerPtr multiplexer_;

  // Because the connections can be in an idle initial state with no pending
  // async operations (after calling PrepareSend()), they are kept alive with
  // a shared_ptr in this map, as well as in the async operation handlers.
  ConnectionSet connections_;
  mutable boost::mutex mutex_;
  OnMessage on_message_;
  OnConnectionAdded on_connection_added_;
  OnConnectionLost on_connection_lost_;
};

typedef std::shared_ptr<Transport> TransportPtr;

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_TRANSPORT_H_
