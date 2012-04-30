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

#include <memory>

#include "boost/filesystem.hpp"
#include "boost/thread.hpp"

#include "maidsafe/common/asio_service.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/private/chunk_actions/chunk_action_authority.h"

#include "maidsafe/private/chunk_store/buffered_chunk_store.h"
#include "maidsafe/private/chunk_store/file_chunk_store.h"
#include "maidsafe/private/chunk_store/memory_chunk_store.h"
#include "maidsafe/private/chunk_store/tests/chunk_store_api_test.h"

namespace maidsafe {

namespace priv {

namespace chunk_store {

namespace test {

template <>
void ChunkStoreTest<BufferedChunkStore>::InitChunkStore(
    std::shared_ptr<ChunkStore> *chunk_store,
    const fs::path &chunk_dir,
    boost::asio::io_service &asio_service) {
  chunk_store->reset(new BufferedChunkStore(asio_service));
  if (!chunk_dir.empty())
    reinterpret_cast<BufferedChunkStore*>(chunk_store->get())->Init(chunk_dir);
}

INSTANTIATE_TYPED_TEST_CASE_P(Buffered, ChunkStoreTest, BufferedChunkStore);

class BufferedChunkStoreTest: public testing::Test {
 public:
  BufferedChunkStoreTest()
      : test_dir_(
            maidsafe::test::CreateTestPath("MaidSafe_TestBufferedChunkStore")),
        chunk_dir_(*test_dir_ / "chunks"),
        asio_service_(),
        test_asio_service_(),
        chunk_action_authority_(),
        chunk_store_(),
        mutex_(),
        cond_var_(),
        store_counter_(0),
        cache_modify_counter_(0) {}

  ~BufferedChunkStoreTest() {}

  void DoStore(const std::string &name, const std::string &content) {
    EXPECT_TRUE(chunk_store_->Store(name, content)) << "Contain?: " <<
      chunk_store_->Has(name) << " Size:" << name.size();
    boost::mutex::scoped_lock lock(mutex_);
    ++store_counter_;
    cond_var_.notify_one();
  }

  void DoCacheStore(const std::string &name, const std::string &content) {
    EXPECT_TRUE(chunk_store_->CacheStore(name, content));
    boost::mutex::scoped_lock lock(mutex_);
    ++store_counter_;
    cond_var_.notify_one();
  }

  void DoCacheModify(const std::string &name, const std::string &content) {
    boost::mutex::scoped_lock lock(mutex_);
    EXPECT_TRUE(chunk_store_->Modify(name, content));
    EXPECT_FALSE(chunk_store_->PermanentHas(name));
    EXPECT_TRUE(chunk_store_->CacheHas(name));
    EXPECT_EQ(content, chunk_store_->Get(name));
    ++cache_modify_counter_;
    cond_var_.notify_one();
  }

  void WaitForCacheModify(const int &count) {
    boost::mutex::scoped_lock lock(mutex_);
    while (cache_modify_counter_ < count)
      cond_var_.wait(lock);
  }

  void WaitForStore(const int &count) {
    boost::mutex::scoped_lock lock(mutex_);
    while (store_counter_ < count)
      cond_var_.wait(lock);
  }

 protected:
  void SetUp() {
    asio_service_.Start(3);
    test_asio_service_.Start(3);
    chunk_store_.reset(new BufferedChunkStore(asio_service_.service()));
    fs::create_directories(chunk_dir_);
    chunk_store_->Init(chunk_dir_);
  }

  void TearDown() {
    test_asio_service_.Stop();
    asio_service_.Stop();
  }

