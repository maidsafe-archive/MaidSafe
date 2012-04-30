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

// Applies to PKI SignaturePackets

#ifndef MAIDSAFE_PRIVATE_CHUNK_ACTIONS_SIGNATURE_PACKET_RULES_H_
#define MAIDSAFE_PRIVATE_CHUNK_ACTIONS_SIGNATURE_PACKET_RULES_H_

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

extern const std::string kRevokedSignaturePacket;

// Returns false.
template <>
bool IsCacheable<kSignaturePacket>();

// Returns false.
template <>
bool IsModifiable<kSignaturePacket>();

// Returns false.
template <>
bool DoesModifyReplace<kSignaturePacket>();

// Returns true if the chunk exists, and
// name == Hash(chunk.data() + chunk.signature()).
template <>
bool IsValidChunk<kSignaturePacket>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Returns first 24 bytes of name.
template <>
std::string GetVersion<kSignaturePacket>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Any user can Get.
// For overall success, the following must be true:
//   * chunk_store.get() succeeds
// NB - version is not used in this function.
template <>
int ProcessGet<kSignaturePacket>(
    const std::string &name,
    const std::string &version,
    const asymm::PublicKey &public_key,
    std::string *existing_content,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Any user can Store.
// For overall success, the following must be true:
//   * the chunk doesn't already exist
//   * content parses as a SignedData
//   * public_key is valid
//   * chunk.signature() validates with public_key
//   * name must match Hash(chunk.data())
// This assumes that public_key has not been revoked on the network.
template <>
int ProcessStore<kSignaturePacket>(
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
int ProcessDelete<kSignaturePacket>(
    const std::string &name,
    const std::string &ownership_proof,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Modify is an invalid operation for all users.
template <>
int ProcessModify<kSignaturePacket>(
    const std::string &name,
    const std::string &content,
    const asymm::PublicKey &public_key,
    int64_t *size_difference,
    std::string *new_content,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Any user can call Has.
// For overall success, the following must be true:
//   * chunk_store.has() succeeds
// NB - version is not used in this function.
template <>
int ProcessHas<kSignaturePacket>(
    const std::string &name,
    const std::string &version,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

}  // namespace chunk_actions

}  // namespace priv

}  // namespace maidsafe

#endif  // MAIDSAFE_PRIVATE_CHUNK_ACTIONS_SIGNATURE_PACKET_RULES_H_
