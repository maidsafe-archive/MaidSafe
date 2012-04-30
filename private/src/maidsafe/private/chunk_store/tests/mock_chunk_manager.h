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
 * @file  mock_chunk_manager.h
 * @brief A mock chunk_manager object, and related helper methods.
 * @date  2011-28-10
 */

#ifndef MAIDSAFE_PRIVATE_CHUNK_STORE_TESTS_MOCK_CHUNK_MANAGER_H_
#define MAIDSAFE_PRIVATE_CHUNK_STORE_TESTS_MOCK_CHUNK_MANAGER_H_

#include <set>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "boost/thread.hpp"

#include "maidsafe/common/utils.h"

#include "maidsafe/private/chunk_store/chunk_store.h"
#include "maidsafe/private/chunk_store/chunk_manager.h"
#include "maidsafe/private/return_codes.h"


namespace maidsafe {

namespace priv {

namespace chunk_store {

namespace test {

class MockChunkManager : public priv::chunk_store::ChunkManager {
 public:
  explicit MockChunkManager(
      std::shared_ptr<priv::chunk_store::ChunkStore> chunk_store);
  virtual ~MockChunkManager();

  MOCK_METHOD4(GetChunk, void(const std::string &chunk_name,
                              const asymm::Identity &owner_key_id,
                              const asymm::PublicKey &owner_public_key,
                              const std::string &ownership_proof));

  MOCK_METHOD3(StoreChunk, void(const std::string &chunk_name,
                                const asymm::Identity &owner_key_id,
                                const asymm::PublicKey &owner_public_key));

  MOCK_METHOD4(ModifyChunk, void(const std::string &name,
                                 const std::string &content,
                                 const asymm::Identity &owner_key_id,
                                 const asymm::PublicKey &owner_public_key));

  MOCK_METHOD4(DeleteChunk, void(const std::string &chunk_name,
                                 const asymm::Identity &owner_key_id,
                                 const asymm::PublicKey &owner_public_key,
                                 const std::string &ownership_proof));

  void Timeout() {
    // do nothing, causing an eventual timeout
  }
  void StoreChunkPass(const std::string &chunk_name) {
    chunk_store()->Store(chunk_name, RandomString(128));
    (*sig_chunk_stored_)(chunk_name, kSuccess);
  }

 private:
  MockChunkManager &operator=(const MockChunkManager&);
  MockChunkManager(const MockChunkManager&);
  boost::thread_group thread_group_;
};

}  // namespace test

}  // namespace chunk_store

}  // namespace priv

}  // namespace maidsafe

#endif  // MAIDSAFE_PRIVATE_CHUNK_STORE_TESTS_MOCK_CHUNK_MANAGER_H_
