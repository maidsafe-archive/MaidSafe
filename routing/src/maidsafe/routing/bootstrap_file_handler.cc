/*******************************************************************************
 *  Copyright 2012 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ******************************************************************************/

#include "boost/thread/shared_mutex.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
#include "maidsafe/common/utils.h"
#include "maidsafe/rudp/managed_connections.h"
#include "maidsafe/routing/bootstrap_file_handler.h"
#include "maidsafe/routing/parameters.h"
// #include "maidsafe/routing/error.h"
#include "maidsafe/routing/log.h"

namespace maidsafe {

namespace routing {

std::vector<boost::asio::ip::udp::endpoint>
                                      ReadBootstrapFile(const fs::path &path) {
  protobuf::Bootstrap protobuf_bootstrap;
  std::vector<boost::asio::ip::udp::endpoint> bootstrap_nodes;

  std::string serialised_endpoints;
  if (!ReadFile(path, &serialised_endpoints)) {
     DLOG(ERROR) << "could not read bootstrap file";
    return bootstrap_nodes;
  }
  if (!protobuf_bootstrap.ParseFromString(serialised_endpoints)) {
    DLOG(ERROR) << "could not parse bootstrap file";
    return bootstrap_nodes;
  }
  bootstrap_nodes.resize(protobuf_bootstrap.bootstrap_contacts().size());
  boost::asio::ip::udp::endpoint endpoint;
  for (int i = 0; i < protobuf_bootstrap.bootstrap_contacts().size(); ++i) {
    endpoint.address().from_string(protobuf_bootstrap.bootstrap_contacts(i).ip());
    endpoint.port(protobuf_bootstrap.bootstrap_contacts(i).port());
    bootstrap_nodes[i] = endpoint;
  }
  return  bootstrap_nodes;
}

bool WriteBootstrapFile(const std::vector<boost::asio::ip::udp::endpoint>
                                                                    &endpoints,
                        const fs::path & path) {
  protobuf::Bootstrap protobuf_bootstrap;

  for (size_t i = 0; i < endpoints.size(); ++i) {
    protobuf::Endpoint *endpoint = protobuf_bootstrap.add_bootstrap_contacts();
    endpoint->set_ip(endpoints[i].address().to_string());
    endpoint->set_port(endpoints[i].port());
  }
  std::string serialised_bootstrap_nodes;
  protobuf_bootstrap.SerializeToString(&serialised_bootstrap_nodes);
  return (WriteFile(path, serialised_bootstrap_nodes));
}

}  // namespace routing

}  // namespace maidsafe
