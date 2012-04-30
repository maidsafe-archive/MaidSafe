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

#ifndef MAIDSAFE_ROUTING_PARAMETERS_H_
#define MAIDSAFE_ROUTING_PARAMETERS_H_

#include "boost/asio.hpp"
#include "boost/filesystem/path.hpp"
#include "maidsafe/routing/log.h"

namespace maidsafe {

namespace routing {

typedef boost::asio::ip::udp::endpoint Endpoint;

struct Parameters {
 public:
  Parameters();
  ~Parameters();
  // this node is client or node (full routing node with storage)
  static bool client_mode;
  // fully encrypt all data at routing level in both directions
  static bool encryption_required;
  static uint16_t num_chunks_to_cache;
  static uint16_t timout_in_seconds;
  static uint16_t closest_nodes_size;
  static uint16_t max_routing_table_size;
  static uint16_t bucket_target_size;
 private:
  Parameters(const Parameters&);  // no copy
  Parameters(const Parameters&&);  // no move
  Parameters& operator=(const Parameters&);  // no assign
};

}  // namespace routing

}  // namespace maidsafe

#endif // MAIDSAFE_ROUTING_PARAMETERS_H_
