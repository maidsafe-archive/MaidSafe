/*******************************************************************************
 *  Copyright 2011 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the license   *
 *  file LICENSE.TXT found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ***************************************************************************//**
 * @file  chunk_manager.h
 * @brief Interface allowing managed storage of chunks.
 * @date  2011-05-09
 */

#ifndef MAIDSAFE_PRIVATE_CHUNK_STORE_CHUNK_MANAGER_H_
#define MAIDSAFE_PRIVATE_CHUNK_STORE_CHUNK_MANAGER_H_

#include <functional>
#include <memory>
#include <string>

#include "boost/signals2/signal.hpp"

#include "maidsafe/common/rsa.h"

#include "maidsafe/private/version.h"
#if MAIDSAFE_PRIVATE_VERSION != 300
#  error This API is not compatible with the installed library.\
    Please update the library.
#endif

namespace bs2 = boost::signals2;

namespace maidsafe {

namespace priv {

namespace chunk_store {

class ChunkStore;

class ChunkManager {
 public:
  typedef bs2::signal<void(const std::string&, const int&)>
      ChunkStoredSig, ChunkModifiedSig, ChunkGotSig, ChunkDeletedSig;
  typedef std::shared_ptr<ChunkStoredSig> ChunkStoredSigPtr;
  typedef std::shared_ptr<ChunkModifiedSig> ChunkModifiedSigPtr;
  typedef std::shared_ptr<ChunkGotSig> ChunkGotSigPtr;
  typedef std::shared_ptr<ChunkDeletedSig> ChunkDeletedSigPtr;

  virtual ~ChunkManager() {}

  virtual void GetChunk(const std::string &name,
                        const asymm::Identity &owner_key_id,
                        const asymm::PublicKey &owner_public_key,
                        const std::string &ownership_proof) = 0;
  virtual void StoreChunk(const std::string &name,
                          const asymm::Identity &owner_key_id,
                          const asymm::PublicKey &owner_public_key) = 0;
  virtual void ModifyChunk(const std::string &name,
                           const std::string &content,
                           const asymm::Identity &owner_key_id,
                           const asymm::PublicKey &owner_public_key) = 0;
  virtual void DeleteChunk(const std::string &name,
                           const asymm::Identity &owner_key_id,
                           const asymm::PublicKey &owner_public_key,
                           const std::string &ownership_proof) = 0;

  ChunkGotSigPtr sig_chunk_got() { return sig_chunk_got_; }
  ChunkStoredSigPtr sig_chunk_stored() { return sig_chunk_stored_; }
  ChunkModifiedSigPtr sig_chunk_modified() { return sig_chunk_modified_; }
  ChunkDeletedSigPtr sig_chunk_deleted() { return sig_chunk_deleted_; }
  std::shared_ptr<ChunkStore> chunk_store() { return chunk_store_; }

 protected:
  explicit ChunkManager(std::shared_ptr<ChunkStore> chunk_store)
      : sig_chunk_got_(new ChunkGotSig),
        sig_chunk_stored_(new ChunkStoredSig),
        sig_chunk_modified_(new ChunkModifiedSig),
        sig_chunk_deleted_(new ChunkDeletedSig),
        chunk_store_(chunk_store) {}
  ChunkGotSigPtr sig_chunk_got_;
  ChunkStoredSigPtr sig_chunk_stored_;
  ChunkModifiedSigPtr sig_chunk_modified_;
  ChunkDeletedSigPtr sig_chunk_deleted_;
  std::shared_ptr<ChunkStore> chunk_store_;

 private:
  ChunkManager(const ChunkManager&);
  ChunkManager& operator=(const ChunkManager&);
};

}  // namespace chunk_store

}  // namespace priv

}  // namespace maidsafe

#endif  // MAIDSAFE_PRIVATE_CHUNK_STORE_CHUNK_MANAGER_H_
