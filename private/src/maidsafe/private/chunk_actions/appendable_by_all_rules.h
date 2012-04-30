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

// Applies to MaidSafe Contact ID & MaidSafe Message ID

#ifndef MAIDSAFE_PRIVATE_CHUNK_ACTIONS_APPENDABLE_BY_ALL_RULES_H_
#define MAIDSAFE_PRIVATE_CHUNK_ACTIONS_APPENDABLE_BY_ALL_RULES_H_

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
bool IsCacheable<kAppendableByAll>();

// Returns true.
template <>
bool IsModifiable<kAppendableByAll>();

// Returns false.
template <>
bool DoesModifyReplace<kAppendableByAll>();

// Returns true if the chunk exists.
template <>
bool IsValidChunk<kAppendableByAll>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Returns Tiger hash of chunk content.
template <>
std::string GetVersion<kAppendableByAll>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Any user can Get.  Owner gets all data; non-owner gets only first value
// which contains owner's public key for encryption of messages.
// For overall success, the following must be true:
//   * chunk_store.get() succeeds
//   * public_key is valid
// This assumes that public_key has not been revoked on the network.
// NB - version is currently ignored for this function.
template <>
int ProcessGet<kAppendableByAll>(
    const std::string &name,
    const std::string &version,
    const asymm::PublicKey &public_key,
    std::string *existing_content,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Any user can Store.
// For overall success, the following must be true:
//   * the chunk doesn't already exist
//   * content parses as AppendableByAll
//   * public_key is valid
//   * chunk.signature() validates with public_key
// This assumes that public_key has not been revoked on the network.
template <>
int ProcessStore<kAppendableByAll>(
    const std::string &name,
    const std::string &content,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Only owner can Delete.
// For overall success, the following must be true:
//   * the chunk doesn't already exsist
//                OR
//   * chunk_store.get() succeeds
//   * public_key is valid
//   * retrieved chunk.signature() validates with public_key
//   * deletion_token validates with public_key
// This assumes that public_key has not been revoked on the network.
template <>
int ProcessDelete<kAppendableByAll>(
    const std::string &name,
    const std::string &ownership_proof,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Any user can Modify.
// The first value contains owner's public key for encryption of messages, and a
// bool authorising other users to append data.
// Owner can either replace only first value (by sending a modified first value)
// or can remove all appended values (by sending an unmodified first value).
// For overall success as owner, the following must be true:
//   * chunk_store.get() succeeds
//   * retrieved content parses as AppendableByAll
//   * public_key is valid
//   * retrieved chunk.signature() validates with public_key
//   * content parses as AppendableByAll
//   * new chunk.signature() validates with public_key
// This assumes that public_key has not been revoked on the network.
// Non-owner can only append a value iff the bool in the first value is true.
// For overall success as non-owner, the following must be true:
//   * chunk_store.get() succeeds
//   * retrieved content parses as AppendableByAll
//   * public_key is valid
//   * owner has set allow_others_to_append to true
//   * content parses as AppendableByAll
//   * chunk.signature() validates with public_key
// This assumes that public_key has not been revoked on the network.
template <>
int ProcessModify<kAppendableByAll>(
    const std::string &name,
    const std::string &content,
    const asymm::PublicKey &public_key,
    int64_t *size_difference,
    std::string *new_content,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Any user can call Has.
// For overall success, the following must be true:
//   * chunk_store.has() succeeds
// NB - version is currently ignored for this function.
template <>
int ProcessHas<kAppendableByAll>(
    const std::string &name,
    const std::string &version,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

}  // namespace chunk_actions

}  // namespace priv

}  // namespace maidsafe

#endif  // MAIDSAFE_PRIVATE_CHUNK_ACTIONS_APPENDABLE_BY_ALL_RULES_H_
