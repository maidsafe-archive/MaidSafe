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

#include "maidsafe/private/chunk_store/buffered_chunk_store.h"

#include "maidsafe/common/utils.h"

#include "maidsafe/private/log.h"
#include "maidsafe/private/chunk_store/file_chunk_store.h"
#include "maidsafe/private/chunk_store/memory_chunk_store.h"
#include "maidsafe/private/chunk_store/threadsafe_chunk_store.h"

namespace maidsafe {

namespace priv {

namespace chunk_store {

// If the cache is full and there are no more chunks left to delete, this is the
// number of chunk transfers to wait for (in Store) before the next check.
const int kWaitTransfersForCacheVacantCheck(10);
const boost::posix_time::seconds kXferWaitTimeout(3);

BufferedChunkStore::BufferedChunkStore(boost::asio::io_service &asio_service)  // NOLINT (Fraser)
    : ChunkStore(),
      cache_mutex_(),
      xfer_mutex_(),
      xfer_cond_var_(),
      asio_service_(asio_service),
      internal_perm_chunk_store_(new FileChunkStore),
      cache_chunk_store_(new MemoryChunkStore),
      perm_chunk_store_(new ThreadsafeChunkStore(internal_perm_chunk_store_)),
      cached_chunks_(),
      removable_chunks_(),
      pending_xfers_(),
      perm_capacity_(0),
      perm_size_(0),
      initialised_(false) {}

BufferedChunkStore::~BufferedChunkStore() {
  boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
  if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
        return pending_xfers_.empty() || asio_service_.stopped();
      })) {
    DLOG(ERROR) << "~BufferedChunkStore - Timed out.";
  }
}

bool BufferedChunkStore::Init(const fs::path &storage_location,
                              std::list<std::string> removable_chunks,
                              unsigned int dir_depth) {
  if (!reinterpret_cast<FileChunkStore*>(
        internal_perm_chunk_store_.get())->Init(storage_location, dir_depth)) {
    DLOG(ERROR) << "Failed to initialise internal permanent chunk store.";
    return false;
  }

  perm_capacity_ = internal_perm_chunk_store_->Capacity();
  perm_size_ = internal_perm_chunk_store_->Size();
  removable_chunks_ = removable_chunks;
  initialised_ = true;
  return true;
}

std::string BufferedChunkStore::Get(const std::string &name) const {
  if (name.empty()) {
    DLOG(ERROR) << "Get - Empty name passed.";
    return "";
  }

  {
    boost::lock_guard<boost::mutex> lock(cache_mutex_);
    if (cache_chunk_store_->Has(name)) {
      auto it = std::find(cached_chunks_.begin(), cached_chunks_.end(), name);
      if (it != cached_chunks_.end()) {
        cached_chunks_.erase(it);
        cached_chunks_.push_front(name);
      }
      return cache_chunk_store_->Get(name);
    }
  }

  std::string content(perm_chunk_store_->Get(name));
  if (!content.empty() && DoCacheStore(name, content))
    AddCachedChunksEntry(name);
  return content;
}

bool BufferedChunkStore::Get(const std::string &name,
                             const fs::path &sink_file_name) const {
  if (name.empty()) {
    DLOG(ERROR) << "Get - Empty name passed.";
    return false;
  }

  {
    boost::lock_guard<boost::mutex> lock(cache_mutex_);
    if (cache_chunk_store_->Has(name)) {
      auto it = std::find(cached_chunks_.begin(), cached_chunks_.end(), name);
      if (it != cached_chunks_.end()) {
        cached_chunks_.erase(it);
        cached_chunks_.push_front(name);
      }
      return cache_chunk_store_->Get(name, sink_file_name);
    }
  }

  std::string content(perm_chunk_store_->Get(name));
  if (!content.empty() && DoCacheStore(name, content))
    AddCachedChunksEntry(name);
  return !content.empty() && WriteFile(sink_file_name, content);
}

