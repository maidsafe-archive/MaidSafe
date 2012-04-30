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

#include "maidsafe/routing/cache_manager.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/utils.h"
#include "maidsafe/routing/routing_pb.h"
#include "maidsafe/routing/routing_table.h"
#include "maidsafe/common/crypto.h"

namespace maidsafe {

namespace routing {

CacheManager::CacheManager()
                    : cache_chunks_(),
                    mutex_()
                    {}

void CacheManager::AddToCache(const protobuf::Message &message) {
    std::pair<std::string, std::string> data;
  try {
    // check data is valid TODO FIXME - ask CAA
    if (crypto::Hash<crypto::SHA512>(message.data()) != message.source_id())
      return;
    data = std::make_pair(message.source_id(), message.data());
    cache_chunks_.push_back(data);
    boost::mutex::scoped_lock lock(mutex_);
    while (cache_chunks_.size() > Parameters::num_chunks_to_cache)
      cache_chunks_.erase(cache_chunks_.begin());
  }
  catch(const std::exception &/*e*/) {
    // oohps reduce cache size quickly
    Parameters::num_chunks_to_cache = Parameters::num_chunks_to_cache / 2;
    boost::mutex::scoped_lock lock(mutex_);
    while (cache_chunks_.size() > Parameters::num_chunks_to_cache)
      cache_chunks_.erase(cache_chunks_.begin()+1);
  }
}

bool CacheManager::GetFromCache(protobuf::Message &message) {
    boost::mutex::scoped_lock lock(mutex_);
    for (auto it = cache_chunks_.begin(); it != cache_chunks_.end(); ++it) {
      if ((*it).first == message.source_id()) {
        message.set_destination_id(message.source_id());
        message.set_data((*it).second);
        message.set_direct(true);
        message.set_type(-message.type());
        return true;
      }
  }
  return false;
}

}  // namespace routing
}  // namespace maidsafe
