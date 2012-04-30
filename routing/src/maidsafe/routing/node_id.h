/*******************************************************************************
 *  Copyright 2012 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ******************************************************************************/

#ifndef MAIDSAFE_ROUTING_NODE_ID_H_
#define MAIDSAFE_ROUTING_NODE_ID_H_

#include <cstdint>
#include <string>
#include <vector>
#include "boost/serialization/nvp.hpp"
#include "maidsafe/common/platform_config.h"
#include "maidsafe/routing/routing_api.h"


namespace maidsafe {

namespace routing {
extern const uint16_t kKeySizeBytes;
extern const uint16_t kKeySizeBits;
extern const std::string kZeroId;

size_t BitToByteCount(const size_t &bit_count);

/**
* @class NodeId
* Class used to contain a valid dht id in the range [0, 2 ^ kKeySizeBits)
*/

class NodeId {
 public:
  enum KadIdType { kMaxId, kRandomId };
  enum EncodingType { kBinary, kHex, kBase32, kBase64 };
  /**
  * Default Constructor.  Creates an id equal to 0.
  **/
  NodeId();

  /**
  * Copy contructor.
  * @param rhs a NodeId object.
  */
  NodeId(const NodeId &other);

  /**
  * Constructor.  Creates an id = (2 ^ kKeySizeBits) - 1 or a random id in the
  * interval [0, 2 ^ kKeySizeBits).
  * @param type Type of id to be created (kMaxId or kRandomId).
  */
  explicit NodeId(const KadIdType &type);

  /**
  * Constructor.  Creates a NodeId from a raw (decoded) string.
  * @param id string representing the decoded dht id.
  */
  explicit NodeId(const std::string &id);

  /**
  * Constructor.  Creates a NodeId from an encoded string.
  * @param id string representing the dht id.
  * @param encoding_type Type of encoding to use.
  */
  NodeId(const std::string &id, const EncodingType &encoding_type);

  /**
  * Constructor.  Creates a NodeId equal to 2 ^ power.
  * @param power < kKeySizeBytes.
  */
  explicit NodeId(const uint16_t &power);

  /**
  * Constructor.  Creates a random NodeId in range [lower ID, higher ID]
  * Prefer to pass lower ID as id1.
  * @param id1 ID upper or lower limit.
  * @param id2 ID upper or lower limit.
  */
  NodeId(const NodeId &id1, const NodeId &id2);

  /**
  * Checks if id1 is closer in XOR distance to target_id than id2.
  * @param id1 NodeId object.
  * @param id2 NodeId object.
  * @param target_id NodeId object to which id1 and id2 distance is computed to
  * be compared.
  * @return True if id1 is closer to target_id than id2, otherwise false.
  */
  static bool CloserToTarget(const NodeId &id1,
                             const NodeId &id2,
                             const NodeId &target_id);

  /** Decoded representation of the dht id.
  * @return A decoded string representation of the dht id.
  */
  const std::string String() const;

  /** Encoded representation of the dht id.
  * @param encoding_type Type of encoding to use.
  * @return An encoded string representation of the dht id.
  */
  const std::string ToStringEncoded(const EncodingType &encoding_type) const;

  /**
  * Checks that raw_id_ has size kKeySizeBytes.
  */
  bool IsValid() const;
  bool operator() (const NodeId &lhs, const NodeId &rhs) const;
  bool operator == (const NodeId &rhs) const;
  bool operator != (const NodeId &rhs) const;
  bool operator < (const NodeId &rhs) const;
  bool operator > (const NodeId &rhs) const;
  bool operator <= (const NodeId &rhs) const;
  bool operator >= (const NodeId &rhs) const;
  NodeId& operator = (const NodeId &rhs);

  /**
  * XOR distance between two dht IDs.  XOR bit to bit.
  * @param rhs NodeId to which this is XOR
  * @return a NodeId object that is equal to this XOR rhs
  */
  const NodeId operator ^ (const NodeId &rhs) const;

 private:
  std::string EncodeToBinary() const;
  void DecodeFromBinary(const std::string &binary_id);
  std::string raw_id_;
};

/** Returns an abbreviated hex representation of node_id */
std::string DebugId(const NodeId &node_id);

}  // namespace routing

}  // namespace maidsafe

#endif  // MAIDSAFE_ROUTING_NODE_ID_H_
