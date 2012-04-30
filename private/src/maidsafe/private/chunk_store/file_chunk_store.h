/* Copyright (c) 2011 maidsafe.net limited
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MAIDSAFE_PRIVATE_CHUNK_STORE_FILE_CHUNK_STORE_H_
#define MAIDSAFE_PRIVATE_CHUNK_STORE_FILE_CHUNK_STORE_H_

#include <string>
#include <utility>

#ifdef __MSVC__
#  pragma warning(push, 1)
#  pragma warning(disable: 4127 4250)
#endif

#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"

#ifdef __MSVC__
#  pragma warning(pop)
#endif

#include "maidsafe/private/chunk_store/chunk_store.h"

namespace fs = boost::filesystem;

namespace maidsafe {

namespace priv {

namespace chunk_store {

namespace test {
class FileChunkStoreTest_BEH_Methods_Test;
}  // namespace test

class FileChunkStore : public ChunkStore {
 public:
  FileChunkStore();
  ~FileChunkStore();
  // Initialises the chunk storage directory.  If the given directory path does
  // not exist, it will be created.  Returns true if directory exists or could
  // be created.
  bool Init(const fs::path &storage_location, unsigned int dir_depth = 5U);
  std::string Get(const std::string &name) const;
  bool Get(const std::string &name, const fs::path &sink_file_name) const;
  bool Store(const std::string &name, const std::string &content);
  bool Store(const std::string &name,
             const fs::path &source_file_name,
             bool delete_source_file);
  bool Delete(const std::string &name);
  bool Modify(const std::string &name, const std::string &content);
  bool Modify(const std::string &name,
              const fs::path &source_file_name,
              bool delete_source_file);
  bool Has(const std::string &name) const;
  bool MoveTo(const std::string &name, ChunkStore *sink_chunk_store);
  uintmax_t Size(const std::string &name) const;
  uintmax_t Size() const { return ChunkStore::Size(); }
  uintmax_t Count(const std::string &name) const;
  uintmax_t Count() const;
  bool Empty() const;
  void Clear();
  friend class test::FileChunkStoreTest_BEH_Methods_Test;

 private:
  typedef std::pair<uintmax_t, uintmax_t> RestoredChunkStoreInfo;

  FileChunkStore(const FileChunkStore&);
  FileChunkStore& operator=(const FileChunkStore&);

  // Generates sub-dirs based on chunk-name and dir_depth_ specified.  Returns
  // the absolute file path after encoding the chunk name to base 32.
  fs::path ChunkNameToFilePath(const std::string &chunk_name,
                               bool generate_dirs = false) const;
  void IncreaseChunkCount() { ++chunk_count_; }
  void DecreaseChunkCount() { --chunk_count_; }
  void ChunkAdded(const uintmax_t &delta);
  void ChunkRemoved(const uintmax_t &delta);
  void ResetChunkCount(uintmax_t chunk_count = 0) {
    chunk_count_ = chunk_count;
  }
  // Tries to read the ChunkStore info file in dir specified and gets total
  // number of chunks and their collective size
  RestoredChunkStoreInfo RetrieveChunkInfo(const fs::path &location) const;
  // Saves the current state of the ChunkStore (in terms of total number of
  // chunks and their collective size) to the info file
  void SaveChunkStoreState();
  bool IsChunkStoreInitialised() const { return initialised_; }
  uintmax_t GetChunkReferenceCount(const fs::path &) const;
  uintmax_t GetNumFromString(const std::string &) const;

  bool initialised_;
  fs::path storage_location_;
  uintmax_t chunk_count_;
  unsigned int dir_depth_;
  fs::fstream info_file_;
};

}  //  namespace chunk_store

}  //  namespace priv

}  //  namespace maidsafe

#endif  // MAIDSAFE_PRIVATE_CHUNK_STORE_FILE_CHUNK_STORE_H_
