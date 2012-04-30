/*******************************************************************************
 *  Copyright 2011 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the license   *
 *  file LICENSE.TXT found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ***************************************************************************//**
 * @file  remote_chunk_store.h
 * @brief Class implementing %ChunkStore wrapper for %ChunkManager.
 * @date  2011-05-18
 */

#ifndef MAIDSAFE_PRIVATE_CHUNK_STORE_REMOTE_CHUNK_STORE_H_
#define MAIDSAFE_PRIVATE_CHUNK_STORE_REMOTE_CHUNK_STORE_H_

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>

#include "boost/asio/io_service.hpp"
#include "boost/bimap.hpp"
#include "boost/bimap/list_of.hpp"
#include "boost/bimap/multiset_of.hpp"
#include "boost/filesystem/path.hpp"
// #include "boost/serialization/access.hpp"
#include "boost/signals2/signal.hpp"
#include "boost/thread/condition_variable.hpp"
#include "boost/thread/mutex.hpp"

#include "maidsafe/common/rsa.h"

#include "maidsafe/private/version.h"
#if MAIDSAFE_PRIVATE_VERSION != 300
#  error This API is not compatible with the installed library.\
    Please update the library.
#endif

namespace bptime = boost::posix_time;
namespace bs2 = boost::signals2;
namespace fs = boost::filesystem;

namespace maidsafe {

namespace priv {

namespace chunk_actions { class ChunkActionAuthority; }

namespace chunk_store {

class BufferedChunkStore;
class ChunkManager;

class RemoteChunkStore {
 public:
  enum OperationType {
    kOpGet = 0,
    kOpStore = 1,
    kOpModify = 2,
    kOpDelete = 3
  };

  static const std::string kOpName[];  // see implementation

  // typedef std::function<void(std::string)> GetFunctor;  // NOLINT
  typedef std::function<void(bool)> OpFunctor;  // NOLINT

  typedef bs2::signal<void(size_t)> NumPendingOpsSig;
  typedef std::shared_ptr<NumPendingOpsSig> NumPendingOpsSigPtr;

  struct ValidationData {
    ValidationData(const asymm::Keys &key_pair_in,
                   const std::string &ownership_proof_in)
        : key_pair(key_pair_in),
          ownership_proof(ownership_proof_in) {}
    ValidationData() : key_pair(), ownership_proof() {}
    asymm::Keys key_pair;
    std::string ownership_proof;
  };

  struct OperationData {
    OperationData()
        : op_type(),
          active(false),
          owner_key_id(),
          owner_public_key(),
          ownership_proof(),
          content(),
          callback() {}
    explicit OperationData(const OperationType &op_type)
        : op_type(op_type),
          active(false),
          owner_key_id(),
          owner_public_key(),
          ownership_proof(),
          content(),
          callback() {}
    OperationData(const OperationType &op_type,
                  const OpFunctor &callback,
                  const ValidationData &validation_data)
        : op_type(op_type),
          active(false),
          owner_key_id(validation_data.key_pair.identity),
          owner_public_key(validation_data.key_pair.public_key),
          ownership_proof(validation_data.ownership_proof),
          content(),
          callback(callback) {}
    OperationType op_type;
    bool active;
    asymm::Identity owner_key_id;
    asymm::PublicKey owner_public_key;
    std::string ownership_proof;
    std::string content;
    OpFunctor callback;
  };

  // typedef std::map<std::string, OperationData> OperationMap;
  typedef std::multimap<std::string, OperationType> OperationMultiMap;
  /**
   * The OperationBimap is used to keep pending operations. The left index
   * is for non-unique chunk names, the right index for unique transaction IDs,
   * the relation index reflects the sequence of adding operations, and the info
   * is additional data of the operation.
   */
  typedef boost::bimaps::bimap<boost::bimaps::multiset_of<std::string>,
                               boost::bimaps::set_of<uint32_t>,
                               boost::bimaps::list_of_relation,
                               boost::bimaps::with_info<OperationData> >
      OperationBimap;

  RemoteChunkStore(
      std::shared_ptr<BufferedChunkStore> chunk_store,
      std::shared_ptr<ChunkManager> chunk_manager,
      std::shared_ptr<chunk_actions::ChunkActionAuthority>
          chunk_action_authority);

  ~RemoteChunkStore();

  std::string Get(
      const std::string &name,
      const ValidationData &validation_data = ValidationData());

