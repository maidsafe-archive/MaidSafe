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

#include "maidsafe/private/chunk_actions/signature_packet_rules.h"

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

const std::string kRevokedSignaturePacket("0");

template <>
bool IsCacheable<kSignaturePacket>() { return false; }

template <>
bool IsModifiable<kSignaturePacket>() { return false; }

template <>
bool DoesModifyReplace<kSignaturePacket>() { return false; }

template <>
bool IsValidChunk<kSignaturePacket>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  std::string existing_data(chunk_store->Get(name));
  if (existing_data.empty()) {
    DLOG(ERROR) << "Failed to get " << Base32Substr(name) << " for validation";
    return false;
  }

  SignedData existing_chunk;
  if (!ParseProtobuf<SignedData>(existing_data, &existing_chunk)) {
    DLOG(ERROR) << "Failed to validate " << Base32Substr(name)
                << ": existing data doesn't parse as a SignedData";
    return false;
  }

  if (crypto::Hash<crypto::SHA512>(
          existing_chunk.data() + existing_chunk.signature()) !=
      RemoveTypeFromName(name)) {
    DLOG(ERROR) << "Failed to validate " << Base32Substr(name)
                << ": chunk isn't hashable";
    return false;
  }

  return true;
}

template <>
std::string GetVersion<kSignaturePacket>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> /*chunk_store*/) {
  return name.substr(0, crypto::Tiger::DIGESTSIZE);
}

template <>
int ProcessGet<kSignaturePacket>(
    const std::string &name,
    const std::string &/*version*/,
    const asymm::PublicKey &/*public_key*/,
    std::string *existing_content,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  *existing_content = chunk_store->Get(name);
  if (existing_content->empty()) {
    DLOG(ERROR) << "Failed to get " << Base32Substr(name);
    return kFailedToFindChunk;
  }

  return kSuccess;
}

template <>
int ProcessStore<kSignaturePacket>(
    const std::string &name,
    const std::string &content,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  if (chunk_store->Has(name)) {
    DLOG(ERROR) << "Failed to store " << Base32Substr(name)
                << ": chunk already exists";
    return kKeyNotUnique;
  }

  SignedData chunk;
  if (!ParseProtobuf<SignedData>(content, &chunk)) {
    DLOG(ERROR) << "Failed to store " << Base32Substr(name)
                << ": data doesn't parse as a SignedData";
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

  if (crypto::Hash<crypto::SHA512>(chunk.data() + chunk.signature()) !=
      RemoveTypeFromName(name)) {
    DLOG(ERROR) << "Failed to validate " << Base32Substr(name)
                << ": chunk isn't hashable";
    return kNotHashable;
  }

  return kSuccess;
}

template <>
int ProcessDelete<kSignaturePacket>(
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

  if (asymm::CheckSignature(existing_chunk.data(),
                            existing_chunk.signature(),
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
int ProcessModify<kSignaturePacket>(
    const std::string &name,
    const std::string &/*content*/,
    const asymm::PublicKey &/*public_key*/,
    int64_t * /*size_difference*/,
    std::string * /*new_content*/,
    std::shared_ptr<chunk_store::ChunkStore> /*chunk_store*/) {
  DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
              << ": no modify of SignedData allowed";
  return kInvalidModify;
}

template <>
int ProcessHas<kSignaturePacket>(
    const std::string &name,
    const std::string &/*version*/,
    const asymm::PublicKey &/*public_key*/,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  if (!chunk_store->Has(name)) {
    DLOG(ERROR) << "Failed to find " << Base32Substr(name);
    return kFailedToFindChunk;
  }

  return kSuccess;
}

}  // namespace chunk_actions

}  // namespace priv

}  // namespace maidsafe
