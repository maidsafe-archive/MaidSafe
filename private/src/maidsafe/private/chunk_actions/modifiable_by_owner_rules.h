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

// Applies to MID, SMID, TMID, STMID and encrypted Directory Listing DataMaps

#ifndef MAIDSAFE_PRIVATE_CHUNK_ACTIONS_MODIFIABLE_BY_OWNER_RULES_H_
#define MAIDSAFE_PRIVATE_CHUNK_ACTIONS_MODIFIABLE_BY_OWNER_RULES_H_

#include <memory>
#include <string>

#include "maidsafe/common/rsa.h"

#include "maidsafe/private/chunk_actions/chunk_types.h"
#include "maidsafe/private/chunk_actions/default_rules.h"
#include "maidsafe/private/version.h"

#if MAIDSAFE_PRIVATE_VERSION != 300
#  error This API is not compatible with the installed library.\
    Please update the library.
#endif


namespace maidsafe {

namespace priv {

namespace chunk_store { class ChunkStore; }

namespace chunk_actions {

// Returns false.
template <>
bool IsCacheable<kModifiableByOwner>();

// Returns true.
template <>
bool IsModifiable<kModifiableByOwner>();

// Returns true.
template <>
bool DoesModifyReplace<kModifiableByOwner>();

// Returns true if the chunk exists.
template <>
bool IsValidChunk<kModifiableByOwner>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Returns Tiger hash of chunk content.
template <>
std::string GetVersion<kModifiableByOwner>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Any user can Get.
// For overall success, the following must be true:
//   * chunk_store.get() succeeds
//   * if version is not an empty string, retrieved chunk's version must be
//     identical to this
template <>
int ProcessGet<kModifiableByOwner>(
    const std::string &name,
    const std::string &version,
    const asymm::PublicKey &public_key,
    std::string *existing_content,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Any user can Store.
// For overall success, the following must be true:
//   * the chunk doesn't already exist
//   * content parses as SignedData
//   * public_key is valid
//   * chunk.signature() validates with public_key
// This assumes that public_key has not been revoked on the network.
template <>
int ProcessStore<kModifiableByOwner>(
    const std::string &name,
    const std::string &content,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Only owner can Delete.
// For overall success, the following must be true:
//   * the chunk doesn't already exist
//                OR
//   * chunk_store.get() succeeds
//   * public_key is valid
//   * retrieved chunk.signature() validates with public_key
//   * deletion_token validates with public_key
// This assumes that public_key has not been revoked on the network.
template <>
int ProcessDelete<kModifiableByOwner>(
    const std::string &name,
    const std::string &ownership_proof,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Only owner can Modify.
// For overall success, the following must be true:
//   * chunk_store.get() succeeds
//   * retrieved content parses as SignedData
//   * public_key is valid
//   * retrieved chunk.signature() validates with public_key
//   * content parses as SignedData
//   * new chunk.signature() validates with public_key
// This assumes that public_key has not been revoked on the network.
template <>
int ProcessModify<kModifiableByOwner>(
    const std::string &name,
    const std::string &content,
    const asymm::PublicKey &public_key,
    int64_t *size_difference,
    std::string *new_content,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Any user can call Has.
// For overall success, the following must be true:
//   * chunk_store.has() succeeds
//   * if version is not an empty string, retrieved chunk's version must be
//     identical to this
template <>
int ProcessHas<kModifiableByOwner>(
    const std::string &name,
    const std::string &version,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

}  // namespace chunk_actions

}  // namespace priv

}  // namespace maidsafe

#endif  // MAIDSAFE_PRIVATE_CHUNK_ACTIONS_MODIFIABLE_BY_OWNER_RULES_H_