bool BufferedChunkStore::Store(const std::string &name,
                               const std::string &content) {
  if (name.empty()) {
    DLOG(ERROR) << "Store - Empty name passed.";
    return false;
  }

  if (!DoCacheStore(name, content)) {
    DLOG(ERROR) << "Failed to cache: " << Base32Substr(name);
    return false;
  }

  if (!MakeChunkPermanent(name, content.size())) {
    // AddCachedChunksEntry(name);
    boost::lock_guard<boost::mutex> lock(cache_mutex_);
    cache_chunk_store_->Delete(name);
    DLOG(ERROR) << "Failed to make chunk permanent: " << Base32Substr(name);
    return false;
  }

  return true;
}

bool BufferedChunkStore::Store(const std::string &name,
                               const fs::path &source_file_name,
                               bool delete_source_file) {
  if (name.empty()) {
    DLOG(ERROR) << "Store - Empty name passed.";
    return false;
  }

  boost::system::error_code ec;
  uintmax_t size(fs::file_size(source_file_name, ec));

  if (!DoCacheStore(name, size, source_file_name, false)) {
    DLOG(ERROR) << "Failed to cache: " << Base32Substr(name);
    return false;
  }

  if (!MakeChunkPermanent(name, size)) {
    // AddCachedChunksEntry(name);
    boost::lock_guard<boost::mutex> lock(cache_mutex_);
    cache_chunk_store_->Delete(name);
    DLOG(ERROR) << "Failed to make chunk permanent: " << Base32Substr(name);
    return false;
  }

  if (delete_source_file)
    fs::remove(source_file_name, ec);

  return true;
}

bool BufferedChunkStore::CacheStore(const std::string &name,
                                    const std::string &content) {
  if (name.empty()) {
    DLOG(ERROR) << "CacheStore - Empty name passed.";
    return false;
  }

  if (!DoCacheStore(name, content)) {
    DLOG(ERROR) << "Failed to cache: " << Base32Substr(name);
    return false;
  }

  AddCachedChunksEntry(name);
  return true;
}

bool BufferedChunkStore::CacheStore(const std::string &name,
                                    const fs::path &source_file_name,
                                    bool delete_source_file) {
  if (name.empty()) {
    DLOG(ERROR) << "CacheStore - Empty name passed.";
    return false;
  }

  boost::system::error_code ec;
  uintmax_t size(fs::file_size(source_file_name, ec));

  if (!DoCacheStore(name, size, source_file_name, false)) {
    DLOG(ERROR) << "Failed to cache: " << Base32Substr(name);
    return false;
  }

  AddCachedChunksEntry(name);
  if (delete_source_file)
    fs::remove(source_file_name, ec);

  return true;
}

bool BufferedChunkStore::PermanentStore(const std::string &name) {
  if (name.empty()) {
    DLOG(ERROR) << "PermanentStore - Empty name passed.";
    return false;
  }

  std::string content;
  {
    boost::lock_guard<boost::mutex> lock(cache_mutex_);
    content = cache_chunk_store_->Get(name);
  }

  {
    boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
    RemoveDeletionMarks(name);
    if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
          return pending_xfers_.find(name) == pending_xfers_.end();
        })) {
      DLOG(ERROR) << "PermanentStore - Timed out storing " << Base32Substr(name)
                  << " while waiting for pending transfers.";
      return false;
    }
    if (perm_chunk_store_->Has(name))
      return true;
    if (content.empty() || !perm_chunk_store_->Store(name, content)) {
      DLOG(ERROR) << "PermanentStore - Could not transfer "
                  << Base32Substr(name);
      return false;
    }
    perm_size_ = perm_chunk_store_->Size();
  }

  return true;
}

