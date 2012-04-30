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

#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"
#include "maidsafe/routing/routing_table.h"
#include "maidsafe/routing/node_id.h"
#include "maidsafe/routing/log.h"

namespace maidsafe {

namespace routing {

namespace test {

NodeId IncreaseId(const NodeId &kad_id) {
  std::string raw(kad_id.String());
  std::string::reverse_iterator rit = raw.rbegin();
  while (rit != raw.rend()) {
    if (++(*rit) == 0)
      ++rit;
    else
      break;
  }
  return NodeId(raw);
}

const std::string ToBinary(const std::string &raw_id)  {
  std::string hex_encoded(EncodeToHex(raw_id));
  std::string result;
  for (size_t i = 0; i < hex_encoded.size(); ++i) {
    std::string temp;
    switch (hex_encoded[i]) {
      case '0': temp = "0000"; break;
      case '1': temp = "0001"; break;
      case '2': temp = "0010"; break;
      case '3': temp = "0011"; break;
      case '4': temp = "0100"; break;
      case '5': temp = "0101"; break;
      case '6': temp = "0110"; break;
      case '7': temp = "0111"; break;
      case '8': temp = "1000"; break;
      case '9': temp = "1001"; break;
      case 'a': temp = "1010"; break;
      case 'b': temp = "1011"; break;
      case 'c': temp = "1100"; break;
      case 'd': temp = "1101"; break;
      case 'e': temp = "1110"; break;
      case 'f': temp = "1111"; break;
      default : DLOG(ERROR) << "Invalid hex format";
    }
    result += temp;
  }
  return result;
}

TEST(NodeIdTest, BEH_BitToByteCount) {
  for (size_t i = 0; i < kKeySizeBytes; ++i) {
    ASSERT_EQ(i, BitToByteCount(8 * i));
    for (size_t j = 1; j < 8; ++j) {
      ASSERT_EQ(i + 1, BitToByteCount((8 * i) + j));
    }
  }
}

TEST(NodeIdTest, BEH_DefaultCtr) {
  NodeId node_id;
  ASSERT_EQ(kKeySizeBytes, node_id.String().size());
  for (size_t i = 0; i < node_id.String().size(); ++i)
    ASSERT_EQ('\0', node_id.String()[i]);
  std::string hex_id(kKeySizeBytes * 2, '0');
  ASSERT_EQ(hex_id, node_id.ToStringEncoded(NodeId::kHex));
  std::string bin_id(kKeySizeBytes * 8, '0');
  ASSERT_EQ(bin_id, node_id.ToStringEncoded(NodeId::kBinary));
  NodeId dave("david");
  ASSERT_NE("david", dave.String());
}

TEST(NodeIdTest, BEH_CopyCtr) {
  NodeId kadid1(NodeId::kRandomId);
  NodeId kadid2(kadid1);
  ASSERT_TRUE(kadid1 == kadid2);
  for (size_t i = 0; i < kadid1.String().size(); ++i)
    ASSERT_EQ(kadid1.String()[i], kadid2.String()[i]);
  ASSERT_EQ(kadid1.ToStringEncoded(NodeId::kBinary),
            kadid2.ToStringEncoded(NodeId::kBinary));
  ASSERT_EQ(kadid1.ToStringEncoded(NodeId::kHex),
            kadid2.ToStringEncoded(NodeId::kHex));
  ASSERT_EQ(kadid1.String(), kadid2.String());
}

TEST(NodeIdTest, BEH_KadIdTypeCtr) {
  std::string min_id = kZeroId;
  ASSERT_EQ(kKeySizeBytes, min_id.size());
  for (int i = 0; i < kKeySizeBytes; ++i)
    ASSERT_EQ(min_id[i], '\0');
  NodeId max_id(NodeId::kMaxId);
  ASSERT_EQ(kKeySizeBytes, max_id.String().size());
  for (int i = 0; i < kKeySizeBytes; ++i)
    ASSERT_EQ(-1, max_id.String()[i]);
  NodeId rand_id(NodeId::kRandomId);
  ASSERT_EQ(kKeySizeBytes, rand_id.String().size());
  // TODO(Fraser#5#): 2010-06-06 - Test for randomness properly
  ASSERT_NE(rand_id.String(), NodeId(NodeId::kRandomId).String());
}

TEST(NodeIdTest, BEH_StringCtr) {
  std::string rand_str(RandomString(kKeySizeBytes));
  NodeId id1(rand_str);
  ASSERT_TRUE(id1.String() == rand_str);
  NodeId id2(rand_str.substr(0, kKeySizeBytes - 1));
  ASSERT_TRUE(id2.String().empty());
  NodeId id3(rand_str + "a");
  ASSERT_TRUE(id3.String().empty());
}

TEST(NodeIdTest, BEH_EncodingCtr) {
  std::string known_raw(kKeySizeBytes, 0);
  for (char c = 0; c < kKeySizeBytes; ++c)
    known_raw.at(static_cast<uint8_t>(c)) = c;
  for (int i = 0; i < 4; ++i) {
    std::string rand_str(RandomString(kKeySizeBytes));
    std::string bad_encoded("Bad Encoded"), encoded, known_encoded;
    NodeId::EncodingType type = static_cast<NodeId::EncodingType>(i);
    switch (type) {
      case NodeId::kBinary :
        encoded = ToBinary(rand_str);
        known_encoded = ToBinary(known_raw);
        break;
      case NodeId::kHex :
        encoded = EncodeToHex(rand_str);
        known_encoded = EncodeToHex(known_raw);
        break;
      case NodeId::kBase32 :
        encoded = EncodeToBase32(rand_str);
        known_encoded = EncodeToBase32(known_raw);
        break;
      case NodeId::kBase64 :
        encoded = EncodeToBase64(rand_str);
        known_encoded = EncodeToBase64(known_raw);
        break;
      default :
        break;
    }
    NodeId bad_id(bad_encoded, type);
    ASSERT_TRUE(bad_id.String().empty());
    ASSERT_FALSE(bad_id.IsValid());
    ASSERT_TRUE(bad_id.ToStringEncoded(type).empty());
    NodeId rand_id(encoded, type);
    ASSERT_EQ(rand_str, rand_id.String());
    ASSERT_EQ(encoded, rand_id.ToStringEncoded(type));
    NodeId known_id(known_encoded, type);
    ASSERT_EQ(known_raw, known_id.String());
    ASSERT_EQ(known_encoded, known_id.ToStringEncoded(type));
    switch (i) {
      case NodeId::kBinary :
        ASSERT_EQ("000000000000000100000010000000110000010000000101000001100000"
                  "011100001000000010010000101000001011000011000000110100001110"
                  "000011110001000000010001000100100001001100010100000101010001"
                  "011000010111000110000001100100011010000110110001110000011101"
                  "000111100001111100100000001000010010001000100011001001000010"
                  "010100100110001001110010100000101001001010100010101100101100"
                  "001011010010111000101111001100000011000100110010001100110011"
                  "010000110101001101100011011100111000001110010011101000111011"
                  "00111100001111010011111000111111", known_encoded);
        break;
      case NodeId::kHex :
        ASSERT_EQ("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d"
                  "1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b"
                  "3c3d3e3f", known_encoded);
        break;
      case NodeId::kBase32 :
        ASSERT_EQ("aaasea2eawdaqcajbifs2diqb6ibcesvcsktnf22depbyha7d2ruaijcenuc"
                  "kjthfawuwk3nfwzc8nbtgi3vipjyg66duqt5hs8v6r2", known_encoded);
        break;
      case NodeId::kBase64 :
        ASSERT_EQ("AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKiss"
                  "LS4vMDEyMzQ1Njc4OTo7PD0+Pw==", known_encoded);
        break;
      default :
        break;
    }
  }
}

TEST(NodeIdTest, BEH_CtrPower) {
#ifdef __MSVC__
#  pragma warning(push)
#  pragma warning(disable: 4245)
#endif
  NodeId node_id(-2);
#ifdef __MSVC__
#  pragma warning(pop)
#endif
  ASSERT_FALSE(node_id.IsValid());
  node_id = NodeId((kKeySizeBytes * 8) + 1);
  ASSERT_FALSE(node_id.IsValid());
  std::string bin_id(kKeySizeBytes * 8, '0');
  for (int16_t i = 0; i < (kKeySizeBytes * 8); ++i) {
    NodeId node_id(i);
    bin_id[(kKeySizeBytes * 8) - 1 - i] = '1';
    ASSERT_EQ(bin_id, node_id.ToStringEncoded(NodeId::kBinary))
        << "Fail to construct 2^" << i << std::endl;
    bin_id[(kKeySizeBytes * 8) - 1 - i] = '0';
  }
}

TEST(NodeIdTest, BEH_CtrBetweenIds) {
  NodeId id1(NodeId::kRandomId), id2(NodeId::kRandomId);
#ifdef __MSVC__
#  pragma warning(push)
#  pragma warning(disable: 4245)
#endif
  NodeId bad_id(-2);
#ifdef __MSVC__
#  pragma warning(pop)
#endif
  NodeId id(id1, bad_id);
  ASSERT_FALSE(id.IsValid());
  id = NodeId(bad_id, id2);
  ASSERT_FALSE(id.IsValid());
  id = NodeId(id1, id1);
  ASSERT_TRUE(id.IsValid());
  ASSERT_EQ(id1.String(), id.String());
  for (int i = 0; i < 100; ++i) {
    id1 = NodeId(NodeId::kRandomId);
    id2 = NodeId(NodeId::kRandomId);
    id = NodeId(id1, id2);
    ASSERT_TRUE(id.IsValid());
    ASSERT_TRUE(id >= std::min(id1, id2)) << "id  = " <<
        id.ToStringEncoded(NodeId::kBinary) << std::endl << "id1 = "
        << id1.ToStringEncoded(NodeId::kBinary) << std::endl << "id2 = "
        << id2.ToStringEncoded(NodeId::kBinary) << std::endl << "min = "
        << std::min(id1, id2).ToStringEncoded(NodeId::kBinary) << std::endl;
    ASSERT_TRUE(id <= std::max(id1, id2)) << "id  = " <<
        id.ToStringEncoded(NodeId::kBinary) << std::endl << "id1 = "
        << id1.ToStringEncoded(NodeId::kBinary) << std::endl << "id2 = "
        << id2.ToStringEncoded(NodeId::kBinary) << std::endl << "max = "
        << std::max(id1, id2).ToStringEncoded(NodeId::kBinary) << std::endl;
  }
  NodeId min_range, max_range(NodeId::kMaxId);
  for (uint16_t i = 0; i < (kKeySizeBytes * 8) - 1; ++i) {
    min_range = NodeId(i);
    max_range = NodeId(i + 1);
    id = NodeId(min_range, max_range);
    ASSERT_TRUE(id >= min_range) << "id = " <<
        id.ToStringEncoded(NodeId::kBinary) << std::endl << "min_range = "
        << min_range.ToStringEncoded(NodeId::kBinary) << std::endl;
    ASSERT_TRUE(max_range >= id) << "id = " <<
        id.ToStringEncoded(NodeId::kBinary) << std::endl << "max_range = "
        << max_range.ToStringEncoded(NodeId::kBinary) << std::endl;
  }
}

TEST(NodeIdTest, BEH_OperatorEqual) {
  NodeId kadid1(NodeId::kRandomId);
  std::string id(kadid1.String());
  NodeId kadid2(id);
  ASSERT_TRUE(kadid1 == kadid2) << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) << std::endl << "kadid2 = " <<
      kadid2.ToStringEncoded(NodeId::kBinary) << std::endl;
  std::string id1;
  for (size_t i = 0; i < BitToByteCount(kKeySizeBytes * 8) * 2;
       ++i) {
    id1 += "f";
  }
  NodeId kadid3(id1, NodeId::kHex);
  ASSERT_FALSE(kadid1 == kadid3) << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) << std::endl << "kadid3 = " <<
      kadid3.ToStringEncoded(NodeId::kBinary) << std::endl;
}

TEST(NodeIdTest, BEH_OperatorDifferent) {
  NodeId kadid1(NodeId::kRandomId);
  std::string id(kadid1.String());
  NodeId kadid2(id);
  ASSERT_FALSE(kadid1 != kadid2) << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) <<
      std::endl << "kadid2 = " << kadid2.ToStringEncoded(NodeId::kBinary) <<
      std::endl;
  std::string id1;
  for (size_t i = 0; i < BitToByteCount(kKeySizeBytes * 8) * 2; ++i)
    id1 += "f";
  NodeId kadid3(id1, NodeId::kHex);
  ASSERT_TRUE(kadid1 != kadid3) << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) << std::endl << "kadid3 = " <<
      kadid3.ToStringEncoded(NodeId::kBinary) << std::endl;
}

