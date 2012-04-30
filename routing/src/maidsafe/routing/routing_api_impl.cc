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

#include "maidsafe/routing/routing_api_impl.h"
#include "maidsafe/common/utils.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/routing_pb.h"
#include "maidsafe/routing/node_id.h"
#include "maidsafe/routing/routing_table.h"
#include "maidsafe/routing/timer.h"
#include "maidsafe/routing/bootstrap_file_handler.h"
#include "maidsafe/routing/return_codes.h"
#include "maidsafe/routing/utils.h"
#include "maidsafe/routing/message_handler.h"
#include "maidsafe/routing/parameters.h"

namespace maidsafe {

namespace routing {

RoutingPrivate::RoutingPrivate(const asymm::Keys &keys,
                               const boost::filesystem::path &path,
                               NodeValidationFunctor node_validation_functor,
                               bool client_mode)
    : asio_service_(),
      bootstrap_nodes_(),
      keys_(keys),
      rudp_(),
      routing_table_(keys_),
      timer_(asio_service_),
      message_received_signal_(),
      network_status_signal_(),
      close_node_from_to_signal_(),
      node_validation_signal_(),
      waiting_for_response_(),
      direct_non_routing_table_connections_(),
      message_handler_(routing_table_,
                      rudp_,
                      timer_,
                      node_validation_functor),
      joined_(false),
      bootstrap_file_path_(path),
      client_mode_(client_mode) {}

RoutingPrivate::~RoutingPrivate() {}

}  // namespace routing

}  // namespace maidsafe
