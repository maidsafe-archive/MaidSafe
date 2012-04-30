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

#ifndef MAIDSAFE_PRIVATE_CHUNK_STORE_MEMORY_CHUNK_STORE_H_
#define MAIDSAFE_PRIVATE_CHUNK_STORE_MEMORY_CHUNK_STORE_H_

#include <map>
#include <memory>
#include <string>
#include <utility>

#ifdef __MSVC__
#  pragma warning(push, 1)
#  pragma warning(disable: 4127)
#endif

#include "boost/filesystem/path.hpp"

#ifdef __MSVC__
#  pragma warning(pop)
#endif

#include "maidsafe/private/chunk_store/chunk_store.h"

namespace fs = boost::filesystem;

namespace maidsafe {

namespace priv {

namespace chunk_store {

class MemoryChunkStore : public ChunkStore {
 public:
  MemoryChunkStore();
  ~MemoryChunkStore();
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

 private:
  typedef std::pair<uintmax_t, std::string> ChunkEntry;
  MemoryChunkStore(const MemoryChunkStore&);
  MemoryChunkStore& operator=(const MemoryChunkStore&);
  std::map<std::string, ChunkEntry> chunks_;
};

}  //  namespace chunk_store

}  //  namespace priv

}  //  namespace maidsafe

#endif  // MAIDSAFE_PRIVATE_CHUNK_STORE_MEMORY_CHUNK_STORE_H_