TEST(NodeIdTest, BEH_OperatorGreaterThan) {
  NodeId kadid1(NodeId::kRandomId);
  while (kadid1 == NodeId(NodeId::kMaxId))
    kadid1 = NodeId(NodeId::kRandomId);
  NodeId kadid2(kadid1);
  ASSERT_FALSE(kadid1 > kadid2) << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) << std::endl << "kadid2 = " <<
      kadid2.ToStringEncoded(NodeId::kBinary) << std::endl;
  NodeId kadid3(IncreaseId(kadid1));
  ASSERT_TRUE(kadid3 > kadid1) << "kadid3 = " <<
      kadid3.ToStringEncoded(NodeId::kBinary) << std::endl << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) << std::endl;
  ASSERT_FALSE(kadid1 > kadid3) << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) << std::endl << "kadid3 = " <<
      kadid3.ToStringEncoded(NodeId::kBinary) << std::endl;
}

TEST(NodeIdTest, BEH_OperatorLessThan) {
  NodeId kadid1(NodeId::kRandomId);
  while (kadid1 == NodeId(NodeId::kMaxId))
    kadid1 = NodeId(NodeId::kRandomId);
  NodeId kadid2(kadid1);
  ASSERT_FALSE(kadid1 < kadid2) << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) << std::endl << "kadid2 = " <<
      kadid2.ToStringEncoded(NodeId::kBinary) << std::endl;
  NodeId kadid3(IncreaseId(kadid1));
  ASSERT_TRUE(kadid1 < kadid3) << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) << std::endl << "kadid3 = " <<
      kadid3.ToStringEncoded(NodeId::kBinary) << std::endl;
  ASSERT_FALSE(kadid3 < kadid1) << "kadid3 = " <<
      kadid3.ToStringEncoded(NodeId::kBinary) << std::endl << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) << std::endl;
}

