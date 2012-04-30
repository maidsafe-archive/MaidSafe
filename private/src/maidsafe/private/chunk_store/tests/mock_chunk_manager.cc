/*******************************************************************************
 *  Copyright 2010-2011 maidsafe.net limited                                   *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the license   *
 *  file LICENSE.TXT found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ***************************************************************************//**
 * @file  mock_chunk_manager.cc
 * @brief A mock chunk_manager class
 * @date  2011-31-10
 */

#include "maidsafe/private/chunk_store/tests/mock_chunk_manager.h"

namespace maidsafe {

namespace priv {

namespace chunk_store {

namespace test {

MockChunkManager::MockChunkManager(
    std::shared_ptr<priv::chunk_store::ChunkStore> chunk_store)
    : ChunkManager(chunk_store),
      thread_group_() {}

MockChunkManager::~MockChunkManager() {
  thread_group_.join_all();
}


}  // namespace test

}  // namespace chunk_store

}  // namespace priv

}  // namespace maidsafe
