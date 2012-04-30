/*
* ============================================================================
*
* Copyright [2011] maidsafe.net limited
*
* The following source code is property of maidsafe.net limited and is not
* meant for external use.  The use of this code is governed by the license
* file licence.txt found in the root of this directory and also on
* www.maidsafe.net.
*
* You are not free to copy, amend or otherwise use this source code without
* the explicit written permission of the board of directors of maidsafe.net.
*
* ============================================================================
*/

#include "maidsafe/private/chunk_actions/default_rules.h"

#include "maidsafe/common/crypto.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/private/log.h"
#include "maidsafe/private/return_codes.h"
#include "maidsafe/private/chunk_actions/chunk_action_authority.h"
#include "maidsafe/private/chunk_actions/chunk_pb.h"
#include "maidsafe/private/chunk_actions/utils.h"

#include "maidsafe/private/chunk_store/chunk_store.h"

namespace maidsafe {

namespace priv {

namespace chunk_actions {

template <>
bool IsCacheable<kDefaultType>() { return true; }

template <>
bool IsModifiable<kDefaultType>() { return false; }

template <>
bool DoesModifyReplace<kDefaultType>() { return false; }

template <>
bool IsValidChunk<kDefaultType>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  std::string content(chunk_store->Get(name));
  if (content.empty()) {
    DLOG(ERROR) << "Failed to get " << Base32Substr(name) << " for validation";
    return false;
  }

  if (crypto::Hash<crypto::SHA512>(content) != RemoveTypeFromName(name)) {
    DLOG(ERROR) << "Failed to validate " << Base32Substr(name)
                << ": chunk isn't hashable";
    return false;
  }

  return true;
}

template <>
std::string GetVersion<kDefaultType>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> /*chunk_store*/) {
  return name.substr(0, crypto::Tiger::DIGESTSIZE);
}

template <>
int ProcessGet<kDefaultType>(
    const std::string &name,
    const std::string &/*version*/,
    const asymm::PublicKey &/*public_key*/,
    std::string *existing_content,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  *existing_content = chunk_store->Get(name);
  if (existing_content->empty()) {
    DLOG(WARNING) << "Failed to get " << Base32Substr(name);
    return kFailedToFindChunk;
  }

  return kSuccess;
}

template <>
int ProcessStore<kDefaultType>(
    const std::string &name,
    const std::string &content,
    const asymm::PublicKey &/*public_key*/,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  std::string existing_content(chunk_store->Get(name));
  if (existing_content.empty()) {
    // New chunk on network - check data hashes to name
    if (crypto::Hash<crypto::SHA512>(content) != RemoveTypeFromName(name)) {
      DLOG(ERROR) << "Failed to store " << Base32Substr(name)
                  << ": default chunk type should be hashable";
      return kNotHashable;
    }
  } else {
    // Pre-existing chunk - ensure data is identical
    if (existing_content != content) {
      DLOG(ERROR) << "Failed to store " << Base32Substr(name)
                  << ": existing data doesn't match new data - can't store";
      return kInvalidSignedData;
    }
  }

  return kSuccess;
}

template <>
int ProcessDelete<kDefaultType>(
    const std::string &/*name*/,
    const std::string &/*ownership_proof*/,
    const asymm::PublicKey &/*public_key*/,
    std::shared_ptr<chunk_store::ChunkStore> /*chunk_store*/) {
  return kSuccess;
}

template <>
int ProcessModify<kDefaultType>(
    const std::string &name,
    const std::string &/*content*/,
    const asymm::PublicKey &/*public_key*/,
    int64_t * /*size_difference*/,
    std::string * /*new_content*/,
    std::shared_ptr<chunk_store::ChunkStore> /*chunk_store*/) {
  DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
              << ": no modify of default chunk type allowed";
  return kInvalidModify;
}

template <>
int ProcessHas<kDefaultType>(
    const std::string &name,
    const std::string &/*version*/,
    const asymm::PublicKey &/*public_key*/,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  if (!chunk_store->Has(name)) {
    DLOG(WARNING) << "Failed to find " << Base32Substr(name);
    return kFailedToFindChunk;
  }

  return kSuccess;
}

}  // namespace chunk_actions

}  // namespace priv

}  // namespace maidsafe