bool BufferedChunkStore::Delete(const std::string &name) {
  if (name.empty()) {
    DLOG(ERROR) << "Delete - Empty name passed.";
    return false;
  }

  bool file_delete_result(false);
  {
    boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
    if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
          return pending_xfers_.find(name) == pending_xfers_.end();
        })) {
      DLOG(ERROR) << "Delete - Timed out deleting " << Base32Substr(name)
                  << " while waiting for pending transfers.";
      return false;
    }
    file_delete_result = perm_chunk_store_->Delete(name);
    perm_size_ = perm_chunk_store_->Size();
  }

  if (!file_delete_result)
    DLOG(ERROR) << "Delete - Could not delete " << Base32Substr(name);

  boost::lock_guard<boost::mutex> lock(cache_mutex_);
  auto it = std::find(cached_chunks_.begin(), cached_chunks_.end(), name);
  if (it != cached_chunks_.end())
    cached_chunks_.erase(it);
  cache_chunk_store_->Delete(name);

  return file_delete_result;
}

bool BufferedChunkStore::Modify(const std::string &name,
                                const std::string &content) {
  if (name.empty()) {
    DLOG(ERROR) << "Modify - Empty name passed.";
    return false;
  }

  boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
  RemoveDeletionMarks(name);

  if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
        return pending_xfers_.find(name) == pending_xfers_.end();
      })) {
    DLOG(ERROR) << "Modify - Timed out modifying " << Base32Substr(name)
                << " while waiting for pending transfers.";
    return false;
  }

  if (perm_chunk_store_->Has(name)) {
    std::string current_perm_content(perm_chunk_store_->Get(name));
    uintmax_t content_size_difference(0);
    bool increase_size(false);
    if (content.size() > current_perm_content.size()) {
      content_size_difference = content.size() - current_perm_content.size();
      increase_size = true;
      if (perm_capacity_ > 0) {  // Check if Perm Chunk Store Size is Infinite
        // Wait For Space in Perm Store
        while (perm_size_ + content_size_difference > perm_capacity_) {
          if (removable_chunks_.empty()) {
            DLOG(ERROR) << "Modify - Can't make space for changes to "
                        << Base32Substr(name);
            return false;
          }
          if (perm_chunk_store_->Delete(removable_chunks_.front()))
            perm_size_ = perm_chunk_store_->Size();
          removable_chunks_.pop_front();
        }
      }
    } else {
      content_size_difference = current_perm_content.size() - content.size();
      increase_size = false;
    }
    if (perm_chunk_store_->Modify(name, content)) {
      if (increase_size)
        perm_size_ += content_size_difference;
      else
        perm_size_ -= content_size_difference;
      {
        boost::lock_guard<boost::mutex> lock(cache_mutex_);
        auto it = std::find(cached_chunks_.begin(), cached_chunks_.end(), name);
        if (it != cached_chunks_.end()) {
          cached_chunks_.erase(it);
          cache_chunk_store_->Delete(name);
        }
      }
      return true;
    } else {
      DLOG(ERROR) << "Modify - Couldn't modify " << Base32Substr(name);
      return false;
    }
  } else {
    std::string current_cache_content;
    {
      boost::mutex::scoped_lock lock(cache_mutex_);
      if (!cache_chunk_store_->Has(name)) {
        DLOG(ERROR) << "Modify - Don't have chunk " << Base32Substr(name);
        return false;
      }

      current_cache_content = cache_chunk_store_->Get(name);
      uintmax_t content_size_difference(0);
      if (content.size() > current_cache_content.size()) {
        content_size_difference = content.size() - current_cache_content.size();
        // Make space in Cache if Needed
        while (!cache_chunk_store_->Vacant(content_size_difference)) {
          if (cached_chunks_.empty()) {
            lock.unlock();
            if (pending_xfers_.empty()) {
              DLOG(ERROR) << "Modify - Can't make space for changes to "
                          << Base32Substr(name);
              return false;
            }

            int limit(kWaitTransfersForCacheVacantCheck);
            if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
                  return pending_xfers_.empty() || (--limit) == 0;
                })) {
              DLOG(ERROR) << "Modify - Timed out modifying "
                          << Base32Substr(name)
                          << " while waiting for pending transfers.";
              return false;
            }
            lock.lock();
          }
          cache_chunk_store_->Delete(cached_chunks_.back());
          cached_chunks_.pop_back();
        }
      }
      return cache_chunk_store_->Modify(name, content);
    }
  }
}

