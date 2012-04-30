/*
* ============================================================================
*
* Copyright [2011] maidsafe.net limited
*
* The following source code is property of maidsafe.net limited and is not
* meant for external use.  The use of this code is governed by the license
* file LICENSE.TXT found in the root of this directory and also on
* www.maidsafe.net.
*
* You are not free to copy, amend or otherwise use this source code without
* the explicit written permission of the board of directors of maidsafe.net.
*
* ============================================================================
*/


#include "maidsafe/private/chunk_store/local_chunk_manager.h"

#include "maidsafe/common/utils.h"

#include "maidsafe/private/log.h"
#include "maidsafe/private/return_codes.h"
#include "maidsafe/private/chunk_actions/chunk_action_authority.h"
#include "maidsafe/private/chunk_actions/chunk_types.h"
#include "maidsafe/private/chunk_store/file_chunk_store.h"
#include "maidsafe/private/chunk_store/threadsafe_chunk_store.h"

namespace pca = maidsafe::priv::chunk_actions;

namespace maidsafe {

namespace priv {

namespace chunk_store {

LocalChunkManager::LocalChunkManager(
    std::shared_ptr<ChunkStore> normal_local_chunk_store,
    const fs::path &simulation_directory,
    const boost::posix_time::time_duration &millisecs)
    : ChunkManager(normal_local_chunk_store),
      simulation_chunk_store_(),
      simulation_chunk_action_authority_(),
      get_wait_(millisecs),
      action_wait_(millisecs * 3) {
  std::shared_ptr<FileChunkStore> file_chunk_store(new FileChunkStore);
  fs::path local_version_directory;
  if (simulation_directory.empty()) {
    boost::system::error_code error_code;
    local_version_directory = fs::temp_directory_path(error_code);
    if (error_code) {
      DLOG(ERROR) << "Failed to get temp directory: " << error_code.message();
      return;
    }
    local_version_directory /= "LocalUserCredentials";
  } else {
    local_version_directory = simulation_directory;
  }

  if (!file_chunk_store->Init(simulation_directory)) {
    DLOG(ERROR) << "Failed to initialise file chunk store";
    return;
  }

  simulation_chunk_store_.reset(new ThreadsafeChunkStore(file_chunk_store));
  simulation_chunk_action_authority_.reset(
      new pca::ChunkActionAuthority(simulation_chunk_store_));
}

LocalChunkManager::~LocalChunkManager() {}

void LocalChunkManager::GetChunk(const std::string &name,
                                 const asymm::Identity &/*owner_key_id*/,
                                 const asymm::PublicKey &owner_public_key,
                                 const std::string &/*ownership_proof*/) {
  if (get_wait_.total_milliseconds() != 0) {
    Sleep(get_wait_);
  }
  // TODO(Team): Add check of ID on network
  if (chunk_store_->Has(name)) {
    (*sig_chunk_got_)(name, kSuccess);
    return;
  }

  std::string content(
      simulation_chunk_action_authority_->Get(name, "", owner_public_key));
  if (content.empty()) {
    DLOG(ERROR) << "CAA failure on network chunkstore " << Base32Substr(name);
    (*sig_chunk_got_)(name, kGetFailure);
    return;
  }

  if (!chunk_store_->Store(name, content)) {
    DLOG(ERROR) << "Failed to store locally " << Base32Substr(name);
    (*sig_chunk_got_)(name, kGetFailure);
    return;
  }

  (*sig_chunk_got_)(name, kSuccess);
}

void LocalChunkManager::StoreChunk(const std::string &name,
                                   const asymm::Identity &/*owner_key_id*/,
                                   const asymm::PublicKey &owner_public_key) {
  if (get_wait_.total_milliseconds() != 0) {
    Sleep(action_wait_);
  }

  // TODO(Team): Add check of ID on network
  std::string content(chunk_store_->Get(name));
  if (content.empty()) {
    DLOG(ERROR) << "No chunk in local chunk store " << Base32Substr(name);
    (*sig_chunk_stored_)(name, kStoreFailure);
    return;
  }

  if (!simulation_chunk_action_authority_->Store(name,
                                                 content,
                                                 owner_public_key)) {
    DLOG(ERROR) << "CAA failure on network chunkstore " << Base32Substr(name);
    (*sig_chunk_stored_)(name, kStoreFailure);
    return;
  }

  (*sig_chunk_stored_)(name, kSuccess);
}

void LocalChunkManager::DeleteChunk(const std::string &name,
                                    const asymm::Identity &/*owner_key_id*/,
                                    const asymm::PublicKey &owner_public_key,
                                    const std::string &ownership_proof) {
  if (get_wait_.total_milliseconds() != 0) {
    Sleep(action_wait_);
  }

  // TODO(Team): Add check of ID on network
  if (!simulation_chunk_action_authority_->Delete(name,
                                                  ownership_proof,
                                                  owner_public_key)) {
    DLOG(ERROR) << "CAA failure on network chunkstore " << Base32Substr(name);
    (*sig_chunk_deleted_)(name, kDeleteFailure);
    return;
  }

  (*sig_chunk_deleted_)(name, kSuccess);
}

void LocalChunkManager::ModifyChunk(const std::string &name,
                                    const std::string &content,
                                    const asymm::Identity &/*owner_key_id*/,
                                    const asymm::PublicKey &owner_public_key) {
  if (get_wait_.total_milliseconds() != 0) {
    Sleep(action_wait_);
  }

  int64_t operation_diff;
  if (!simulation_chunk_action_authority_->Modify(name,
                                                  content,
                                                  owner_public_key,
                                                  &operation_diff)) {
    DLOG(ERROR) << "CAA failure on network chunkstore " << Base32Substr(name);
    (*sig_chunk_modified_)(name, kModifyFailure);
    return;
  }

  (*sig_chunk_modified_)(name, kSuccess);
}

}  // namespace chunk_store

}  // namespace priv

}  // namespace maidsafe
