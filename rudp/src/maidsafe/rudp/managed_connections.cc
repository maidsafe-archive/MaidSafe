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

#include "maidsafe/rudp/managed_connections.h"

#include <functional>
#include <iterator>

#include "maidsafe/rudp/common.h"
#include "maidsafe/rudp/log.h"
#include "maidsafe/rudp/return_codes.h"
#include "maidsafe/rudp/transport.h"
#include "maidsafe/rudp/utils.h"

namespace args = std::placeholders;
namespace bptime = boost::posix_time;

namespace maidsafe {

namespace rudp {

namespace {
const int kMaxTransports(10);
}  // unnamed namespace

ManagedConnections::ManagedConnections()
    : asio_service_(new AsioService),
      message_received_functor_(),
      connection_lost_functor_(),
      transports_(),
      connection_map_(),
      shared_mutex_() {}

ManagedConnections::~ManagedConnections() {
  UniqueLock unique_lock(shared_mutex_);
  std::for_each(transports_.begin(),
                transports_.end(),
                [](const TransportAndSignalConnections &element) {
    element.on_connection_lost_connection.disconnect();
    element.on_connection_added_connection.disconnect();
    element.on_message_connection.disconnect();
  });
}

Endpoint ManagedConnections::Bootstrap(
    const std::vector<Endpoint> &bootstrap_endpoints,
    MessageReceivedFunctor message_received_functor,
    ConnectionLostFunctor connection_lost_functor,
    boost::asio::ip::udp::endpoint local_endpoint) {
  if (!message_received_functor) {
    DLOG(ERROR) << "You must provide a valid MessageReceivedFunctor.";
    return Endpoint();
  }
  message_received_functor_ = message_received_functor;
  if (!connection_lost_functor) {
    DLOG(ERROR) << "You must provide a valid ConnectionLostFunctor.";
    return Endpoint();
  }
  connection_lost_functor_ = connection_lost_functor;

  {
    SharedLock shared_lock(shared_mutex_);
    if (!connection_map_.empty()) {
      DLOG(ERROR) << "Already bootstrapped.";
      return Endpoint();
    }
    BOOST_ASSERT(transports_.empty());
  }

  asio_service_->Start(Parameters::thread_count);

  Endpoint new_endpoint(StartNewTransport(bootstrap_endpoints, local_endpoint));
  if (!IsValid(new_endpoint)) {
    DLOG(ERROR) << "Failed to bootstrap managed connections.";
    return Endpoint();
  }

  return new_endpoint;
}

Endpoint ManagedConnections::StartNewTransport(
    std::vector<Endpoint> bootstrap_endpoints,
    Endpoint local_endpoint) {
  TransportAndSignalConnections transport_and_signals_connections;
  transport_and_signals_connections.transport =
      std::make_shared<Transport>(asio_service_);

  bool bootstrapping(!bootstrap_endpoints.empty());
  if (!bootstrapping) {
    bootstrap_endpoints.reserve(kMaxTransports * Transport::kMaxConnections());
    SharedLock shared_lock(shared_mutex_);
    std::for_each(
        connection_map_.begin(),
        connection_map_.end(),
        [&bootstrap_endpoints](const ConnectionMap::value_type &entry) {
      bootstrap_endpoints.push_back(entry.first);
    });
  }

  Endpoint chosen_endpoint;
  transport_and_signals_connections.transport->Bootstrap(
      bootstrap_endpoints,
      local_endpoint,
      std::bind(&ManagedConnections::OnMessageSlot, this, args::_1),
      std::bind(&ManagedConnections::OnConnectionAddedSlot, this, args::_1,
                args::_2),
      std::bind(&ManagedConnections::OnConnectionLostSlot, this, args::_1,
                args::_2),
      &chosen_endpoint,
      &transport_and_signals_connections.on_message_connection,
      &transport_and_signals_connections.on_connection_added_connection,
      &transport_and_signals_connections.on_connection_lost_connection);
  if (!IsValid(chosen_endpoint)) {
    SharedLock shared_lock(shared_mutex_);
    DLOG(WARNING) << "Failed to start a new Transport.  "
                  << connection_map_.size() << " currently running.";
    return Endpoint();
  }

  UniqueLock unique_lock(shared_mutex_);
  transports_.push_back(transport_and_signals_connections);
                                                                              //  if (bootstrapping) {
                                                                              //    connection_map_.insert(std::make_pair(chosen_endpoint,
                                                                              //                           transport_and_signals_connections.transport));
                                                                              //  }
  return chosen_endpoint;
}

int ManagedConnections::GetAvailableEndpoint(EndpointPair *endpoint_pair) {
  if (!endpoint_pair) {
    DLOG(ERROR) << "Null parameter passed.";
    return kNullParameter;
  }

  int transports_size(0);
  {
    SharedLock shared_lock(shared_mutex_);
    transports_size = static_cast<int>(transports_.size());
  }

  if (transports_size < kMaxTransports) {
    if (transports_size == 0) {
      DLOG(ERROR) << "No running Transports.";
      return kNoneAvailable;
    }

    Endpoint new_endpoint(StartNewTransport(std::vector<Endpoint>(),
                                            Endpoint()));
    if (IsValid(new_endpoint)) {
      UniqueLock unique_lock(shared_mutex_);
      endpoint_pair->external =
          (*transports_.rbegin()).transport->external_endpoint();
      endpoint_pair->local =
          (*transports_.rbegin()).transport->local_endpoint();
      return kSuccess;
    }
  }

  // Get transport with least connections.
  {
    size_t least_connections(Transport::kMaxConnections());
    EndpointPair chosen_endpoint_pair;
    SharedLock shared_lock(shared_mutex_);
    std::for_each(
        transports_.begin(),
        transports_.end(),
        [&least_connections, &chosen_endpoint_pair]
            (const TransportAndSignalConnections &element) {
      if (element.transport->ConnectionsCount() < least_connections) {
        least_connections = element.transport->ConnectionsCount();
        chosen_endpoint_pair.external = element.transport->external_endpoint();
        chosen_endpoint_pair.local = element.transport->local_endpoint();
      }
    });

    if (!IsValid(chosen_endpoint_pair.external) ||
        !IsValid(chosen_endpoint_pair.local)) {
      DLOG(ERROR) << "All Transports are full.";
      return kFull;
    }

    *endpoint_pair = chosen_endpoint_pair;
    return kSuccess;
  }
}

int ManagedConnections::Add(const Endpoint &this_endpoint,
                            const Endpoint &peer_endpoint,
                            const std::string &validation_data) {
  std::vector<TransportAndSignalConnections>::iterator itr;
  {
    SharedLock shared_lock(shared_mutex_);
    itr = std::find_if(
        transports_.begin(),
        transports_.end(),
        [&this_endpoint] (const TransportAndSignalConnections &element) {
      return element.transport->external_endpoint() == this_endpoint ||
             element.transport->local_endpoint() == this_endpoint;
    });
    if (itr == transports_.end()) {
      DLOG(ERROR) << "No Transports have endpoint " << this_endpoint
                  << " - ensure GetAvailableEndpoint has been called first.";
      return kInvalidTransport;
    }

    if (connection_map_.find(peer_endpoint) != connection_map_.end()) {
      DLOG(ERROR) << "A managed connection to " << peer_endpoint
                  << " already exists.";
      return kConnectionAlreadyExists;
    }
  }

  (*itr).transport->Connect(peer_endpoint, validation_data);
  return kSuccess;
}

void ManagedConnections::Remove(const Endpoint &peer_endpoint) {
  SharedLock shared_lock(shared_mutex_);
  auto itr(connection_map_.find(peer_endpoint));
  if (itr == connection_map_.end()) {
    DLOG(WARNING) << "Can't remove " << peer_endpoint << " - not in map.";
    return;
  }
  (*itr).second->CloseConnection(peer_endpoint);
}

int ManagedConnections::Send(const Endpoint &peer_endpoint,
                             const std::string &message) const {
  SharedLock shared_lock(shared_mutex_);
  auto itr(connection_map_.find(peer_endpoint));
  if (itr == connection_map_.end()) {
    DLOG(ERROR) << "Can't send to " << peer_endpoint << " - not in map.";
    return kInvalidConnection;
  }
  return (*itr).second->Send(peer_endpoint, message);
}

bool ManagedConnections::Ping(const Endpoint &peer_endpoint) const {
  return TryConnectTo(std::vector<Endpoint>(1, peer_endpoint), false);
}

void ManagedConnections::OnMessageSlot(const std::string &message) {
  SharedLock shared_lock(shared_mutex_);
  message_received_functor_(message);
}

void ManagedConnections::OnConnectionAddedSlot(const Endpoint &peer_endpoint,
                                               TransportPtr transport) {
  UniqueLock unique_lock(shared_mutex_);
  auto result(connection_map_.insert(std::make_pair(peer_endpoint, transport)));
  if (result.second)
    DLOG(INFO) << "Added managed connection to " << peer_endpoint;
  else
    DLOG(ERROR) << "Already connected to " << peer_endpoint;
}

void ManagedConnections::OnConnectionLostSlot(const Endpoint &peer_endpoint,
                                              TransportPtr transport) {
  UniqueLock unique_lock(shared_mutex_);
  size_t result(connection_map_.erase(peer_endpoint));
  if (result == 1U) {
    DLOG(INFO) << "Removed managed connection to " << peer_endpoint
               << (transport ? " - also removing corresponding transport" : "");
    connection_lost_functor_(peer_endpoint);
  } else {
    DLOG(ERROR) << "Was not connected to " << peer_endpoint;
  }

  if (!transport)
    return;

  auto itr(std::find_if(
      transports_.begin(),
      transports_.end(),
      [&transport](const TransportAndSignalConnections &element) {
        return transport == element.transport;
      }));

  if (itr == transports_.end()) {
    DLOG(ERROR) << "Failed to find transport in vector.";
    return;
  }
  (*itr).on_message_connection.disconnect();
  (*itr).on_connection_added_connection.disconnect();
  (*itr).on_connection_lost_connection.disconnect();
  transports_.erase(itr);
}

}  // namespace rudp

}  // namespace maidsafe