  fs::path CreateRandomFile(const fs::path &file_path,
                            const uint64_t &file_size) {
    fs::ofstream ofs(file_path, std::ios::binary | std::ios::out |
                                std::ios::trunc);
    if (file_size != 0) {
      size_t string_size = (file_size > 100000) ? 100000 :
                          static_cast<size_t>(file_size);
      uint64_t remaining_size = file_size;
      std::string rand_str = RandomString(2 * string_size);
      std::string file_content;
      uint64_t start_pos = 0;
      while (remaining_size) {
        srand(17);
        start_pos = rand() % string_size;  // NOLINT (Fraser)
        if (remaining_size < string_size) {
          string_size = static_cast<size_t>(remaining_size);
          file_content = rand_str.substr(0, string_size);
        } else {
          file_content = rand_str.substr(static_cast<size_t>(start_pos),
                                        string_size);
        }
        ofs.write(file_content.c_str(), file_content.size());
        remaining_size -= string_size;
      }
    }
    ofs.close();
    return file_path;
  }

  maidsafe::test::TestPath test_dir_;
  fs::path chunk_dir_;
  AsioService asio_service_, test_asio_service_;
  std::shared_ptr<chunk_actions::ChunkActionAuthority> chunk_action_authority_;
  std::shared_ptr<BufferedChunkStore> chunk_store_;
  boost::mutex mutex_;
  boost::condition_variable cond_var_;
  int store_counter_;
  int cache_modify_counter_;
};

TEST_F(BufferedChunkStoreTest, BEH_CacheInit) {
  EXPECT_EQ(0, chunk_store_->CacheSize());
  EXPECT_EQ(0, chunk_store_->CacheCapacity());
  EXPECT_EQ(0, chunk_store_->CacheCount());
  EXPECT_TRUE(chunk_store_->CacheEmpty());
  EXPECT_FALSE(chunk_store_->CacheHas(""));
  EXPECT_FALSE(chunk_store_->CacheHas("something"));
}

TEST_F(BufferedChunkStoreTest, BEH_CacheStore) {
  std::string content(RandomString(123));
  std::string name_mem(crypto::Hash<crypto::SHA512>(content));
  fs::path path(*this->test_dir_ / "chunk.dat");
  this->CreateRandomFile(path, 456);
  std::string name_file(crypto::HashFile<crypto::SHA512>(path));
  ASSERT_NE(name_mem, name_file);

  // invalid input
  EXPECT_FALSE(chunk_store_->CacheStore(name_mem, ""));
  EXPECT_FALSE(chunk_store_->CacheStore("", content));
  EXPECT_FALSE(chunk_store_->CacheStore(name_file, "", false));
  EXPECT_FALSE(chunk_store_->CacheStore(name_file, *this->test_dir_ / "fail",
                                        false));
  EXPECT_FALSE(chunk_store_->CacheStore("", path, false));
  EXPECT_TRUE(chunk_store_->CacheEmpty());
  EXPECT_EQ(0, chunk_store_->CacheCount());
  EXPECT_EQ(0, chunk_store_->CacheSize());
  EXPECT_FALSE(chunk_store_->CacheHas(name_mem));
  EXPECT_FALSE(chunk_store_->CacheHas(name_file));

  // store from string
  EXPECT_TRUE(chunk_store_->CacheStore(name_mem, content));
  EXPECT_FALSE(chunk_store_->CacheEmpty());
  EXPECT_EQ(1, chunk_store_->CacheCount());
  EXPECT_EQ(123, chunk_store_->CacheSize());
  EXPECT_TRUE(chunk_store_->CacheHas(name_mem));
  EXPECT_FALSE(chunk_store_->CacheHas(name_file));

  ASSERT_EQ(name_mem,
            crypto::Hash<crypto::SHA512>(chunk_store_->Get(name_mem)));

  // store from file
  EXPECT_TRUE(chunk_store_->CacheStore(name_file, path, false));
  EXPECT_FALSE(chunk_store_->CacheEmpty());
  EXPECT_EQ(2, chunk_store_->CacheCount());
  EXPECT_EQ(579, chunk_store_->CacheSize());
  EXPECT_TRUE(chunk_store_->CacheHas(name_mem));
  EXPECT_TRUE(chunk_store_->CacheHas(name_file));

  ASSERT_EQ(name_file,
            crypto::Hash<crypto::SHA512>(chunk_store_->Get(name_file)));

  fs::path new_path(*this->test_dir_ / "chunk2.dat");
  this->CreateRandomFile(new_path, 333);
  std::string new_name(crypto::HashFile<crypto::SHA512>(new_path));

  // overwrite existing, should be ignored
  EXPECT_TRUE(chunk_store_->CacheStore(name_mem, ""));
  EXPECT_TRUE(chunk_store_->CacheStore(name_mem, RandomString(222)));
  EXPECT_TRUE(chunk_store_->CacheStore(name_file, "", false));
  EXPECT_TRUE(chunk_store_->CacheStore(name_file, new_path, false));
  EXPECT_FALSE(chunk_store_->CacheEmpty());
  EXPECT_EQ(2, chunk_store_->CacheCount());
  EXPECT_EQ(579, chunk_store_->CacheSize());
  EXPECT_TRUE(chunk_store_->CacheHas(name_mem));
  EXPECT_TRUE(chunk_store_->CacheHas(name_file));

  ASSERT_EQ(name_mem,
            crypto::Hash<crypto::SHA512>(chunk_store_->Get(name_mem)));
  ASSERT_EQ(name_file,
            crypto::Hash<crypto::SHA512>(chunk_store_->Get(name_file)));

  // delete input file (existing chunk)
  EXPECT_FALSE(chunk_store_->CacheStore("", path, true));
  EXPECT_TRUE(fs::exists(path));
  EXPECT_TRUE(chunk_store_->CacheStore(name_mem, path, true));
  EXPECT_FALSE(fs::exists(path));

  // delete input file (new chunk)
  EXPECT_TRUE(chunk_store_->CacheStore(new_name, new_path, true));
  EXPECT_EQ(new_name,
            crypto::Hash<crypto::SHA512>(chunk_store_->Get(new_name)));
  EXPECT_FALSE(fs::exists(path));
  EXPECT_TRUE(chunk_store_->CacheStore(new_name, new_path, true));
  EXPECT_FALSE(chunk_store_->CacheEmpty());
  EXPECT_EQ(3, chunk_store_->CacheCount());
  EXPECT_EQ(912, chunk_store_->CacheSize());
  EXPECT_TRUE(chunk_store_->CacheHas(new_name));
}

TEST_F(BufferedChunkStoreTest, BEH_CacheHitMiss) {
  std::string content(RandomString(123));
  std::string name_mem(crypto::Hash<crypto::SHA512>(content));

  // store from string
  EXPECT_TRUE(chunk_store_->Store(name_mem, content));
  EXPECT_FALSE(chunk_store_->CacheEmpty());
  EXPECT_EQ(1, chunk_store_->CacheCount());
  EXPECT_EQ(123, chunk_store_->CacheSize());
  EXPECT_TRUE(chunk_store_->CacheHas(name_mem));
  EXPECT_FALSE(chunk_store_->Empty());
  EXPECT_EQ(1, chunk_store_->Count());
  EXPECT_EQ(123, chunk_store_->Size());
  EXPECT_TRUE(chunk_store_->Has(name_mem));

  chunk_store_->CacheClear();
  EXPECT_TRUE(chunk_store_->CacheEmpty());
  EXPECT_EQ(0, chunk_store_->CacheCount());
  EXPECT_EQ(0, chunk_store_->CacheSize());
  EXPECT_FALSE(chunk_store_->CacheHas(name_mem));
  EXPECT_FALSE(chunk_store_->Empty());
  EXPECT_EQ(1, chunk_store_->Count());
  EXPECT_EQ(123, chunk_store_->Size());
  EXPECT_TRUE(chunk_store_->Has(name_mem));

  fs::path path(*this->test_dir_ / "chunk.dat");
  EXPECT_TRUE(chunk_store_->Get(name_mem, path));
  ASSERT_EQ(name_mem, crypto::HashFile<crypto::SHA512>(path));

  EXPECT_FALSE(chunk_store_->CacheEmpty());
  EXPECT_EQ(1, chunk_store_->CacheCount());
  EXPECT_EQ(123, chunk_store_->CacheSize());
  EXPECT_TRUE(chunk_store_->CacheHas(name_mem));

  this->CreateRandomFile(path, 456);
  std::string name_file(crypto::HashFile<crypto::SHA512>(path));
  ASSERT_NE(name_mem, name_file);

  // store from file
  EXPECT_TRUE(chunk_store_->Store(name_file, path, false));
  EXPECT_FALSE(chunk_store_->CacheEmpty());
  EXPECT_EQ(2, chunk_store_->CacheCount());
  EXPECT_EQ(579, chunk_store_->CacheSize());
  EXPECT_TRUE(chunk_store_->CacheHas(name_file));
  EXPECT_FALSE(chunk_store_->Empty());
  EXPECT_EQ(2, chunk_store_->Count());
  EXPECT_EQ(579, chunk_store_->Size());
  EXPECT_TRUE(chunk_store_->Has(name_file));

  chunk_store_->CacheClear();
  EXPECT_TRUE(chunk_store_->CacheEmpty());
  EXPECT_EQ(0, chunk_store_->CacheCount());
  EXPECT_EQ(0, chunk_store_->CacheSize());
  EXPECT_FALSE(chunk_store_->CacheHas(name_file));
  EXPECT_FALSE(chunk_store_->Empty());
  EXPECT_EQ(2, chunk_store_->Count());
  EXPECT_EQ(579, chunk_store_->Size());
  EXPECT_TRUE(chunk_store_->Has(name_file));

  ASSERT_EQ(name_file,
            crypto::Hash<crypto::SHA512>(chunk_store_->Get(name_file)));

  EXPECT_FALSE(chunk_store_->CacheEmpty());
  EXPECT_EQ(1, chunk_store_->CacheCount());
  EXPECT_EQ(456, chunk_store_->CacheSize());
  EXPECT_TRUE(chunk_store_->CacheHas(name_file));

  EXPECT_TRUE(chunk_store_->Delete(name_mem));
  EXPECT_TRUE(chunk_store_->Delete(name_file));
  EXPECT_TRUE(chunk_store_->CacheEmpty());
  EXPECT_EQ(0, chunk_store_->CacheCount());
  EXPECT_EQ(0, chunk_store_->CacheSize());
  EXPECT_FALSE(chunk_store_->CacheHas(name_mem));
  EXPECT_FALSE(chunk_store_->CacheHas(name_file));
  EXPECT_TRUE(chunk_store_->Empty());
  EXPECT_EQ(0, chunk_store_->Count());
  EXPECT_EQ(0, chunk_store_->Size());
  EXPECT_FALSE(chunk_store_->Has(name_mem));
  EXPECT_FALSE(chunk_store_->Has(name_file));
}

TEST_F(BufferedChunkStoreTest, BEH_CacheCapacity) {
  std::string content1(RandomString(100));
  std::string name1(crypto::Hash<crypto::SHA512>(content1));
  std::string content2(RandomString(50));
  std::string name2(crypto::Hash<crypto::SHA512>(content2));
  std::string content3(RandomString(25));
  std::string name3(crypto::Hash<crypto::SHA512>(content3));

  EXPECT_EQ(0, chunk_store_->CacheCapacity());
  EXPECT_TRUE(chunk_store_->CacheVacant(0));
  EXPECT_TRUE(chunk_store_->CacheVacant(123456789));
  chunk_store_->SetCacheCapacity(125);
  EXPECT_EQ(125, chunk_store_->CacheCapacity());
  EXPECT_TRUE(chunk_store_->CacheVacant(125));
  EXPECT_FALSE(chunk_store_->CacheVacant(126));

  // store #1, space to 100
  EXPECT_TRUE(chunk_store_->CacheVacant(content1.size()));
  EXPECT_TRUE(chunk_store_->CacheStore(name1, content1));
  EXPECT_TRUE(chunk_store_->CacheHas(name1));
  EXPECT_EQ(100, chunk_store_->CacheSize());

  // store #2, 25 over limit, #1 will be pruned
  EXPECT_FALSE(chunk_store_->CacheVacant(content2.size()));
  EXPECT_TRUE(chunk_store_->CacheStore(name2, content2));
  EXPECT_FALSE(chunk_store_->CacheHas(name1));
  EXPECT_TRUE(chunk_store_->CacheHas(name2));
  EXPECT_EQ(50, chunk_store_->CacheSize());

  // store #3, space to 75
  EXPECT_TRUE(chunk_store_->CacheVacant(content3.size()));
  EXPECT_TRUE(chunk_store_->CacheStore(name3, content3));
  EXPECT_FALSE(chunk_store_->CacheHas(name1));
  EXPECT_TRUE(chunk_store_->CacheHas(name2));
  EXPECT_TRUE(chunk_store_->CacheHas(name3));
  EXPECT_EQ(75, chunk_store_->CacheSize());

  // store #1, 50 over limit, prune #2
  EXPECT_FALSE(chunk_store_->CacheVacant(content1.size()));
  EXPECT_TRUE(chunk_store_->CacheStore(name1, content1));
  EXPECT_TRUE(chunk_store_->CacheHas(name1));
  EXPECT_FALSE(chunk_store_->CacheHas(name2));
  EXPECT_TRUE(chunk_store_->CacheHas(name3));
  EXPECT_EQ(125, chunk_store_->CacheSize());

  // store #1 again, nothing changes
  EXPECT_FALSE(chunk_store_->CacheVacant(content1.size()));
  EXPECT_TRUE(chunk_store_->CacheStore(name1, content1));
  EXPECT_TRUE(chunk_store_->CacheHas(name1));
  EXPECT_FALSE(chunk_store_->CacheHas(name2));
  EXPECT_TRUE(chunk_store_->CacheHas(name3));
  EXPECT_EQ(125, chunk_store_->CacheSize());

  // store #2, 50 over limit, prune #3 and #1 because of FIFO
  EXPECT_FALSE(chunk_store_->CacheVacant(content2.size()));
  EXPECT_TRUE(chunk_store_->CacheStore(name2, content2));
  EXPECT_FALSE(chunk_store_->CacheHas(name1));
  EXPECT_TRUE(chunk_store_->CacheHas(name2));
  EXPECT_FALSE(chunk_store_->CacheHas(name3));
  EXPECT_EQ(50, chunk_store_->CacheSize());

  // reduce capacity to current size
  EXPECT_EQ(125, chunk_store_->CacheCapacity());
  chunk_store_->SetCacheCapacity(10);
  EXPECT_EQ(50, chunk_store_->CacheCapacity());

  // try to store #1, fails because of size
  EXPECT_FALSE(chunk_store_->CacheVacant(content1.size()));
  EXPECT_FALSE(chunk_store_->CacheStore(name1, content1));
  EXPECT_FALSE(chunk_store_->CacheHas(name1));
  EXPECT_EQ(50, chunk_store_->CacheSize());

  // store #3, 25 over limit, prune #2
  EXPECT_FALSE(chunk_store_->CacheVacant(content3.size()));
  EXPECT_TRUE(chunk_store_->CacheStore(name3, content3));
  EXPECT_FALSE(chunk_store_->CacheHas(name1));
  EXPECT_FALSE(chunk_store_->CacheHas(name2));
  EXPECT_TRUE(chunk_store_->CacheHas(name3));
  EXPECT_EQ(25, chunk_store_->CacheSize());

  fs::path path(*this->test_dir_ / "chunk.dat");
  this->CreateRandomFile(path, 100);
  std::string name_file(crypto::HashFile<crypto::SHA512>(path));
  ASSERT_NE(name3, name_file);

  // try to store from file, fails because of size
  EXPECT_FALSE(chunk_store_->CacheVacant(100));
  EXPECT_FALSE(chunk_store_->CacheStore(name_file, path, false));
  EXPECT_FALSE(chunk_store_->CacheHas(name1));
  EXPECT_FALSE(chunk_store_->CacheHas(name2));
  EXPECT_TRUE(chunk_store_->CacheHas(name3));
  EXPECT_FALSE(chunk_store_->CacheHas(name_file));
  EXPECT_EQ(25, chunk_store_->CacheSize());

  chunk_store_->SetCacheCapacity(100);

  // try to store from file again, 25 over limit, prune #3
  EXPECT_FALSE(chunk_store_->CacheVacant(100));
  EXPECT_TRUE(chunk_store_->CacheStore(name_file, path, false));
  EXPECT_FALSE(chunk_store_->CacheHas(name1));
  EXPECT_FALSE(chunk_store_->CacheHas(name2));
  EXPECT_FALSE(chunk_store_->CacheHas(name3));
  EXPECT_TRUE(chunk_store_->CacheHas(name_file));
  EXPECT_EQ(100, chunk_store_->CacheSize());
}

TEST_F(BufferedChunkStoreTest, BEH_CacheClear) {
  std::vector<std::string> chunks;
  for (int i = 0; i < 20; ++i) {
    std::string content(RandomString(100));
    std::string name(crypto::Hash<crypto::SHA512>(content));
    chunks.push_back(name);
    EXPECT_TRUE(chunk_store_->CacheStore(name, content));
    EXPECT_TRUE(chunk_store_->CacheHas(name));
  }
  EXPECT_FALSE(chunk_store_->CacheEmpty());
  EXPECT_EQ(20, chunk_store_->CacheCount());
  EXPECT_EQ(2000, chunk_store_->CacheSize());

  chunk_store_->CacheClear();

  for (auto it = chunks.begin(); it != chunks.end(); ++it)
    EXPECT_FALSE(chunk_store_->CacheHas(*it));
  EXPECT_TRUE(chunk_store_->CacheEmpty());
  EXPECT_EQ(0, chunk_store_->CacheCount());
  EXPECT_EQ(0, chunk_store_->CacheSize());

  std::string content(RandomString(100));
  std::string name(crypto::Hash<crypto::SHA512>(content));
  EXPECT_TRUE(chunk_store_->Store(name, content));
  chunk_store_->CacheClear();
  EXPECT_FALSE(chunk_store_->CacheHas(name));
  EXPECT_TRUE(chunk_store_->PermanentHas(name));
}

TEST_F(BufferedChunkStoreTest, BEH_PermanentStore) {
  std::string content(RandomString(100));
  std::string name1(RandomString(64)), name2(RandomString(64));

  EXPECT_FALSE(chunk_store_->PermanentStore(""));
  EXPECT_FALSE(chunk_store_->PermanentStore(name1));
  EXPECT_FALSE(chunk_store_->CacheHas(name1));
  EXPECT_FALSE(chunk_store_->PermanentHas(name1));

  EXPECT_TRUE(chunk_store_->CacheStore(name1, content));
  EXPECT_TRUE(chunk_store_->CacheHas(name1));
  EXPECT_FALSE(chunk_store_->PermanentHas(name1));

  EXPECT_TRUE(chunk_store_->Store(name1, content));
  EXPECT_TRUE(chunk_store_->CacheHas(name1));
  EXPECT_TRUE(chunk_store_->PermanentHas(name1));

  EXPECT_FALSE(chunk_store_->CacheHas(name2));
  EXPECT_FALSE(chunk_store_->PermanentHas(name2));

  EXPECT_TRUE(chunk_store_->CacheStore(name2, content));
  EXPECT_TRUE(chunk_store_->CacheHas(name2));
  EXPECT_FALSE(chunk_store_->PermanentHas(name2));

  EXPECT_TRUE(chunk_store_->PermanentStore(name2));
  EXPECT_TRUE(chunk_store_->CacheHas(name2));
  EXPECT_TRUE(chunk_store_->PermanentHas(name2));

  chunk_store_->CacheClear();
  EXPECT_EQ(content, chunk_store_->Get(name1));
  EXPECT_EQ(content, chunk_store_->Get(name2));

  chunk_store_->CacheClear();
  EXPECT_TRUE(chunk_store_->PermanentStore(name1));
  EXPECT_TRUE(chunk_store_->PermanentHas(name2));

  chunk_store_->Clear();
  EXPECT_TRUE(chunk_store_->Store(name1, content));
  EXPECT_TRUE(chunk_store_->PermanentHas(name1));

  chunk_store_->MarkForDeletion(name1);
  EXPECT_TRUE(chunk_store_->Has(name1));
  EXPECT_TRUE(chunk_store_->CacheHas(name1));
  EXPECT_FALSE(chunk_store_->PermanentHas(name1));
  EXPECT_TRUE(chunk_store_->PermanentStore(name1));
  EXPECT_TRUE(chunk_store_->PermanentHas(name1));

  chunk_store_->CacheClear();
  chunk_store_->MarkForDeletion(name1);
  EXPECT_FALSE(chunk_store_->Has(name1));
  EXPECT_FALSE(chunk_store_->CacheHas(name1));
  EXPECT_FALSE(chunk_store_->PermanentHas(name1));
  EXPECT_TRUE(chunk_store_->PermanentStore(name1));
  EXPECT_TRUE(chunk_store_->PermanentHas(name1));

  chunk_store_->Clear();
  EXPECT_TRUE(chunk_store_->Store(name1, content));
  EXPECT_TRUE(chunk_store_->Store(name1, content));
  EXPECT_TRUE(chunk_store_->Store(name1, content));
  EXPECT_EQ(3, chunk_store_->Count(name1));

  chunk_store_->MarkForDeletion(name1);
  chunk_store_->MarkForDeletion(name1);
  EXPECT_TRUE(chunk_store_->PermanentHas(name1));

  chunk_store_->MarkForDeletion(name1);
  EXPECT_FALSE(chunk_store_->PermanentHas(name1));

  EXPECT_TRUE(chunk_store_->PermanentStore(name1));
  EXPECT_TRUE(chunk_store_->PermanentHas(name1));

  chunk_store_->MarkForDeletion(name1);
  chunk_store_->MarkForDeletion(name1);
  EXPECT_TRUE(chunk_store_->PermanentHas(name1));

  chunk_store_->MarkForDeletion(name1);
  EXPECT_FALSE(chunk_store_->PermanentHas(name1));
}

TEST_F(BufferedChunkStoreTest, BEH_WaitForTransfer) {
  std::string content(RandomString(256 << 10));

  store_counter_ = 0;
  for (int i = 0; i < 100; ++i)
    test_asio_service_.service().post(
        std::bind(&BufferedChunkStoreTest::DoStore, this,
                  RandomString(64), content));
  WaitForStore(100);
  chunk_store_->Clear();

  store_counter_ = 0;
  for (int i = 0; i < 100; ++i)
    test_asio_service_.service().post(
        std::bind(&BufferedChunkStoreTest::DoStore, this,
                  RandomString(64), content));
  WaitForStore(100);
  chunk_store_.reset();
}

TEST_F(BufferedChunkStoreTest, BEH_CacheFlooding) {
  std::string content(RandomString(256 << 10));  // 256 KB chunk
  chunk_store_->SetCacheCapacity(4 << 20);  // 4 MB cache space = 16 chunks

  std::string first(RandomString(64));
  EXPECT_TRUE(chunk_store_->Store(first, content));

  store_counter_ = 1;
  for (int i = 1; i < 500; ++i)
    test_asio_service_.service().post(
        std::bind(&BufferedChunkStoreTest::DoStore, this,
                  RandomString(64), content));
  WaitForStore(500);
  chunk_store_->Delete(first);
  EXPECT_EQ(499, chunk_store_->Count());
}

TEST_F(BufferedChunkStoreTest, BEH_StoreWithRemovableChunks) {
  const uint16_t kChunkCount = 10;
  std::vector<std::string> chunks;
  for (auto i = 0; i < kChunkCount; ++i)
    chunks.push_back(RandomString(64));

  //  Set capacity of Chunk Store
  chunk_store_->SetCapacity(2570);
  EXPECT_EQ(uintmax_t(2570), chunk_store_->Capacity());

  //  Store chunks in Chunk Store
  for (auto it = chunks.begin(); it != chunks.end(); ++it) {
    EXPECT_TRUE(chunk_store_->Store(*it, RandomString(256)));
    EXPECT_TRUE(chunk_store_->Has(*it));
    chunk_store_->MarkForDeletion(*it);
  }
  EXPECT_EQ(kChunkCount, chunk_store_->Count());
  EXPECT_EQ(uintmax_t(2560), chunk_store_->Size());
  std::string name1(RandomString(64));
  std::string content1(RandomString(2580));

  //  Trying to store chunk bigger than Chunk Store Capacity
  EXPECT_FALSE(chunk_store_->Store(name1, content1));

  content1 = RandomString(2560);
  EXPECT_TRUE(chunk_store_->Store(name1, content1));
  EXPECT_TRUE(chunk_store_->Has(name1));
  EXPECT_EQ(uintmax_t(1), chunk_store_->Count());
  EXPECT_EQ(uintmax_t(2560), chunk_store_->Size());
}

TEST_F(BufferedChunkStoreTest, BEH_ModifyCacheChunks) {
  std::string modifying_chunk_content(RandomString(100));
  std::string modifying_chunk_name(RandomString(65));
  store_counter_ = 0;
  cache_modify_counter_ = 0;
  chunk_store_->SetCacheCapacity(4 << 20);
  chunk_store_->SetCapacity(4 << 20);
  test_asio_service_.service().post(
      std::bind(&BufferedChunkStoreTest::DoCacheStore, this,
                modifying_chunk_name, modifying_chunk_content));
  WaitForStore(1);

  for (int i = 1; i < 100; ++i) {
    test_asio_service_.service().post(
        std::bind(&BufferedChunkStoreTest::DoStore,
                  this,
                  RandomString(64 + i % 2),
                  RandomString(RandomUint32() % 99 + 1)));
    test_asio_service_.service().post(
        std::bind(&BufferedChunkStoreTest::DoCacheModify, this,
                  modifying_chunk_name, RandomString(RandomUint32() % 120)));
  }
  WaitForStore(100);
  WaitForCacheModify(99);
}

TEST_F(BufferedChunkStoreTest, BEH_DeleteAllMarked) {
  std::string content(RandomString(100));
  std::string name1(RandomString(64)), name2(RandomString(64));

  for (int i = 0; i < 4; ++i)
    EXPECT_TRUE(chunk_store_->Store(name1, content));
  EXPECT_EQ(4, chunk_store_->Count(name1));
  EXPECT_TRUE(chunk_store_->Store(name2, content));

  for (int i = 0; i < 3; ++i)
    chunk_store_->MarkForDeletion(name1);

  std::list<std::string> delete_list = chunk_store_->GetRemovableChunks();
  EXPECT_EQ(3, delete_list.size());
  EXPECT_TRUE(chunk_store_->DeleteAllMarked());
  EXPECT_TRUE(chunk_store_->PermanentHas(name1));
  EXPECT_EQ(1, chunk_store_->Count(name1));
  EXPECT_EQ(1, chunk_store_->Count(name2));

  chunk_store_->MarkForDeletion(name1);
  EXPECT_TRUE(chunk_store_->DeleteAllMarked());
  EXPECT_FALSE(chunk_store_->PermanentHas(name1));
  EXPECT_EQ(1, chunk_store_->Count(name2));

  delete_list = chunk_store_->GetRemovableChunks();
  EXPECT_EQ(0, delete_list.size());
}

}  // namespace test

}  // namespace chunk_store

}  // namespace priv

}  // namespace maidsafe
