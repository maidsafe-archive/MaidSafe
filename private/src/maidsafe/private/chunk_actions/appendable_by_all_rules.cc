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

#include "maidsafe/private/chunk_actions/appendable_by_all_rules.h"

#include "maidsafe/common/utils.h"

#include "maidsafe/private/return_codes.h"
#include "maidsafe/private/log.h"

#include "maidsafe/private/chunk_actions/appendable_by_all_pb.h"
#include "maidsafe/private/chunk_actions/chunk_action_authority.h"
#include "maidsafe/private/chunk_actions/chunk_pb.h"
#include "maidsafe/private/chunk_actions/utils.h"

#include "maidsafe/private/chunk_store/chunk_store.h"

namespace maidsafe {

namespace priv {

namespace chunk_actions {

template <>
bool IsCacheable<kAppendableByAll>() { return false; }

template <>
bool IsModifiable<kAppendableByAll>() { return true; }

template <>
bool DoesModifyReplace<kAppendableByAll>() { return false; }

template <>
bool IsValidChunk<kAppendableByAll>(
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
std::string GetVersion<kAppendableByAll>(
    const std::string &name,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  std::string hash;
  return (GetContentAndTigerHash(name, chunk_store, nullptr,
                                 &hash) == kSuccess ? hash : "");
}

template <>
int ProcessGet<kAppendableByAll>(
    const std::string &name,
    const std::string &/*version*/,
    const asymm::PublicKey &public_key,
    std::string *existing_content,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  existing_content->clear();
  std::string all_existing_content(chunk_store->Get(name));
  if (all_existing_content.empty()) {
    DLOG(ERROR) << "Failed to get " << Base32Substr(name);
    return kFailedToFindChunk;
  }

  AppendableByAll existing_chunk;
  if (!ParseProtobuf<AppendableByAll>(all_existing_content, &existing_chunk)) {
    DLOG(ERROR) << "Failed to get " << Base32Substr(name)
                << ": existing data doesn't parse as AppendableByAll";
    return kGeneralError;
  }

  if (!asymm::ValidateKey(public_key)) {
    DLOG(ERROR) << "Failed to get " << Base32Substr(name)
                << ": invalid public key";
    return kInvalidPublicKey;
  }

  if (asymm::CheckSignature(existing_chunk.allow_others_to_append().data(),
                            existing_chunk.allow_others_to_append().signature(),
                            public_key) == kSuccess) {
    // Owner - return all data
    *existing_content = all_existing_content;
    // and the content in the chunk shall be cleaned up later on via base class
    existing_chunk.clear_appendices();
    std::string with_empty_appendices;
    BOOST_VERIFY(existing_chunk.SerializeToString(&with_empty_appendices));
    if (!chunk_store->Modify(name, with_empty_appendices)) {
      DLOG(ERROR) << "Failed to modify chunk: " << Base32Substr(name);
      return kModifyFailure;
    }
  } else {
    // Not owner - return only first value
    if (!existing_chunk.identity_key().SerializeToString(existing_content)) {
      DLOG(ERROR) << "Failed to serialise: " << Base32Substr(name);
      return kGeneralError;
    }
  }

  return kSuccess;
}

template <>
int ProcessStore<kAppendableByAll>(
    const std::string &name,
    const std::string &content,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  if (chunk_store->Has(name)) {
    DLOG(WARNING) << "Failed to store " << Base32Substr(name)
                  << ": chunk already exists";
    return kKeyNotUnique;
  }

  AppendableByAll chunk;
  if (!ParseProtobuf<AppendableByAll>(content, &chunk)) {
    DLOG(ERROR) << "Failed to store " << Base32Substr(name)
                << ": data doesn't parse as AppendableByAll";
    return kInvalidSignedData;
  }

  if (!asymm::ValidateKey(public_key)) {
    DLOG(ERROR) << "Failed to store " << Base32Substr(name)
                << ": invalid public key";
    return kInvalidPublicKey;
  }

  if (asymm::CheckSignature(chunk.allow_others_to_append().data(),
                            chunk.allow_others_to_append().signature(),
                            public_key) != kSuccess) {
    DLOG(ERROR) << "Failed to store " << Base32Substr(name)
                << ": signature verification failed";
    return kSignatureVerificationFailure;
  }

  return kSuccess;
}

template <>
int ProcessDelete<kAppendableByAll>(
    const std::string &name,
    const std::string &ownership_proof,
    const asymm::PublicKey &public_key,
    std::shared_ptr<chunk_store::ChunkStore> chunk_store) {
  std::string existing_content = chunk_store->Get(name);
  if (existing_content.empty()) {
    DLOG(INFO) << Base32Substr(name) << " already deleted";
    return kSuccess;
  }

  AppendableByAll existing_chunk;
  if (!ParseProtobuf<AppendableByAll>(existing_content, &existing_chunk)) {
    DLOG(ERROR) << "Failed to delete " << Base32Substr(name)
                << ": existing data doesn't parse";
    return kGeneralError;
  }

  if (!asymm::ValidateKey(public_key)) {
    DLOG(ERROR) << "Failed to delete " << Base32Substr(name)
                << ": invalid public key";
    return kInvalidPublicKey;
  }

  if (asymm::CheckSignature(existing_chunk.allow_others_to_append().data(),
                            existing_chunk.allow_others_to_append().signature(),
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
int ProcessModify<kAppendableByAll>(
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

  AppendableByAll existing_chunk;
  if (!ParseProtobuf<AppendableByAll>(existing_content, &existing_chunk)) {
    DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
                << ": existing data doesn't parse as AppendableByAll";
    return kGeneralError;
  }

  if (!asymm::ValidateKey(public_key)) {
    DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
                << ": invalid public key";
    return kInvalidPublicKey;
  }

  bool is_owner(asymm::CheckSignature(
                          existing_chunk.allow_others_to_append().data(),
                          existing_chunk.allow_others_to_append().signature(),
                          public_key) == kSuccess);

  if (is_owner) {
    ModifyAppendableByAll chunk;
    if (!ParseProtobuf<ModifyAppendableByAll>(content, &chunk)) {
      DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
                  << ": data doesn't parse as ModifyAppendableByAll";
      return kParseFailure;
    }

    bool has_allow_others_to_append(chunk.has_allow_others_to_append());
    bool has_identity_key(chunk.has_identity_key());

    // One and only one new_data of identity_key and allow_others_to_append
    // shall be provided via content
    if ((!has_allow_others_to_append) && (!has_identity_key)) {
      DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
                  << ": no new_control_content provided";
      return kInvalidModify;
    }
    if ((has_allow_others_to_append) && (has_identity_key)) {
      DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
                  << ": too much new_control_content provided";
      return kInvalidModify;
    }

    if (has_allow_others_to_append) {
      if (asymm::CheckSignature(chunk.allow_others_to_append().data(),
                                chunk.allow_others_to_append().signature(),
                                public_key) != kSuccess) {
        DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
                    << ": signature verification failed";
        return kSignatureVerificationFailure;
      }

      if (chunk.allow_others_to_append().data() ==
          existing_chunk.allow_others_to_append().data()) {
        // Remove appendices only
        // TODO(qi.ma) the following clear is commented out as it already
        // happened in "processget" (get is guaranteed to happen before modify)
        // existing_chunk.clear_appendices();
        // BOOST_VERIFY(existing_chunk.SerializeToString(new_content));
      } else {
        // Replace field only, leave appendices untouched
        existing_chunk.mutable_allow_others_to_append()->CopyFrom(
            chunk.allow_others_to_append());
        BOOST_VERIFY(existing_chunk.SerializeToString(new_content));
      }
    } else {
      if (asymm::CheckSignature(chunk.identity_key().data(),
                                chunk.identity_key().signature(),
                                public_key) != kSuccess) {
        DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
                    << ": signature verification failed";
        return kSignatureVerificationFailure;
      }
      // Replace field only, leave appendices untouched
      existing_chunk.mutable_identity_key()->CopyFrom(chunk.identity_key());
      BOOST_VERIFY(existing_chunk.SerializeToString(new_content));
    }
  } else {
    char appendability;
    appendability = (existing_chunk.allow_others_to_append().data().c_str()[0]);
    if (appendability == kAppendableByAll) {
      SignedData appendix;
      if (!ParseProtobuf<SignedData>(content, &appendix)) {
        DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
                    << ": data doesn't parse as SignedData";
        return kInvalidSignedData;
      }

      if (asymm::CheckSignature(appendix.data(),
                                appendix.signature(),
                                public_key) != kSuccess) {
        DLOG(ERROR) << "Failed to modify " << Base32Substr(name)
                    << ": signature verification failed";
        return kSignatureVerificationFailure;
      }

      existing_chunk.add_appendices()->CopyFrom(appendix);
      BOOST_VERIFY(existing_chunk.SerializeToString(new_content));
    } else {
      DLOG(INFO) << "Failed to modify " << Base32Substr(name)
                 << ": appending disallowed by owner";
      return kAppendDisallowed;
    }
  }

  *size_difference = static_cast<int64_t>(existing_content.size()) -
                     new_content->size();
  return kSuccess;
}

template <>
int ProcessHas<kAppendableByAll>(
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