TEST(NodeIdTest, BEH_OperatorGreaterEqual) {
  NodeId kadid1(NodeId::kRandomId);
  while (kadid1 == NodeId(NodeId::kMaxId))
    kadid1 = NodeId(NodeId::kRandomId);
  NodeId kadid2(kadid1);
  ASSERT_TRUE(kadid1 >= kadid2) << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) << std::endl << "kadid2 = " <<
      kadid2.ToStringEncoded(NodeId::kBinary) << std::endl;
  NodeId kadid3(IncreaseId(kadid1));
  ASSERT_TRUE(kadid3 >= kadid1) << "kadid3 = " <<
      kadid3.ToStringEncoded(NodeId::kBinary) << std::endl << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) << std::endl;
}

TEST(NodeIdTest, BEH_OperatorLessEqual) {
  NodeId kadid1(NodeId::kRandomId);
  while (kadid1 == NodeId(NodeId::kMaxId))
    kadid1 = NodeId(NodeId::kRandomId);
  NodeId kadid2(kadid1);
  ASSERT_TRUE(kadid1 <= kadid2) << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) << std::endl << "kadid2 = " <<
      kadid2.ToStringEncoded(NodeId::kBinary) << std::endl;
  NodeId kadid3(IncreaseId(kadid1));
  ASSERT_TRUE(kadid1 <= kadid3) << "kadid1 = " <<
      kadid1.ToStringEncoded(NodeId::kBinary) << std::endl << "kadid3 = " <<
      kadid3.ToStringEncoded(NodeId::kBinary) << std::endl;
}