bool BufferedChunkStore::Modify(const std::string &name,
                                const fs::path &source_file_name,
                                bool delete_source_file) {
  if (source_file_name.empty()) {
    DLOG(ERROR) << "Modify - No source file passed for " << Base32Substr(name);
    return false;
  }

  // TODO(Steve) implement optimized Modify for changes from file

  std::string content;
  if (!ReadFile(source_file_name, &content)) {
    DLOG(ERROR) << "Modify - Couldn't read source file for "
                << Base32Substr(name);
    return false;
  }

  if (!Modify(name, content)) {
    DLOG(ERROR) << "Modify - Couldn't modify " << Base32Substr(name);
    return false;
  }

  boost::system::error_code ec;
  if (delete_source_file)
    fs::remove(source_file_name, ec);
  return true;
}

bool BufferedChunkStore::Has(const std::string &name) const {
  return CacheHas(name) || PermanentHas(name);
}

bool BufferedChunkStore::MoveTo(const std::string &name,
                                ChunkStore *sink_chunk_store) {
  if (name.empty()) {
    DLOG(ERROR) << "MoveTo - Empty name passed.";
    return false;
  }

  bool chunk_moved(false);
  {
    boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
    if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
          return pending_xfers_.find(name) == pending_xfers_.end();
        })) {
      DLOG(ERROR) << "MoveTo - Timed out moving " << Base32Substr(name)
                  << " while waiting for pending transfers.";
      return false;
    }
    chunk_moved = perm_chunk_store_->MoveTo(name, sink_chunk_store);
    perm_size_ = perm_chunk_store_->Size();
  }

  if (!chunk_moved) {
    DLOG(ERROR) << "MoveTo - Could not move " << Base32Substr(name);
    return false;
  }

  boost::lock_guard<boost::mutex> lock(cache_mutex_);
  auto it = std::find(cached_chunks_.begin(), cached_chunks_.end(), name);
  if (it != cached_chunks_.end())
    cached_chunks_.erase(it);
  cache_chunk_store_->Delete(name);

  return true;
}

bool BufferedChunkStore::CacheHas(const std::string &name) const {
  if (name.empty()) {
    DLOG(ERROR) << "CacheHas - Empty name passed.";
    return false;
  }

  boost::lock_guard<boost::mutex> lock(cache_mutex_);
  return cache_chunk_store_->Has(name);
}

bool BufferedChunkStore::PermanentHas(const std::string &name) const {
  if (name.empty()) {
    DLOG(ERROR) << "PermanentHas - Empty name passed.";
    return false;
  }

  boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
  if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
        return pending_xfers_.find(name) == pending_xfers_.end();
      })) {
    DLOG(ERROR) << "PermanentHas - Timed out for " << Base32Substr(name)
                << " while waiting for pending transfers.";
    return false;
  }
  uintmax_t rem_count(0);
  for (auto it = removable_chunks_.begin(); it != removable_chunks_.end(); ++it)
    if (*it == name)
      ++rem_count;
  return perm_chunk_store_->Count(name) > rem_count;
}

uintmax_t BufferedChunkStore::Size(const std::string &name) const {
  if (name.empty()) {
    DLOG(ERROR) << "Size - Empty name passed.";
    return 0;
  }

  {
    boost::lock_guard<boost::mutex> lock(cache_mutex_);
    if (cache_chunk_store_->Has(name))
      return cache_chunk_store_->Size(name);
  }
  return perm_chunk_store_->Size(name);
}

uintmax_t BufferedChunkStore::Size() const {
  boost::lock_guard<boost::mutex> lock(xfer_mutex_);
  return perm_size_;
}

uintmax_t BufferedChunkStore::CacheSize() const {
  boost::lock_guard<boost::mutex> lock(cache_mutex_);
  return cache_chunk_store_->Size();
}

