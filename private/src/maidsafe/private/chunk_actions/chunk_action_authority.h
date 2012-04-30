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

#ifndef MAIDSAFE_PRIVATE_CHUNK_ACTIONS_CHUNK_ACTION_AUTHORITY_H_
#define MAIDSAFE_PRIVATE_CHUNK_ACTIONS_CHUNK_ACTION_AUTHORITY_H_

#include <map>
#include <memory>
#include <string>

#include "boost/filesystem.hpp"
#include "boost/signals2/signal.hpp"

#include "maidsafe/common/rsa.h"
#include "maidsafe/private/version.h"

#if MAIDSAFE_PRIVATE_VERSION != 300
#  error This API is not compatible with the installed library.\
    Please update the library.
#endif

namespace bs2 = boost::signals2;
namespace fs = boost::filesystem;


namespace maidsafe {

namespace priv {

namespace chunk_store { class ChunkStore; }

namespace chunk_actions {

namespace test {
class ChunkActionAuthorityTest;
class ChunkActionAuthorityTest_BEH_ValidStore_Test;
class ChunkActionAuthorityTest_BEH_ValidGet_Test;
class ChunkActionAuthorityTest_BEH_ValidDelete_Test;
class ChunkActionAuthorityTest_BEH_ValidModify_Test;
}  // namespace test

std::string ApplyTypeToName(const std::string &name, unsigned char chunk_type);
std::string RemoveTypeFromName(const std::string &name);
unsigned char GetDataType(const std::string &name);

class ChunkActionAuthority {
 public:
  explicit ChunkActionAuthority(
      std::shared_ptr<chunk_store::ChunkStore> chunk_store);
  virtual ~ChunkActionAuthority();

  std::string Get(const std::string &name,
                  const std::string &version,
                  const asymm::PublicKey &public_key) const;
  // Retrieves a chunk's content as a file, potentially overwriting an existing
  // file of the same name.
  bool Get(const std::string &name,
           const fs::path &sink_file_name,
           const std::string &version,
           const asymm::PublicKey &public_key) const;
  bool Store(const std::string &name,
             const std::string &content,
             const asymm::PublicKey &public_key);
  bool Store(const std::string &name,
             const fs::path &source_file_name,
             bool delete_source_file,
             const asymm::PublicKey &public_key);
  // Returns true if chunk deleted or non-existant
  bool Delete(const std::string &name,
              const std::string &ownership_proof,
              const asymm::PublicKey &public_key);
  bool Modify(const std::string &name,
              const std::string &content,
              const asymm::PublicKey &public_key,
              int64_t *size_difference);
  bool Modify(const std::string &name,
              const fs::path &source_file_name,
              bool delete_source_file,
              const asymm::PublicKey &public_key,
              int64_t *size_difference);
  bool Has(const std::string &name,
           const std::string &version,
           const asymm::PublicKey &public_key) const;

  bool ValidName(const std::string &name) const;
  bool Cacheable(const std::string &name) const;
  bool Modifiable(const std::string &name) const;
  bool ModifyReplaces(const std::string &name) const;
  bool ValidChunk(const std::string &name) const;
  std::string Version(const std::string &name) const;

  friend class test::ChunkActionAuthorityTest;
  friend class test::ChunkActionAuthorityTest_BEH_ValidStore_Test;
  friend class test::ChunkActionAuthorityTest_BEH_ValidGet_Test;
  friend class test::ChunkActionAuthorityTest_BEH_ValidDelete_Test;
  friend class test::ChunkActionAuthorityTest_BEH_ValidModify_Test;

 private:
  ChunkActionAuthority &operator=(const ChunkActionAuthority&);
  ChunkActionAuthority(const ChunkActionAuthority&);

  int ValidGet(const std::string &name,
               const std::string &version,
               const asymm::PublicKey &public_key,
               std::string *existing_content = NULL) const;
  int ValidStore(const std::string &name,
                 const std::string &content,
                 const asymm::PublicKey &public_key) const;
  int ValidDelete(const std::string &name,
                  const std::string &ownership_proof,
                  const asymm::PublicKey &public_key) const;
  int ValidModify(const std::string &name,
                  const std::string &content,
                  const asymm::PublicKey &public_key,
                  int64_t *size_difference,
                  std::string *new_content = NULL) const;
  virtual int ValidHas(const std::string &name,
                       const std::string &version,
                       const asymm::PublicKey &public_key) const;

  std::shared_ptr<chunk_store::ChunkStore> chunk_store_;
};

}  // namespace chunk_actions

}  // namespace priv

}  // namespace maidsafe

#endif  // MAIDSAFE_PRIVATE_CHUNK_ACTIONS_CHUNK_ACTION_AUTHORITY_H_
