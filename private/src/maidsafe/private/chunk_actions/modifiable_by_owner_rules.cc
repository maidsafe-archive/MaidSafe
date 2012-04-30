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

#include "maidsafe/private/chunk_actions/modifiable_by_owner_rules.h"

#include "maidsafe/common/utils.h"

#include "maidsafe/private/log.h"
#include "maidsafe/private/return_codes.h"

#include "maidsafe/private/chunk_actions/chunk_pb.h"
#include "maidsafe/private/chunk_actions/chunk_action_authority.h"
#include "maidsafe/private/chunk_actions/utils.h"

#include "maidsafe/private/chunk_store/chunk_store.h"

namespace maidsafe {

namespace priv {

namespace chunk_actions {

template <>
bool IsCacheable<kModifiableByOwner>() { return false; }

template <>
bool IsModifiable<kModifiableByOwner>() { return true; }

template <>
bool DoesModifyReplace<kModifiableByOwner>() { return true; }

template <>
bool IsValidChunk<kModifiableByOwner>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  // TODO(Fraser#5#): 2011-12-17 - Check this is all that's needed here
  std::string existing_data(chunk_store->Get(name));
  if (existing_data.empty()) {
    DLOG(ERROR) << "Failed to get " << Base32Substr(name) << " for validation";
    return false;
  }
  return true;
}

template <>
std::string GetVersion<kModifiableByOwner>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  std::string hash;
  return (GetContentAndTigerHash(name, chunk_store, nullptr,
                                 &hash) == kSuccess ? hash : "");
}

template <>
int ProcessGet<kModifiableByOwner>(
    const std::string &name,
    const std::string &version,
    const asymm::PublicKey &/*public_key*/,
    std::string *existing_content,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  if (version.empty()) {
    *existing_content = chunk_store->Get(name);
    if (existing_content->empty()) {
      DLOG(ERROR) << "Failed to get " << Base32Substr(name);
      return kFailedToFindChunk;
    }
  } else {
    std::string existing_version;
    int result(GetContentAndTigerHash(name, chunk_store, existing_content,
                                      &existing_version));
    if (result != kSuccess) {
      DLOG(ERROR) << "Failed to get " << Base32Substr(name);
      return result;
    }
    if (version != existing_version) {
      DLOG(ERROR) << "Failed to get requested version of "
                  << Base32Substr(name);
      return kDifferentVersion;
    }
  }

  return kSuccess;
}

template <>
int ProcessStore<kModifiableByOwner>(
    const std::string &name,
    const std::string &content,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  if (chunk_store->Has(name)) {
    DLOG(WARNING) << "Failed to store " << Base32Substr(name)
                  << ": chunk already exists";
    return kKeyNotUnique;
  }

  SignedData chunk;
  if (!ParseProtobuf<SignedData>(content, &chunk)) {
    DLOG(ERROR) << "Failed to store " << Base32Substr(name)
                << ": data doesn't parse as a chunk";
    return kInvalidSignedData;
  }

  if (!asymm::ValidateKey(public_key)) {
    DLOG(ERROR) << "Failed to store " << Base32Substr(name)
                << ": invalid public key";
    return kInvalidPublicKey;
  }

  if (asymm::CheckSignature(chunk.data(), chunk.signature(), public_key) !=
      kSuccess) {
    DLOG(ERROR) << "Failed to store " << Base32Substr(name)
                << ": signature verification failed";
    return kSignatureVerificationFailure;
  }

  return kSuccess;
}

template <>
int ProcessDelete<kModifiableByOwner>(
    const std::string &name,
    const std::string &ownership_proof,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  std::string existing_content = chunk_store->Get(name);
  if (existing_content.empty()) {
    DLOG(INFO) << Base32Substr(name) << " already deleted";
    return kSuccess;
  }

  SignedData existing_chunk;
  if (!ParseProtobuf<SignedData>(existing_content, &existing_chunk)) {
    DLOG(ERROR) << "Failed to delete " << Base32Substr(name)
                << ": existing data doesn't parse";
    return kGeneralError;
  }

  if (!asymm::ValidateKey(public_key)) {
    DLOG(ERROR) << "Failed to delete " << Base32Substr(name)
                << ": invalid public key";
    return kInvalidPublicKey;
  }

  if (asymm::CheckSignature(existing_chunk.data(), existing_chunk.signature(),
                            public_key) != kSuccess) {
    DLOG(ERROR) << "Failed to delete " << Base32Substr(name)
                << ": signature verification failed";
    return kSignatureVerificationFailure;
  }

  SignedData deletion_token;
  if (!ParseProtobuf<SignedData>(ownership_proof, &deletion_token)) {
    DLOG(ERROR) << "Failed to delete " << Base32Substr(name)
                << ": deletion_token doesn't parse - not owner";
    return kNotOwner;
  }
  if (asymm::CheckSignature(deletion_token.data(), deletion_token.signature(),
                            public_key) != kSuccess) {
    DLOG(ERROR) << "Failed to delete " << Base32Substr(name)
                << ": signature verification failed - not owner";
    return kNotOwner;
  }

  return kSuccess;
}

template <>
int ProcessModify<kModifiableByOwner>(
    const std::string &name,
    const std::string &content,
    const asymm::PublicKey &public_key,
    int64_t *size_difference,
    std::string *new_content,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  new_content->clear();
  std::string existing_content = chunk_store->Get(name);
  if (existing_content.empty()) {
    DLOG(ERROR) << Base32Substr(name) << " doesn't exist";
    return kFailedToFindChunk;
  }

  SignedData existing_chunk;
  if (!ParseProtobuf<SignedData>(existing_content, &existing_chunk)) {
    DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
                << ": existing data doesn't parse as SignedData";
    return kGeneralError;
  }

  if (!asymm::ValidateKey(public_key)) {
    DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
                << ": invalid public key";
    return kInvalidPublicKey;
  }

  if (asymm::CheckSignature(existing_chunk.data(), existing_chunk.signature(),
                            public_key) != kSuccess) {
    DLOG(ERROR) << "Failed to modify " << Base32Substr(name) << ": not owner";
    return kNotOwner;
  }

  SignedData new_chunk;
  if (!ParseProtobuf<SignedData>(content, &new_chunk)) {
    DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
                << ": new data doesn't parse as SignedData";
    return kInvalidSignedData;
  }

  if (asymm::CheckSignature(new_chunk.data(), new_chunk.signature(),
                            public_key) != kSuccess) {
    DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
                << ": signature verification failed";
    return kSignatureVerificationFailure;
  }

  *size_difference = static_cast<int64_t>(existing_content.size()) -
                     content.size();
  *new_content = content;
  return kSuccess;
}

template <>
int ProcessHas<kModifiableByOwner>(
    const std::string &name,
    const std::string &version,
    const asymm::PublicKey &/*public_key*/,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  if (version.empty()) {
    if (!chunk_store->Has(name)) {
      DLOG(ERROR) << "Failed to find " << Base32Substr(name);
      return kFailedToFindChunk;
    }
  } else {
    std::string existing_version;
    int result(GetContentAndTigerHash(name, chunk_store, nullptr,
                                      &existing_version));
    if (result != kSuccess) {
      DLOG(ERROR) << "Failed to find " << Base32Substr(name);
      return result;
    }
    if (version != existing_version) {
      DLOG(ERROR) << "Failed to find requested version of "
                  << Base32Substr(name);
      return kDifferentVersion;
    }
  }

  return kSuccess;
}

}  // namespace chunk_actions

}  // namespace priv

}  // namespace maidsafe
