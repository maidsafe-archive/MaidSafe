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

#include "maidsafe/routing/parameters.h"

namespace maidsafe {

namespace routing {

bool Parameters::encryption_required(false);
uint16_t Parameters::num_chunks_to_cache(100);
uint16_t Parameters::timout_in_seconds(5);
uint16_t Parameters::closest_nodes_size(8);
uint16_t Parameters::max_routing_table_size(64);
uint16_t Parameters::bucket_target_size(1);
}  // namespace routing

}  // namespace maidsafe
