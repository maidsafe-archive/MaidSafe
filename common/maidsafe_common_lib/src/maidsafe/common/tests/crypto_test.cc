/* Copyright (c) 2009 maidsafe.net limited
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

#include <cstdlib>

#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/lexical_cast.hpp"

#include "maidsafe/common/crypto.h"
#include "maidsafe/common/return_codes.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

namespace fs = boost::filesystem;

namespace maidsafe {

namespace crypto {

namespace test {

TEST(CryptoTest, BEH_Obfuscation) {
  EXPECT_TRUE(XOR("A", "").empty());
  EXPECT_TRUE(XOR("", "B").empty());
  EXPECT_TRUE(XOR("A", "BB").empty());
  const size_t kStringSize(1024*256);
  std::string str1 = RandomString(kStringSize);
  std::string str2 = RandomString(kStringSize);
  std::string obfuscated = XOR(str1, str2);
  EXPECT_EQ(kStringSize, obfuscated.size());
  EXPECT_EQ(obfuscated, XOR(str2, str1));
  EXPECT_EQ(str1, XOR(obfuscated, str2));
  EXPECT_EQ(str2, XOR(obfuscated, str1));
  const std::string kZeros(kStringSize, 0);
  EXPECT_EQ(kZeros, XOR(str1, str1));
  EXPECT_EQ(str1, XOR(kZeros, str1));
  const std::string kKnown1("\xa5\x5a");
  const std::string kKnown2("\x5a\xa5");
  EXPECT_EQ(std::string("\xff\xff"), XOR(kKnown1, kKnown2));
}

TEST(CryptoTest, BEH_SecurePasswordGeneration) {
#pragma omp parallel
  {  // NOLINT (dirvine)
    std::string password;
    EXPECT_EQ(kGeneralError,
              SecurePassword("", "salt", 100, &password));
    EXPECT_EQ(kGeneralError,
              SecurePassword("password", "", 100, &password));
    EXPECT_EQ(kGeneralError,
              SecurePassword("password", "salt", 0, &password));
    const std::string kKnownPassword1(DecodeFromHex("70617373776f7264"));
    const std::string kKnownSalt1(DecodeFromHex("1234567878563412"));
    const uint32_t kKnownIterations1(5);
    const std::string kKnownDerived1(DecodeFromHex("4391697b647773d2ac2969385"
    "3dc66c21f036d36256a8b1e617b2364af10aee1e53d7d4ef0c237f40c539769e4f162e0"));
     password.clear();

    EXPECT_EQ(kSuccess,
              SecurePassword(kKnownPassword1,
              kKnownSalt1, kKnownIterations1, &password));
//    std::cout << EncodeToHex(password) << std::endl;
    EXPECT_EQ(kKnownDerived1, password);
    const std::string kKnownPassword2(DecodeFromHex("416c6c206e2d656e746974"
        "696573206d75737420636f6d6d756e69636174652077697468206f74686572206e"
        "2d656e74697469657320766961206e2d3120656e746974656568656568656573"));
    const std::string kKnownSalt2(DecodeFromHex("1234567878563412"));
    const uint32_t kKnownIterations2(500);
    const std::string kKnownDerived2(DecodeFromHex("4391697b647773d2ac29693853"
    "dc66c21f036d36256a8b1e617b2364af10aee1e53d7d4ef0c237f40c539769e4f162e0c1"
    "999230ef5e0196b71598bb945247391fa3d53ca46e5bcf9c697256c7b131d3bcf310b523e"
    "05c3ffc14d7fd8511c840"));
    EXPECT_EQ(kSuccess, SecurePassword(kKnownPassword2,
              kKnownSalt2, kKnownIterations2, &password));
    EXPECT_EQ(kKnownDerived2, password);
  }
}

struct HashTestData {
  HashTestData(const std::string &input_data,
               const std::string &SHA1_hex_res,
               const std::string &SHA256_hex_res,
               const std::string &SHA384_hex_res,
               const std::string &SHA512_hex_res,
               const std::string &Tiger_hex_res)
      : input(input_data),
        SHA1_hex_result(SHA1_hex_res),
        SHA256_hex_result(SHA256_hex_res),
        SHA384_hex_result(SHA384_hex_res),
        SHA512_hex_result(SHA512_hex_res),
        Tiger_hex_result(Tiger_hex_res),
        SHA1_raw_result(DecodeFromHex(SHA1_hex_res)),
        SHA256_raw_result(DecodeFromHex(SHA256_hex_res)),
        SHA384_raw_result(DecodeFromHex(SHA384_hex_res)),
        SHA512_raw_result(DecodeFromHex(SHA512_hex_res)),
        Tiger_raw_result(DecodeFromHex(Tiger_hex_res)) {}
  std::string input;
  std::string SHA1_hex_result;
  std::string SHA256_hex_result;
  std::string SHA384_hex_result;
  std::string SHA512_hex_result;
  std::string Tiger_hex_result;
  std::string SHA1_raw_result;
  std::string SHA256_raw_result;
  std::string SHA384_raw_result;
  std::string SHA512_raw_result;
  std::string Tiger_raw_result;
};

TEST(CryptoTest, BEH_Hash) {
// #pragma omp parallel
  {  // NOLINT (dirvine)
    // Set up industry standard test data
    std::vector<HashTestData> test_data;
    test_data.push_back(HashTestData("abc",
        "a9993e364706816aba3e25717850c26c9cd0d89d",
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
        "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed"
        "8086072ba1e7cc2358baeca134c825a7",
        "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a"
        "2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f",
        "2aab1484e8c158f2bfb8c5ff41b57a525129131c957b5f93"));
    test_data.push_back(HashTestData(
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        "84983e441c3bd26ebaae4aa1f95129e5e54670f1",
        "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1",
        "",
        "",
        "0f7bf9a19b9c58f2b7610df7e84f0ac3a71c631e7b53f78e"));
    test_data.push_back(HashTestData(
        std::string(64 * 15625, 'a'),
        "34aa973cd4c4daa4f61eeb2bdbad27316534016f",
        "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0",
        "9d0e1809716474cb086e834e310a4a1ced149e9c00f248527972cec5704c2a5b"
        "07b8b3dc38ecc4ebae97ddd87f3d8985",
        "e718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973eb"
        "de0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b",
        "6db0e2729cbead93d715c6a7d36302e9b3cee0d2bc314b41"));
    test_data.push_back(HashTestData(
        "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno"
        "ijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
        "",
        "",
        "09330c33f71147e83d192fc782cd1b4753111b173b3b05d22fa08086e3b0f712"
        "fcc7c71a557e2db966c3e9fa91746039",
        "8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018"
        "501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909",
        "ecce1e3610505fce94f732ee25e8cb7afaf7fcc8888866fd"));

    // Set up temp test dir and files
    std::shared_ptr<fs::path> test_dir(
        maidsafe::test::CreateTestPath("MaidSafe_TestCrypto"));
    EXPECT_FALSE(test_dir->empty());
    std::vector<fs::path> input_files;
    for (size_t i = 0; i < test_data.size(); ++i) {
      fs::path input_path(*test_dir);
      input_path /= "Input" + boost::lexical_cast<std::string>(i) + ".txt";
      input_files.push_back(input_path);
      std::fstream input_file(
          input_path.c_str(),
          std::ios::out | std::ios::trunc | std::ios::binary);
      input_file << test_data.at(i).input;
      input_file.close();
    }

    // Run tests
    for (size_t j = 0; j < test_data.size(); ++j) {
      std::string input(test_data.at(j).input);
      if (!test_data.at(j).SHA1_hex_result.empty()) {
        EXPECT_EQ(test_data.at(j).SHA1_hex_result,
                  EncodeToHex(Hash<crypto::SHA1>(input)));
        EXPECT_EQ(test_data.at(j).SHA1_raw_result, Hash<crypto::SHA1>(input));
        EXPECT_EQ(test_data.at(j).SHA1_hex_result,
                  EncodeToHex(HashFile<crypto::SHA1>(input_files.at(j))));
        EXPECT_EQ(test_data.at(j).SHA1_raw_result,
                  HashFile<crypto::SHA1>(input_files.at(j)));
      }

      if (!test_data.at(j).SHA256_hex_result.empty()) {
        EXPECT_EQ(test_data.at(j).SHA256_hex_result,
                  EncodeToHex(Hash<crypto::SHA256>(input)));
        EXPECT_EQ(test_data.at(j).SHA256_raw_result,
                  Hash<crypto::SHA256>(input));
        EXPECT_EQ(test_data.at(j).SHA256_hex_result,
                  EncodeToHex(HashFile<crypto::SHA256>(input_files.at(j))));
        EXPECT_EQ(test_data.at(j).SHA256_raw_result,
                  HashFile<crypto::SHA256>(input_files.at(j)));
      }

      if (!test_data.at(j).SHA384_hex_result.empty()) {
        EXPECT_EQ(test_data.at(j).SHA384_hex_result,
                  EncodeToHex(Hash<crypto::SHA384>(input)));
        EXPECT_EQ(test_data.at(j).SHA384_raw_result,
                  Hash<crypto::SHA384>(input));
        EXPECT_EQ(test_data.at(j).SHA384_hex_result,
                  EncodeToHex(HashFile<crypto::SHA384>(input_files.at(j))));
        EXPECT_EQ(test_data.at(j).SHA384_raw_result,
                  HashFile<crypto::SHA384>(input_files.at(j)));
      }

      if (!test_data.at(j).SHA512_hex_result.empty()) {
        EXPECT_EQ(test_data.at(j).SHA512_hex_result,
                  EncodeToHex(Hash<crypto::SHA512>(input)));
        EXPECT_EQ(test_data.at(j).SHA512_raw_result,
                  Hash<crypto::SHA512>(input));
        EXPECT_EQ(test_data.at(j).SHA512_hex_result,
                  EncodeToHex(HashFile<crypto::SHA512>(input_files.at(j))));
        EXPECT_EQ(test_data.at(j).SHA512_raw_result,
                  HashFile<crypto::SHA512>(input_files.at(j)));
      }

      if (!test_data.at(j).Tiger_hex_result.empty()) {
        EXPECT_EQ(test_data.at(j).Tiger_hex_result,
                  EncodeToHex(Hash<crypto::Tiger>(input)));
        EXPECT_EQ(test_data.at(j).Tiger_raw_result, Hash<crypto::Tiger>(input));
        EXPECT_EQ(test_data.at(j).Tiger_hex_result,
                  EncodeToHex(HashFile<crypto::Tiger>(input_files.at(j))));
        EXPECT_EQ(test_data.at(j).Tiger_raw_result,
                  HashFile<crypto::Tiger>(input_files.at(j)));
      }
    }

    // Check using invalid filename
    EXPECT_TRUE(HashFile<crypto::SHA1>(fs::path("/")).empty());
    EXPECT_TRUE(HashFile<crypto::SHA1>(fs::path("NonExistent")).empty());
    EXPECT_TRUE(HashFile<crypto::SHA256>(fs::path("/")).empty());
    EXPECT_TRUE(HashFile<crypto::SHA256>(fs::path("NonExistent")).empty());
    EXPECT_TRUE(HashFile<crypto::SHA384>(fs::path("/")).empty());
    EXPECT_TRUE(HashFile<crypto::SHA384>(fs::path("NonExistent")).empty());
    EXPECT_TRUE(HashFile<crypto::SHA512>(fs::path("/")).empty());
    EXPECT_TRUE(HashFile<crypto::SHA512>(fs::path("NonExistent")).empty());
    EXPECT_TRUE(HashFile<crypto::Tiger>(fs::path("/")).empty());
    EXPECT_TRUE(HashFile<crypto::Tiger>(fs::path("NonExistent")).empty());
  }
}

std::string CorruptData(const std::string &input) {
  // Replace a single char of input to a different random char.
  std::string output(input);
  output.at(RandomUint32() % input.size()) +=
      (RandomUint32() % 254) + 1;
  return output;
}

TEST(CryptoTest, BEH_SymmEncrypt) {
#pragma omp parallel
  {  // NOLINT (dirvine)
    // Set up data
    const std::string kKey(DecodeFromHex("0a89927670e292af98080a3c3e2bdee4"
          "289b768de74570f9f470282756390fe3"));
    const std::string kIV(DecodeFromHex("92af98080a3c3e2bdee4289b768de7af"));
    const std::string kUnencrypted(DecodeFromHex("8b4a84c8f409d8c8b4a8e70f4"
    "9867c63661f2b31d6e4c984a6a01b2r15e48a47bc46af231d2b146e54a87db43f51c2a"
    "5"));
    const std::string kEncrypted(DecodeFromHex("441f907b71a14c2f482c4d1fef6"
    "1f3d7ffc0f14953f4f575601803feed5d10a3387c273f9a92b2ceb4d9236167d707"));
    const std::string kBadKey(CorruptData(kKey));
    const std::string kBadIV(CorruptData(kIV));
    const std::string kBadUnencrypted(CorruptData(kUnencrypted));
    const std::string kBadEncrypted(CorruptData(kEncrypted));

    // Encryption string to string
    EXPECT_EQ(kEncrypted, SymmEncrypt(kUnencrypted, kKey, kIV));
    EXPECT_NE(kEncrypted, SymmEncrypt(kBadUnencrypted, kKey, kIV));
    EXPECT_NE(kEncrypted, SymmEncrypt(kUnencrypted, kBadKey, kBadIV));

    // Decryption string to string
    EXPECT_EQ(kUnencrypted, SymmDecrypt(kEncrypted, kKey, kIV));
    EXPECT_NE(kUnencrypted, SymmDecrypt(kBadEncrypted, kKey, kIV));
    EXPECT_NE(kUnencrypted, SymmDecrypt(kEncrypted, kBadKey, kBadIV));

    // Check using empty string
    EXPECT_TRUE(SymmEncrypt("", kKey, kIV).empty());
    EXPECT_TRUE(SymmDecrypt("", kKey, kIV).empty());

    // Check using wrong key and wrong IV
    EXPECT_TRUE(SymmEncrypt(kUnencrypted, "", kIV).empty());
    EXPECT_TRUE(SymmEncrypt(kUnencrypted, kKey, "").empty());
    EXPECT_TRUE(SymmDecrypt(kEncrypted, "", kIV).empty());
    EXPECT_TRUE(SymmDecrypt(kEncrypted, kKey, "").empty());
  }
}

TEST(CryptoTest, BEH_Compress) {
  const size_t kTestDataSize(10000);
  const size_t kTolerance(kTestDataSize / 200);
  std::string initial_data(kTestDataSize, 'A');
  initial_data.replace(0, kTestDataSize / 2, RandomString(kTestDataSize / 2));
  std::random_shuffle(initial_data.begin(), initial_data.end());
  const std::string kTestData(initial_data);

  // Compress
  std::vector<std::string> compressed_strings;
  for (uint16_t level = 0; level <= kMaxCompressionLevel; ++level) {
    compressed_strings.push_back(Compress(kTestData, level));
    if (level > 0) {
      EXPECT_GE(compressed_strings.at(level - 1).size() + kTolerance,
                compressed_strings.at(level).size());
    }
  }
  EXPECT_GT(kTestData.size(),
            compressed_strings.at(kMaxCompressionLevel).size());

  // Uncompress
  for (uint16_t level = 0; level <= kMaxCompressionLevel; ++level)
    EXPECT_EQ(kTestData, Uncompress(compressed_strings.at(level)));

  // Try to compress with invalid compression level
  EXPECT_TRUE(Compress(kTestData, kMaxCompressionLevel + 1).empty());

  // Try to uncompress uncompressed data
  EXPECT_TRUE(Uncompress(kTestData).empty());
}

TEST(CryptoTest, BEH_GzipSHA512Deterministic) {
  // if the algorithm changes this test will start failing
  // as it is a bit of a sledgehammer approach
  std::string test_data = "11111111111111122222222222222222222333333333333";
  std::string answer = "d3261fe3c660734571787e5aa730c2e5bf18886e28e2b346cfe7b";
  answer += "8dd4c44e6d01a88526647df8c7555330f3d347e1ac37";
  answer += "35e1a73c79c258e9fa7094f9ab07e33";
  EXPECT_EQ(EncodeToHex(Hash<crypto::SHA512>(
                  (crypto::Compress(test_data, 9)))), answer);
  for (int i = 1; i < 20; ++i)
    test_data += test_data;
  // 23 Mb approx
  std::string answer2 ="651d460d960d3329da36304f0e0bb3098112e4f0583f6e34d2fc";
  answer2 += "3ecdf7908c2a493c4defdce4109d9e715e767890cef558f6b7ae02";
  answer2 += "4f6e8561be2ef0d483872f";
  EXPECT_EQ(EncodeToHex(Hash<crypto::SHA512>(
    (crypto::Compress(test_data, 9)))), answer2);
}

TEST(CryptoTest, BEH_AESTigerDeterministic) {
  // if the algorithm changes this test will start failing
  // as it is a bit of a sledgehammer approach
  std::string test_data = "11111111111111122222222222222222222333333333333";
  std::string answer = "43ecf84f0b07b3f6df2b2910dbdc5022fd6c6124c89647c9";
  EXPECT_EQ(EncodeToHex(Hash<crypto::Tiger>(
                  (crypto::Compress(test_data, 9)))), answer);
  for (int i = 1; i < 20; ++i)
    test_data += test_data;
  std::string answer2 ="f98bb1b55f14f3ec8612212919d47db91bb94c2e9329de2d";
  EXPECT_EQ(EncodeToHex(Hash<crypto::Tiger>(
    (crypto::Compress(test_data, 9)))), answer2);
}

TEST(CryptoTest, BEH_SecretSharing) {
  std::string rand_string(RandomString(64));
  std::string recovered;
  uint8_t num_shares(20);
  uint8_t threshold(10);
  std::vector<std::string> data_parts;
  SecretShareData(threshold, num_shares, rand_string, &data_parts);
  SecretRecoverData(threshold, data_parts, &recovered);
  EXPECT_EQ(recovered, rand_string);
  uint8_t not_enough(9);
  recovered.clear();
  SecretRecoverData(not_enough, data_parts, &recovered);
  EXPECT_NE(recovered, rand_string);
  uint8_t too_many(100);
  recovered.clear();
  SecretRecoverData(too_many, data_parts, &recovered);
  EXPECT_EQ(recovered, rand_string);
}

}  // namespace test

}  // namespace crypto

}  // namespace maidsafe