uintmax_t BufferedChunkStore::Capacity() const {
  boost::lock_guard<boost::mutex> lock(xfer_mutex_);
  return perm_capacity_;
}

uintmax_t BufferedChunkStore::CacheCapacity() const {
  boost::lock_guard<boost::mutex> lock(cache_mutex_);
  return cache_chunk_store_->Capacity();
}

void BufferedChunkStore::SetCapacity(const uintmax_t &capacity) {
  boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
  if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
        return pending_xfers_.empty();
      })) {
    DLOG(ERROR) << "SetCapacity - Timed out waiting for pending transfers.";
    return;
  }
  perm_chunk_store_->SetCapacity(capacity);
  perm_capacity_ = perm_chunk_store_->Capacity();
}

void BufferedChunkStore::SetCacheCapacity(const uintmax_t &capacity) {
  boost::lock_guard<boost::mutex> lock(cache_mutex_);
  cache_chunk_store_->SetCapacity(capacity);
}

bool BufferedChunkStore::Vacant(const uintmax_t &required_size) const {
  boost::lock_guard<boost::mutex> lock(xfer_mutex_);
  return perm_capacity_ == 0 || perm_size_ + required_size <= perm_capacity_;
}

bool BufferedChunkStore::CacheVacant(
    const uintmax_t &required_size) const {
  boost::lock_guard<boost::mutex> lock(cache_mutex_);
  return cache_chunk_store_->Vacant(required_size);
}

uintmax_t BufferedChunkStore::Count(const std::string &name) const {
  if (name.empty()) {
    DLOG(ERROR) << "Count - Empty name passed.";
    return 0;
  }

  boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
  if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
        return pending_xfers_.find(name) == pending_xfers_.end();
      })) {
    DLOG(ERROR) << "Count - Timed out for " << Base32Substr(name)
                << " while waiting for pending transfers.";
    return false;
  }
  return perm_chunk_store_->Count(name);
}

uintmax_t BufferedChunkStore::Count() const {
  boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
  if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
        return pending_xfers_.empty();
      })) {
    DLOG(ERROR) << "Count - Timed out waiting for pending transfers.";
    return false;
  }
  return perm_chunk_store_->Count();
}

uintmax_t BufferedChunkStore::CacheCount() const {
  boost::lock_guard<boost::mutex> lock(cache_mutex_);
  return cache_chunk_store_->Count();
}

bool BufferedChunkStore::Empty() const {
  return CacheEmpty() && perm_chunk_store_->Empty();
}

bool BufferedChunkStore::CacheEmpty() const {
  boost::lock_guard<boost::mutex> lock(cache_mutex_);
  return cache_chunk_store_->Empty();
}

void BufferedChunkStore::Clear() {
  boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
  if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
        return pending_xfers_.empty();
      })) {
    DLOG(ERROR) << "Clear - Timed out waiting for pending transfers.";
    return;
  }
  boost::lock_guard<boost::mutex> lock(cache_mutex_);
  cached_chunks_.clear();
  removable_chunks_.clear();
  cache_chunk_store_->Clear();
  perm_chunk_store_->Clear();
  perm_capacity_ = perm_chunk_store_->Capacity();
  perm_size_ = 0;
}

void BufferedChunkStore::CacheClear() {
  boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
  if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
        return pending_xfers_.empty();
      })) {
    DLOG(ERROR) << "CacheClear - Timed out waiting for pending transfers.";
    return;
  }
  boost::lock_guard<boost::mutex> lock(cache_mutex_);
  cached_chunks_.clear();
  cache_chunk_store_->Clear();
}

void BufferedChunkStore::MarkForDeletion(const std::string &name) {
  if (name.empty())
    return;
  boost::lock_guard<boost::mutex> lock(xfer_mutex_);
  removable_chunks_.push_back(name);
}

