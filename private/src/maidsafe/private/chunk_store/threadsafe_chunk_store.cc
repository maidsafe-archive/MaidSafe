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

#include "maidsafe/private/chunk_store/threadsafe_chunk_store.h"

namespace fs = boost::filesystem;

namespace maidsafe {

namespace priv {

namespace chunk_store {

ThreadsafeChunkStore::ThreadsafeChunkStore(
    std::shared_ptr<ChunkStore> chunk_store)
        : ChunkStore(),
          chunk_store_(chunk_store),
          mutex_() {}

ThreadsafeChunkStore::~ThreadsafeChunkStore() {}

std::string ThreadsafeChunkStore::Get(const std::string &name) const {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Get(name);
}

bool ThreadsafeChunkStore::Get(const std::string &name,
                               const fs::path &sink_file_name) const {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Get(name, sink_file_name);
}

bool ThreadsafeChunkStore::Store(const std::string &name,
                                 const std::string &content) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Store(name, content);
}

bool ThreadsafeChunkStore::Store(const std::string &name,
                                 const fs::path &source_file_name,
                                 bool delete_source_file) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Store(name, source_file_name, delete_source_file);
}

bool ThreadsafeChunkStore::Delete(const std::string &name) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Delete(name);
}

bool ThreadsafeChunkStore::Modify(const std::string &name,
                                  const std::string &content) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Modify(name, content);
}

bool ThreadsafeChunkStore::Modify(const std::string &name,
                                  const fs::path &source_file_name,
                                  bool delete_source_file) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Modify(name, source_file_name, delete_source_file);
}

bool ThreadsafeChunkStore::Has(const std::string &name) const {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Has(name);
}

bool ThreadsafeChunkStore::MoveTo(const std::string &name,
                                  ChunkStore *sink_chunk_store) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->MoveTo(name, sink_chunk_store);
}

uintmax_t ThreadsafeChunkStore::Size(const std::string &name) const {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Size(name);
}

uintmax_t ThreadsafeChunkStore::Size() const {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Size();
}

uintmax_t ThreadsafeChunkStore::Capacity() const {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Capacity();
}

void ThreadsafeChunkStore::SetCapacity(const uintmax_t &capacity) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  chunk_store_->SetCapacity(capacity);
}

bool ThreadsafeChunkStore::Vacant(const uintmax_t &required_size) const {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Vacant(required_size);
}

uintmax_t ThreadsafeChunkStore::Count(const std::string &name) const {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Count(name);
}

uintmax_t ThreadsafeChunkStore::Count() const {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Count();
}

bool ThreadsafeChunkStore::Empty() const {
  boost::lock_guard<boost::mutex> lock(mutex_);
  return chunk_store_->Empty();
}

void ThreadsafeChunkStore::Clear() {
  boost::lock_guard<boost::mutex> lock(mutex_);
  chunk_store_->Clear();
}

}  // namespace chunk_store

}  // namespace priv

}  // namespace maidsafe
