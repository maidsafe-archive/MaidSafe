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

#include "maidsafe/private/chunk_store/memory_chunk_store.h"

#include "maidsafe/common/utils.h"

#include "maidsafe/private/log.h"

namespace maidsafe {

namespace priv {

namespace chunk_store {

MemoryChunkStore::MemoryChunkStore() : ChunkStore(), chunks_() {}

MemoryChunkStore::~MemoryChunkStore() {}

std::string MemoryChunkStore::Get(const std::string &name) const {
  auto it = chunks_.find(name);
  if (it == chunks_.end()) {
    DLOG(ERROR) << "Get - Can't get chunk " << Base32Substr(name);
    return "";
  }

  return (*it).second.second;
}

bool MemoryChunkStore::Get(const std::string &name,
                           const fs::path &sink_file_name) const {
  auto it = chunks_.find(name);
  if (it == chunks_.end()) {
    DLOG(ERROR) << "Get - Can't get chunk " << Base32Substr(name);
    return false;
  }

  return WriteFile(sink_file_name, (*it).second.second);
}

bool MemoryChunkStore::Store(const std::string &name,
                             const std::string &content) {
  if (name.empty()) {
    DLOG(ERROR) << "Store - Empty name passed.";
    return false;
  }

  auto it(chunks_.lower_bound(name));
  if (it != chunks_.end() && (*it).first == name) {
    ++(*it).second.first;
//     DLOG(INFO) << "Store - Increased count of chunk " << Base32Substr(name)
//                << " to " << (*it).second.first;
    return true;
  }

  uintmax_t chunk_size(content.size());
  if (chunk_size == 0) {
    DLOG(ERROR) << "Store - Empty contents passed for " << Base32Substr(name);
    return false;
  }

  if (!Vacant(chunk_size)) {
    DLOG(ERROR) << "Store - Chunk " << Base32Substr(name) << " has size "
                << chunk_size << " > vacant space";
    return false;
  }

  if (!chunks_.empty()) {
    if (it == chunks_.begin())
      it = --chunks_.end();
    else
      --it;
  }
  chunks_.insert(it, std::make_pair(name, ChunkEntry(1, content)));
  IncreaseSize(chunk_size);
//   DLOG(INFO) << "Store - Stored chunk " << Base32Substr(name);
  return true;
}

bool MemoryChunkStore::Store(const std::string &name,
                             const fs::path &source_file_name,
                             bool delete_source_file) {
  if (name.empty()) {
    DLOG(ERROR) << "Store - Empty name passed.";
    return false;
  }

  boost::system::error_code ec;
  auto it = chunks_.find(name);
  if (it == chunks_.end()) {
    uintmax_t chunk_size(fs::file_size(source_file_name, ec));
    if (ec) {
      DLOG(ERROR) << "Store - Failed to calculate size for chunk "
                  << Base32Substr(name) << ": " << ec.message();
      return false;
    }

    if (chunk_size == 0) {
      DLOG(ERROR) << "Store - Chunk " << Base32Substr(name) << " has size 0";
      return false;
    }

    if (!Vacant(chunk_size)) {
      DLOG(ERROR) << "Store - Chunk " << Base32Substr(name) << " has size "
                  << chunk_size << " > vacant space.";
      return false;
    }

    std::string content;
    if (!ReadFile(source_file_name, &content)) {
      DLOG(ERROR) << "Store - Failed to read file for chunk "
                  << Base32Substr(name);
      return false;
    }

    if (content.size() != chunk_size) {
      DLOG(ERROR) << "Store - File content size " << content.size()
                  << " != chunk_size " << chunk_size << " for chunk "
                  << Base32Substr(name);
      return false;
    }

    chunks_[name] = ChunkEntry(1, content);
    IncreaseSize(chunk_size);
//     DLOG(INFO) << "Store - Stored chunk " << Base32Substr(name);
  } else {
    ++(*it).second.first;
//     DLOG(INFO) << "Store - Increased count of chunk " << Base32Substr(name)
//                << " to " << (*it).second.first;
  }

  if (delete_source_file)
    fs::remove(source_file_name, ec);

  return true;
}

bool MemoryChunkStore::Delete(const std::string &name) {
  if (name.empty()) {
    DLOG(ERROR) << "Delete - Empty name passed.";
    return false;
  }

  auto it = chunks_.find(name);
  if (it == chunks_.end()) {
//     DLOG(INFO) << "Delete - Chunk " << Base32Substr(name)
//                << " already deleted";
    return true;
  }

  if (--(*it).second.first == 0) {
    DecreaseSize((*it).second.second.size());
    chunks_.erase(it);
//     DLOG(INFO) << "Delete - Deleted chunk " << Base32Substr(name);
//   } else {
//     DLOG(INFO) << "Delete - Decreased count of chunk " << Base32Substr(name)
//                << " to " << (*it).second.first << " via deletion";
  }

  return true;
}

bool MemoryChunkStore::Modify(const std::string &name,
                              const std::string &content) {
  if (name.empty()) {
    DLOG(ERROR) << "Modify - Empty name passed.";
    return false;
  }

  auto it = chunks_.find(name);
  if (it == chunks_.end())
    return false;

  std::string current_content((*it).second.second);

  uintmax_t content_size_difference;
  bool increase_size(false);
  if (!AssessSpaceRequirement(current_content.size(),
                              content.size(),
                              &increase_size,
                              &content_size_difference)) {
    DLOG(ERROR) << "Size differential unacceptable - increase_size: "
                << increase_size << ", name: " << Base32Substr(name);
    return false;
  }

  chunks_[name] = ChunkEntry((*it).second.first, content);

  AdjustChunkStoreStats(content_size_difference, increase_size);
  return true;
}

bool MemoryChunkStore::Modify(const std::string &name,
                              const fs::path &source_file_name,
                              bool delete_source_file) {
  if (source_file_name.empty()) {
    DLOG(ERROR) << "source_file_name empty: " << Base32Substr(name);
    return false;
  }

  std::string content;
  if (!ReadFile(source_file_name, &content)) {
    DLOG(ERROR) << "Error reading file: " << Base32Substr(name)
                << ", path: " << source_file_name;
    return false;
  }

  if (!Modify(name, content)) {
    DLOG(ERROR) << "Failed to modify: " << Base32Substr(name);
    return false;
  }

  boost::system::error_code ec;
  if (delete_source_file)
    fs::remove(source_file_name, ec);
  return true;
}

bool MemoryChunkStore::Has(const std::string &name) const {
  bool found(chunks_.find(name) != chunks_.end());
//   DLOG(INFO) << (found ? "Have chunk " : "Do not have chunk ")
//              << Base32Substr(name);
  return found;
}

bool MemoryChunkStore::MoveTo(const std::string &name,
                              ChunkStore *sink_chunk_store) {
  if (!sink_chunk_store) {
    DLOG(ERROR) << "MoveTo - NULL sink passed for chunk " << Base32Substr(name);
    return false;
  }

  auto it = chunks_.find(name);
  if (it == chunks_.end()) {
    DLOG(WARNING) << "MoveTo - Failed to find chunk " << Base32Substr(name);
    return false;
  }

  if (!sink_chunk_store->Store(name, (*it).second.second)) {
    DLOG(ERROR) << "MoveTo - Failed to store chunk " << Base32Substr(name)
                << " in sink";
    return false;
  }

  if (--(*it).second.first == 0) {
    DecreaseSize((*it).second.second.size());
    chunks_.erase(it);
    DLOG(INFO) << "MoveTo - Moved chunk " << Base32Substr(name);
  } else {
    DLOG(INFO) << "MoveTo - Decreased count of chunk " << Base32Substr(name)
               << " to " << (*it).second.first << " via move";
  }

  return true;
}

uintmax_t MemoryChunkStore::Size(const std::string &name) const {
  auto it = chunks_.find(name);
  if (it == chunks_.end()) {
    DLOG(ERROR) << "Chunk not found: " << Base32Substr(name);
    return 0;
  }

  return (*it).second.second.size();
}

uintmax_t MemoryChunkStore::Count(const std::string &name) const {
  auto it = chunks_.find(name);
  if (it == chunks_.end()) {
    DLOG(ERROR) << "Chunk not found: " << Base32Substr(name);
    return 0;
  }

  return (*it).second.first;
}

uintmax_t MemoryChunkStore::Count() const {
  return chunks_.size();
}

bool MemoryChunkStore::Empty() const {
  return chunks_.empty();
}

void MemoryChunkStore::Clear() {
  chunks_.clear();
  ChunkStore::Clear();
}

}  // namespace chunk_store

}  // namespace priv

}  // namespace maidsafe