  bool Store(const std::string &name,
             const std::string &content,
             const OpFunctor &callback,
             const ValidationData &validation_data = ValidationData());

  bool Delete(const std::string &name,
              const OpFunctor &callback,
              const ValidationData &validation_data = ValidationData());

  bool Modify(const std::string &name,
              const std::string &content,
              const OpFunctor &callback,
              const ValidationData &validation_data = ValidationData());

  uintmax_t Size() const {
    // TODO(Steve) get from account
    return 0;  // chunk_store_->Size();
  }

  uintmax_t Capacity() const {
    // TODO(Steve) get from account
    return 0;  // chunk_store_->Capacity();
  }

  uintmax_t NumPendingOps() const {
    return pending_ops_.size();
  }

  bool Empty() const;

  void Clear();

  /// Waits for pending operations, returns false if it times out.
  bool WaitForCompletion();

  /// Sets the maximum number of operations to be processed in parallel.
  void SetMaxActiveOps(const int &max_active_ops) {
    boost::mutex::scoped_lock lock(mutex_);
    max_active_ops_ = max_active_ops;
    if (max_active_ops_ < 1)
      max_active_ops_ = 1;
  }

  /// Sets the time to wait in WaitForCompletion before failing.
  void SetCompletionWaitTimeout(const boost::posix_time::time_duration &value) {
    completion_wait_timeout_ = value;
  }
  /// Sets the time to wait in WaitForConflictingOps before failing.
  void SetOperationWaitTimeout(const boost::posix_time::time_duration &value) {
    operation_wait_timeout_ = value;
  }

  NumPendingOpsSigPtr sig_num_pending_ops() { return sig_num_pending_ops_; }

//   friend class boost::serialization::access;
//   template<class Archive>
//   void serialize(Archive &archive, const unsigned int version);  // NOLINT
//
//   void StoreOpBackups(std::shared_ptr<boost::asio::deadline_timer> timer,
//                       const std::string &pmid);
//   void DoOpBackups(boost::system::error_code error_code,
//                    const std::string &pmid,
//                    std::shared_ptr<boost::asio::deadline_timer> timer);
//   void RetriveOpBackups();
//   void StopOpBackups();

 protected:
  NumPendingOpsSigPtr sig_num_pending_ops_;

 private:
  RemoteChunkStore(const RemoteChunkStore&);
  RemoteChunkStore& operator=(const RemoteChunkStore&);

  void OnOpResult(const OperationType &op_type,
                  const std::string &name,
                  const int &result);
  int WaitForConflictingOps(const std::string &name,
                            const OperationType &op_type,
                            const uint32_t &transaction_id,
                            boost::mutex::scoped_lock *lock);
  bool WaitForGetOps(const std::string &name,
                     const uint32_t &transaction_id,
                     boost::mutex::scoped_lock *lock);
  uint32_t EnqueueOp(const std::string &name,
                     const OperationData &op_data,
                     boost::mutex::scoped_lock *lock);
  void ProcessPendingOps(boost::mutex::scoped_lock *lock);

  std::shared_ptr<BufferedChunkStore> chunk_store_;
  std::shared_ptr<ChunkManager> chunk_manager_;
  std::shared_ptr<chunk_actions::ChunkActionAuthority> chunk_action_authority_;
  bs2::connection cm_get_conn_,
                  cm_store_conn_,
                  cm_modify_conn_,
                  cm_delete_conn_;
  boost::mutex mutex_;
  boost::condition_variable cond_var_;
  int max_active_ops_, active_ops_count_;
  boost::posix_time::time_duration completion_wait_timeout_;
  boost::posix_time::time_duration operation_wait_timeout_;
  OperationBimap pending_ops_;
  OperationMultiMap failed_ops_;
  std::multiset<std::string> waiting_gets_;
  std::map<std::string, bptime::ptime> failed_gets_;
  uintmax_t op_count_[4],
            op_success_count_[4],
            op_skip_count_[4],
            op_size_[4];
};

std::shared_ptr<RemoteChunkStore> CreateLocalChunkStore(
    const fs::path &buffered_chunk_store_path,
    const fs::path &local_chunk_manager_path,
    boost::asio::io_service &asio_service,  // NOLINT (Dan)
    const bptime::time_duration &millisecs = bptime::milliseconds(0));

}  // namespace chunk_store

}  // namespace priv

}  // namespace maidsafe

#endif  // MAIDSAFE_PRIVATE_CHUNK_STORE_REMOTE_CHUNK_STORE_H_
