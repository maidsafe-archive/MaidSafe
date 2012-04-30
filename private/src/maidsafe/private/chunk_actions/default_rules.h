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

// Applies to self-encrypted file chunks

#ifndef MAIDSAFE_PRIVATE_CHUNK_ACTIONS_DEFAULT_RULES_H_
#define MAIDSAFE_PRIVATE_CHUNK_ACTIONS_DEFAULT_RULES_H_

#include <memory>
#include <string>

#include "maidsafe/common/rsa.h"

#include "maidsafe/private/chunk_actions/chunk_types.h"
#include "maidsafe/private/version.h"

#if MAIDSAFE_PRIVATE_VERSION != 300
#  error This API is not compatible with the installed library.\
    Please update the library.
#endif


namespace maidsafe {

namespace priv {

namespace chunk_store { class ChunkStore; }

namespace chunk_actions {

template <unsigned char DataType>
bool IsCacheable();

template <unsigned char DataType>
bool IsModifiable();

template <unsigned char DataType>
bool DoesModifyReplace();

template <unsigned char DataType>
bool IsValidChunk(const std::string &name,
                  std::shared_ptr<chunk_store::ChunkStore> chunk_store);

template <unsigned char DataType>
std::string GetVersion(const std::string &name,
                       std::shared_ptr<chunk_store::ChunkStore> chunk_store);

template <unsigned char DataType>
int ProcessGet(const std::string &name,
               const std::string &version,
               const asymm::PublicKey &public_key,
               std::string *existing_content,
               std::shared_ptr<chunk_store::ChunkStore> chunk_store);

template <unsigned char DataType>
int ProcessStore(const std::string &name,
                 const std::string &content,
                 const asymm::PublicKey &public_key,
                 std::shared_ptr<chunk_store::ChunkStore> chunk_store);

template <unsigned char DataType>
int ProcessDelete(const std::string &name,
                  const std::string &ownership_proof,
                  const asymm::PublicKey &public_key,
                  std::shared_ptr<chunk_store::ChunkStore> chunk_store);

template <unsigned char DataType>
int ProcessModify(const std::string &name,
                  const std::string &content,
                  const asymm::PublicKey &public_key,
                  int64_t *size_difference,
                  std::string *new_content,
                  std::shared_ptr<chunk_store::ChunkStore> chunk_store);

template <unsigned char DataType>
int ProcessHas(const std::string &name,
               const std::string &version,
               const asymm::PublicKey &public_key,
               std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Returns true.
template <>
bool IsCacheable<kDefaultType>();

// Returns false.
template <>
bool IsModifiable<kDefaultType>();

// Returns false.
template <>
bool DoesModifyReplace<kDefaultType>();

// Returns true if the chunk exists, and name == Hash(content).
template <>
bool IsValidChunk<kDefaultType>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Returns first 24 bytes of name.
template <>
std::string GetVersion<kDefaultType>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Any user can Get.
// For overall success, the following must be true:
//   * chunk_store.get() succeeds
// NB - version is not used in this function.
template <>
int ProcessGet<kDefaultType>(
    const std::string &name,
    const std::string &version,
    const asymm::PublicKey &public_key,
    std::string *existing_content,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Any user can Store.
// For overall success, the following must be true:
//   * public_key is valid
//   * if the chunk exsist already, content must match existing content,
//     otherwise name must match Hash(content)
// This assumes that public_key has not been revoked on the network.
template <>
int ProcessStore<kDefaultType>(
    const std::string &name,
    const std::string &content,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Any user can Delete.
// Always returns kSuccess.
// This assumes that owner of public_key has already been confirmed as being
// a valid Chunk Info Holder, and that public_key has not been revoked on the
// network.
template <>
int ProcessDelete<kDefaultType>(
    const std::string &name,
    const std::string &ownership_proof,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

// Modify is an invalid operation for all users.
template <>
int ProcessModify<kDefaultType>(
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
int ProcessHas<kDefaultType>(
    const std::string &name,
    const std::string &version,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store);

}  // namespace chunk_actions

}  // namespace priv

}  // namespace maidsafe

#endif  // MAIDSAFE_PRIVATE_CHUNK_ACTIONS_DEFAULT_RULES_H_