/// @note Ensure cache mutex is not locked.
void BufferedChunkStore::AddCachedChunksEntry(const std::string &name) const {
  if (name.empty())
    return;
  boost::lock_guard<boost::mutex> lock(cache_mutex_);
  auto it = std::find(cached_chunks_.begin(), cached_chunks_.end(), name);
  if (it != cached_chunks_.end())
    cached_chunks_.erase(it);
  cached_chunks_.push_front(name);
}

bool BufferedChunkStore::DoCacheStore(const std::string &name,
                                      const std::string &content) const {
  boost::mutex::scoped_lock lock(cache_mutex_);
  if (cache_chunk_store_->Has(name))
    return true;

  // Check whether cache has capacity to store chunk
  if (content.size() > cache_chunk_store_->Capacity() &&
      cache_chunk_store_->Capacity() > 0) {
    DLOG(ERROR) << "DoCacheStore - Chunk " << Base32Substr(name) << " too big ("
                << BytesToBinarySiUnits(content.size()) << " vs. "
                << BytesToBinarySiUnits(cache_chunk_store_->Capacity()) << ").";
    return false;
  }

  // Make space in cache
  while (!cache_chunk_store_->Vacant(content.size())) {
    while (cached_chunks_.empty()) {
      lock.unlock();
      {
        boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
        if (pending_xfers_.empty()) {
          DLOG(ERROR) << "DoCacheStore - Can't make space for "
                      << Base32Substr(name);
          return false;
        }
        int limit(kWaitTransfersForCacheVacantCheck);
        if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
              return pending_xfers_.empty() || (--limit) == 0;
            })) {
          DLOG(ERROR) << "DoCacheStore - Timed out for " << Base32Substr(name)
                      << " while waiting for pending transfers.";
          return false;
        }
      }
      lock.lock();
    }
    cache_chunk_store_->Delete(cached_chunks_.back());
    cached_chunks_.pop_back();
  }

  return cache_chunk_store_->Store(name, content);
}

bool BufferedChunkStore::DoCacheStore(const std::string &name,
                                      const uintmax_t &size,
                                      const fs::path &source_file_name,
                                      bool delete_source_file) const {
  boost::mutex::scoped_lock lock(cache_mutex_);
  if (cache_chunk_store_->Has(name))
    return true;

  // Check whether cache has capacity to store chunk
  if (size > cache_chunk_store_->Capacity() &&
      cache_chunk_store_->Capacity() > 0) {
    DLOG(ERROR) << "DoCacheStore - Chunk " << Base32Substr(name) << " too big ("
                << BytesToBinarySiUnits(size) << " vs. "
                << BytesToBinarySiUnits(cache_chunk_store_->Capacity()) << ").";
    return false;
  }

  // Make space in cache
  while (!cache_chunk_store_->Vacant(size)) {
    while (cached_chunks_.empty()) {
      lock.unlock();
      {
        boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
        if (pending_xfers_.empty()) {
          DLOG(ERROR) << "DoCacheStore - Can't make space for "
                      << Base32Substr(name);
          return false;
        }
        int limit(kWaitTransfersForCacheVacantCheck);
        if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
              return pending_xfers_.empty() || (--limit) == 0;
            })) {
          DLOG(ERROR) << "DoCacheStore - Timed out for " << Base32Substr(name)
                      << " while waiting for pending transfers.";
          return false;
        }
      }
      lock.lock();
    }
    cache_chunk_store_->Delete(cached_chunks_.back());
    cached_chunks_.pop_back();
  }

  return cache_chunk_store_->Store(name, source_file_name, delete_source_file);
}