TEST(NodeIdTest, BEH_OperatorXOR) {
  NodeId kadid1(NodeId::kRandomId), kadid2(NodeId::kRandomId);
  NodeId kadid3(kadid1 ^ kadid2);
  std::string binid1(kadid1.ToStringEncoded(NodeId::kBinary));
  std::string binid2(kadid2.ToStringEncoded(NodeId::kBinary));
  std::string binresult;
  for (size_t i = 0; i < binid1.size(); ++i) {
    if (binid1[i] == binid2[i]) {
      binresult += "0";
    } else {
      binresult += "1";
    }
  }
  std::string binzero;
  for (size_t i = 0; i < binid1.size(); ++i)
    binzero += "0";
  ASSERT_NE(binzero, kadid3.ToStringEncoded(NodeId::kBinary));
  ASSERT_EQ(binresult, kadid3.ToStringEncoded(NodeId::kBinary));
  NodeId kadid4(kadid2 ^ kadid1);
  ASSERT_EQ(binresult, kadid4.ToStringEncoded(NodeId::kBinary));
  NodeId kadid5(kadid1.String());
  NodeId kadid6(kadid1 ^ kadid5);
  ASSERT_EQ(binzero, kadid6.ToStringEncoded(NodeId::kBinary));
  std::string zero(kadid6.String());
  ASSERT_EQ(BitToByteCount(kKeySizeBytes * 8), zero.size());
  for (size_t i = 0; i < zero.size(); ++i)
    ASSERT_EQ('\0', zero[i]);
}

TEST(NodeIdTest, BEH_OperatorEql) {
  NodeId kadid1(NodeId::kRandomId), kadid2;
  kadid2 = kadid1;
  ASSERT_TRUE(kadid1 == kadid2);
  for (size_t i = 0; i < kadid1.String().size(); ++i)
    ASSERT_EQ(kadid1.String()[i], kadid2.String()[i]);
  ASSERT_EQ(kadid1.ToStringEncoded(NodeId::kBinary),
            kadid2.ToStringEncoded(NodeId::kBinary));
  ASSERT_EQ(kadid1.ToStringEncoded(NodeId::kHex),
            kadid2.ToStringEncoded(NodeId::kHex));
  ASSERT_EQ(kadid1.String(), kadid2.String());
}

}  // namespace test

}  // namespace routing

}  // namespace maidsafe