bool BufferedChunkStore::MakeChunkPermanent(const std::string& name,
                                            const uintmax_t &size) {
  boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
  if (!initialised_) {
    DLOG(ERROR) << "MakeChunkPermanent - Can't make " << Base32Substr(name)
                << " permanent, not initialised.";
    return false;
  }

  RemoveDeletionMarks(name);

  // Check whether permanent store has capacity to store chunk
  if (perm_capacity_ > 0) {
    if (size > perm_capacity_) {
      DLOG(ERROR) << "MakeChunkPermanent - Chunk " << Base32Substr(name)
                  << " too big (" << BytesToBinarySiUnits(size) << " vs. "
                  << BytesToBinarySiUnits(perm_capacity_) << ").";
      return false;
    }

    bool is_new(true);
    if (perm_size_ + size > perm_capacity_) {
      if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
            return pending_xfers_.empty();
          })) {
        DLOG(ERROR) << "MakeChunkPermanent - Timed out for "
                    << Base32Substr(name) << " waiting for pending transfers.";
        return false;
      }
      if (perm_chunk_store_->Has(name)) {
        is_new = false;
      } else {
        // Make space in permanent store
        while (perm_size_ + size > perm_capacity_) {
          if (removable_chunks_.empty()) {
            DLOG(ERROR) << "MakeChunkPermanent - Can't make space for "
                        << Base32Substr(name);
            return false;
          }
          if (perm_chunk_store_->Delete(removable_chunks_.front()))
            perm_size_ = perm_chunk_store_->Size();
          removable_chunks_.pop_front();
        }
      }
    }

    if (is_new)
      perm_size_ += size;  // account for chunk in transfer
  }

  pending_xfers_.insert(name);
  asio_service_.post([=] {
    DoMakeChunkPermanent(name);
  });

  return true;
}

void BufferedChunkStore::DoMakeChunkPermanent(const std::string &name) {
  std::string content;
  {
    boost::lock_guard<boost::mutex> lock(cache_mutex_);
    content = cache_chunk_store_->Get(name);
  }

  if (content.empty()) {
    DLOG(ERROR) << "DoMakeChunkPermanent - Could not get " << Base32Substr(name)
                << " from cache.";
  } else if (perm_chunk_store_->Store(name, content)) {
    AddCachedChunksEntry(name);
  } else {
    DLOG(ERROR) << "DoMakeChunkPermanent - Could not store "
                << Base32Substr(name);
  }

  boost::lock_guard<boost::mutex> lock(xfer_mutex_);
  perm_size_ = perm_chunk_store_->Size();
  pending_xfers_.erase(pending_xfers_.find(name));
  xfer_cond_var_.notify_all();
}

void BufferedChunkStore::RemoveDeletionMarks(const std::string &name) {
  removable_chunks_.remove_if([&name](const std::string &i) {
      return name == i;
  });
}

bool BufferedChunkStore::DeleteAllMarked() {
  bool delete_result(true);
  std::list<std::string> rem_chunks;
  {
    boost::mutex::scoped_lock xfer_lock(xfer_mutex_);
    rem_chunks = removable_chunks_;
    removable_chunks_.clear();
    if (!xfer_cond_var_.timed_wait(xfer_lock, kXferWaitTimeout, [&] {
        return pending_xfers_.empty();
      })) {
    DLOG(ERROR) << "DeleteAllMarked - Timed out waiting for pending transfers.";
    return false;
  }
    for (auto it = rem_chunks.begin(); it != rem_chunks.end(); ++it) {
      if (!perm_chunk_store_->Delete(*it)) {
        delete_result = false;
        DLOG(ERROR) << "DeleteAllMarked - Could not delete "
                    << Base32Substr(*it) << " from permanent store.";
      }
    }
    perm_size_ = perm_chunk_store_->Size();
  }

  boost::lock_guard<boost::mutex> lock(cache_mutex_);
  for (auto it = rem_chunks.begin(); it != rem_chunks.end(); ++it) {
    auto it2 = std::find(cached_chunks_.begin(), cached_chunks_.end(), *it);
    if (it2 != cached_chunks_.end())
      cached_chunks_.erase(it2);
    cache_chunk_store_->Delete(*it);
  }

  return delete_result;
}

std::list<std::string> BufferedChunkStore::GetRemovableChunks() const {
  boost::lock_guard<boost::mutex> lock(xfer_mutex_);
  return removable_chunks_;
}

}  // namespace chunk_store

}  // namespace priv

}  // namespace maidsafe
