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

#include <algorithm>
#include <cstdlib>
#include <set>

#include "boost/filesystem.hpp"
#include "boost/lexical_cast.hpp"

#include "maidsafe/common/crypto.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/omp.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

namespace fs = boost::filesystem;
namespace bptime = boost::posix_time;

namespace maidsafe {

namespace test {

void GenerateRandomStrings(const int &string_count, const size_t &string_size) {
  for (int i = 0; i < string_count; ++i)
    RandomString(string_size);
}


TEST(UtilsTest, BEH_BytesToDecimalSiUnits) {
  EXPECT_EQ("0 B", BytesToDecimalSiUnits(0U));
  EXPECT_EQ("1 B", BytesToDecimalSiUnits(1U));
  EXPECT_EQ("12 B", BytesToDecimalSiUnits(12U));
  EXPECT_EQ("123 B", BytesToDecimalSiUnits(123U));
  EXPECT_EQ("999 B", BytesToDecimalSiUnits(999U));

  EXPECT_EQ("1 kB", BytesToDecimalSiUnits(1000U));
  EXPECT_EQ("1 kB", BytesToDecimalSiUnits(1499U));
  EXPECT_EQ("2 kB", BytesToDecimalSiUnits(1500U));
  EXPECT_EQ("2 kB", BytesToDecimalSiUnits(1999U));
  EXPECT_EQ("12 kB", BytesToDecimalSiUnits(12499U));
  EXPECT_EQ("13 kB", BytesToDecimalSiUnits(12500U));
  EXPECT_EQ("123 kB", BytesToDecimalSiUnits(123499U));
  EXPECT_EQ("124 kB", BytesToDecimalSiUnits(123500U));
  EXPECT_EQ("999 kB", BytesToDecimalSiUnits(999499U));

  EXPECT_EQ("1 MB", BytesToDecimalSiUnits(999500U));
  EXPECT_EQ("1 MB", BytesToDecimalSiUnits(1000000U));
  EXPECT_EQ("1 MB", BytesToDecimalSiUnits(1499999U));
  EXPECT_EQ("2 MB", BytesToDecimalSiUnits(1500000U));
  EXPECT_EQ("12 MB", BytesToDecimalSiUnits(12499999U));
  EXPECT_EQ("13 MB", BytesToDecimalSiUnits(12500000U));
  EXPECT_EQ("123 MB", BytesToDecimalSiUnits(123499999U));
  EXPECT_EQ("124 MB", BytesToDecimalSiUnits(123500000U));
  EXPECT_EQ("999 MB", BytesToDecimalSiUnits(999499999U));

  EXPECT_EQ("1 GB", BytesToDecimalSiUnits(999500000U));
  EXPECT_EQ("1 GB", BytesToDecimalSiUnits(1000000000U));
  EXPECT_EQ("1 GB", BytesToDecimalSiUnits(1499999999U));
  EXPECT_EQ("2 GB", BytesToDecimalSiUnits(1500000000U));
  EXPECT_EQ("12 GB", BytesToDecimalSiUnits(12499999999U));
  EXPECT_EQ("13 GB", BytesToDecimalSiUnits(12500000000U));
  EXPECT_EQ("123 GB", BytesToDecimalSiUnits(123499999999U));
  EXPECT_EQ("124 GB", BytesToDecimalSiUnits(123500000000U));
  EXPECT_EQ("999 GB", BytesToDecimalSiUnits(999499999999U));

  EXPECT_EQ("1 TB", BytesToDecimalSiUnits(999500000000U));
  EXPECT_EQ("1 TB", BytesToDecimalSiUnits(1000000000000U));
  EXPECT_EQ("1 TB", BytesToDecimalSiUnits(1499999999999U));
  EXPECT_EQ("2 TB", BytesToDecimalSiUnits(1500000000000U));
  EXPECT_EQ("12 TB", BytesToDecimalSiUnits(12499999999999U));
  EXPECT_EQ("13 TB", BytesToDecimalSiUnits(12500000000000U));
  EXPECT_EQ("123 TB", BytesToDecimalSiUnits(123499999999999U));
  EXPECT_EQ("124 TB", BytesToDecimalSiUnits(123500000000000U));
  EXPECT_EQ("999 TB", BytesToDecimalSiUnits(999499999999999U));

  EXPECT_EQ("1 PB", BytesToDecimalSiUnits(999500000000000U));
  EXPECT_EQ("1 PB", BytesToDecimalSiUnits(1000000000000000U));
  EXPECT_EQ("1 PB", BytesToDecimalSiUnits(1499999999999999U));
  EXPECT_EQ("2 PB", BytesToDecimalSiUnits(1500000000000000U));
  EXPECT_EQ("12 PB", BytesToDecimalSiUnits(12499999999999999U));
  EXPECT_EQ("13 PB", BytesToDecimalSiUnits(12500000000000000U));
  EXPECT_EQ("123 PB", BytesToDecimalSiUnits(123499999999999999U));
  EXPECT_EQ("124 PB", BytesToDecimalSiUnits(123500000000000000U));
  EXPECT_EQ("999 PB", BytesToDecimalSiUnits(999499999999999999U));

  EXPECT_EQ("1 EB", BytesToDecimalSiUnits(999500000000000000U));
  EXPECT_EQ("1 EB", BytesToDecimalSiUnits(1000000000000000000U));
  EXPECT_EQ("1 EB", BytesToDecimalSiUnits(1499999999999999999U));
  EXPECT_EQ("2 EB", BytesToDecimalSiUnits(1500000000000000000U));
  EXPECT_EQ("9 EB", BytesToDecimalSiUnits(9499999999999999999U));
  EXPECT_EQ("10 EB", BytesToDecimalSiUnits(9500000000000000000U));
  EXPECT_EQ("12 EB", BytesToDecimalSiUnits(12499999999999999999U));
  EXPECT_EQ("13 EB", BytesToDecimalSiUnits(12500000000000000000U));
  EXPECT_EQ("18 EB", BytesToDecimalSiUnits(18446744073709551615U));
}

TEST(UtilsTest, BEH_BytesToBinarySiUnits) {
  EXPECT_EQ("0 B", BytesToBinarySiUnits(0U));
  EXPECT_EQ("1 B", BytesToBinarySiUnits(1U));
  EXPECT_EQ("12 B", BytesToBinarySiUnits(12U));
  EXPECT_EQ("123 B", BytesToBinarySiUnits(123U));
  EXPECT_EQ("1023 B", BytesToBinarySiUnits(1023U));

  EXPECT_EQ("1 KiB", BytesToBinarySiUnits(1024U));
  EXPECT_EQ("1 KiB", BytesToBinarySiUnits(1535U));
  EXPECT_EQ("2 KiB", BytesToBinarySiUnits(1536U));
  EXPECT_EQ("12 KiB", BytesToBinarySiUnits(12799U));
  EXPECT_EQ("13 KiB", BytesToBinarySiUnits(12800U));
  EXPECT_EQ("123 KiB", BytesToBinarySiUnits(126463U));
  EXPECT_EQ("124 KiB", BytesToBinarySiUnits(126464U));
  EXPECT_EQ("1023 KiB", BytesToBinarySiUnits(1048063U));

  EXPECT_EQ("1 MiB", BytesToBinarySiUnits(1048064U));
  EXPECT_EQ("1 MiB", BytesToBinarySiUnits(1572863U));
  EXPECT_EQ("2 MiB", BytesToBinarySiUnits(1572864U));
  EXPECT_EQ("12 MiB", BytesToBinarySiUnits(13107199U));
  EXPECT_EQ("13 MiB", BytesToBinarySiUnits(13107200U));
  EXPECT_EQ("123 MiB", BytesToBinarySiUnits(129499135U));
  EXPECT_EQ("124 MiB", BytesToBinarySiUnits(129499136U));
  EXPECT_EQ("1023 MiB", BytesToBinarySiUnits(1073217535U));

  EXPECT_EQ("1 GiB", BytesToBinarySiUnits(1073217536U));
  EXPECT_EQ("1 GiB", BytesToBinarySiUnits(1610612735U));
  EXPECT_EQ("2 GiB", BytesToBinarySiUnits(1610612736U));
  EXPECT_EQ("12 GiB", BytesToBinarySiUnits(13421772799U));
  EXPECT_EQ("13 GiB", BytesToBinarySiUnits(13421772800U));
  EXPECT_EQ("123 GiB", BytesToBinarySiUnits(132607115263U));
  EXPECT_EQ("124 GiB", BytesToBinarySiUnits(132607115264U));
  EXPECT_EQ("1023 GiB", BytesToBinarySiUnits(1098974756863U));

  EXPECT_EQ("1 TiB", BytesToBinarySiUnits(1098974756864U));
  EXPECT_EQ("1 TiB", BytesToBinarySiUnits(1649267441663U));
  EXPECT_EQ("2 TiB", BytesToBinarySiUnits(1649267441664U));
  EXPECT_EQ("12 TiB", BytesToBinarySiUnits(13743895347199U));
  EXPECT_EQ("13 TiB", BytesToBinarySiUnits(13743895347200U));
  EXPECT_EQ("123 TiB", BytesToBinarySiUnits(135789686030335U));
  EXPECT_EQ("124 TiB", BytesToBinarySiUnits(135789686030336U));
  EXPECT_EQ("1023 TiB", BytesToBinarySiUnits(1125350151028735U));

  EXPECT_EQ("1 PiB", BytesToBinarySiUnits(1125350151028736U));
  EXPECT_EQ("1 PiB", BytesToBinarySiUnits(1688849860263935U));
  EXPECT_EQ("2 PiB", BytesToBinarySiUnits(1688849860263936U));
  EXPECT_EQ("12 PiB", BytesToBinarySiUnits(14073748835532799U));
  EXPECT_EQ("13 PiB", BytesToBinarySiUnits(14073748835532800U));
  EXPECT_EQ("123 PiB", BytesToBinarySiUnits(139048638495064063U));
  EXPECT_EQ("124 PiB", BytesToBinarySiUnits(139048638495064064U));
  EXPECT_EQ("1023 PiB", BytesToBinarySiUnits(1152358554653425663U));

  EXPECT_EQ("1 EiB", BytesToBinarySiUnits(1152358554653425664U));
  EXPECT_EQ("1 EiB", BytesToBinarySiUnits(1729382256910270463U));
  EXPECT_EQ("2 EiB", BytesToBinarySiUnits(1729382256910270464U));
  EXPECT_EQ("9 EiB", BytesToBinarySiUnits(10952754293765046271U));
  EXPECT_EQ("10 EiB", BytesToBinarySiUnits(10952754293765046272U));
  EXPECT_EQ("15 EiB", BytesToBinarySiUnits(17870283321406128127U));
  EXPECT_EQ("16 EiB", BytesToBinarySiUnits(17870283321406128128U));
  EXPECT_EQ("16 EiB", BytesToBinarySiUnits(18446744073709551615U));
}

TEST(UtilsTest, FUNC_RandomStringMultiThread) {
  int thread_count(20);
#pragma omp parallel num_threads(thread_count)
  {  // NOLINT (dirvine)
    int string_count(1000);
    size_t string_size(4096);
    test::GenerateRandomStrings(string_count, string_size);
  }
  --thread_count;  // to satisfy compiler
}


TEST(UtilsTest, BEH_RandomStringGenerator) {
  std::set<std::string>random_strings;
  const size_t kCount(100);
  const size_t kMaxDuplicates(1);
  for (size_t j = 10; j< 100; ++j) {
    for (size_t i = 0; i < kCount; ++i) {
      random_strings.insert(RandomString(j));
    }
    EXPECT_GE(kMaxDuplicates, kCount - random_strings.size());
    random_strings.clear();
  }
}

TEST(UtilsTest, BEH_Stats) {
  {
    Stats<int> stats;
    EXPECT_EQ(0, stats.Size());
    EXPECT_EQ(0, stats.Min());
    EXPECT_EQ(0, stats.Max());
    EXPECT_EQ(0, stats.Sum());
    EXPECT_EQ(0, stats.Mean());

    stats.Add(1);
    stats.Add(2);
    stats.Add(4);
    stats.Add(5);
    stats.Add(0);

    EXPECT_EQ(5, stats.Size());
    EXPECT_EQ(0, stats.Min());
    EXPECT_EQ(5, stats.Max());
    EXPECT_EQ(12, stats.Sum());
    EXPECT_EQ(2, stats.Mean());
  }
  {
    Stats<float> stats;
    EXPECT_EQ(0, stats.Size());
    EXPECT_FLOAT_EQ(0.0, stats.Min());
    EXPECT_FLOAT_EQ(0.0, stats.Max());
    EXPECT_FLOAT_EQ(0.0, stats.Sum());
    EXPECT_FLOAT_EQ(0.0, stats.Mean());

    stats.Add(1.1f);
    stats.Add(2.2f);
    stats.Add(3.3f);
    stats.Add(4.4f);
    stats.Add(0.0f);

    EXPECT_EQ(5, stats.Size());
    EXPECT_FLOAT_EQ(0.0f, stats.Min());
    EXPECT_FLOAT_EQ(4.4f, stats.Max());
    EXPECT_FLOAT_EQ(11.0f, stats.Sum());
    EXPECT_FLOAT_EQ(2.2f, stats.Mean());
  }
}

TEST(UtilsTest, BEH_IntToString) {
  EXPECT_EQ("1234567890", IntToString(1234567890));
  EXPECT_EQ("-1234567890", IntToString(-1234567890));
  EXPECT_EQ("0", IntToString(0));
  EXPECT_EQ("0", IntToString(-0));
}

TEST(UtilsTest, BEH_RandomStringSingleThread) {
  const size_t kStringSize = 4096;
  std::string test1 = RandomAlphaNumericString(kStringSize);
  std::string test2 = RandomAlphaNumericString(kStringSize);
  EXPECT_EQ(kStringSize, test1.size());
  EXPECT_EQ(kStringSize, test2.size());
  EXPECT_NE(test1, test2);
  const std::string kAlphaNumeric("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefgh"
                                  "ijklmnopqrstuvwxyz");
  for (size_t i = 0; i < kStringSize; ++i) {
    EXPECT_NE(std::string::npos, kAlphaNumeric.find(test1.at(i)));
    EXPECT_NE(std::string::npos, kAlphaNumeric.find(test2.at(i)));
  }
}

TEST(UtilsTest, BEH_HexEncodeDecode) {
  const std::string kKnownEncoded("0123456789abcdef");
  const std::string kKnownDecoded("\x1\x23\x45\x67\x89\xab\xcd\xef");
  EXPECT_EQ(kKnownEncoded, EncodeToHex(kKnownDecoded));
  EXPECT_EQ(kKnownDecoded, DecodeFromHex(kKnownEncoded));
  EXPECT_TRUE(EncodeToHex("").empty());
  EXPECT_TRUE(DecodeFromHex("").empty());
  EXPECT_TRUE(DecodeFromHex("{").empty());
  for (int i = 0; i < 1000; ++i) {
    std::string original = RandomString(100);
    std::string encoded = EncodeToHex(original);
    EXPECT_EQ(200U, encoded.size());
    std::string decoded = DecodeFromHex(encoded);
    EXPECT_EQ(original, decoded);
  }
}

TEST(UtilsTest, BEH_Base64EncodeDecode) {
  const std::string kKnownEncoded("BCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqr"
                                  "stuvwxyz0123456789+/A");
  const std::string kKnownDecoded("\x04\x20\xc4\x14\x61\xc8\x24\xa2\xcc\x34\xe3"
      "\xd0\x45\x24\xd4\x55\x65\xd8\x65\xa6\xdc\x75\xe7\xe0\x86\x28\xe4\x96\x69"
      "\xe8\xa6\xaa\xec\xb6\xeb\xf0\xc7\x2c\xf4\xd7\x6d\xf8\xe7\xae\xfc\xf7\xef"
      "\xc0");
  EXPECT_EQ(kKnownEncoded, EncodeToBase64(kKnownDecoded));
  EXPECT_EQ(kKnownDecoded, DecodeFromBase64(kKnownEncoded));
  EXPECT_TRUE(EncodeToBase64("").empty());
  EXPECT_TRUE(DecodeFromBase64("").empty());
  EXPECT_TRUE(DecodeFromBase64("{").empty());
  for (int i = 0; i < 1000; ++i) {
    std::string original = RandomString(100);
    std::string encoded = EncodeToBase64(original);
    EXPECT_EQ(136U, encoded.size()) << "Encoding failed.";
    std::string decoded = DecodeFromBase64(encoded);
    EXPECT_EQ(original, decoded) << "encoded -> decoded failed.";
  }
}

TEST(UtilsTest, BEH_Base32EncodeDecode) {
  const std::string kKnownEncoded("bcdefghijkmnpqrstuvwxyz23456789a");
  const std::string kKnownDecoded("\x08\x86\x42\x98\xe8\x4a\x96\xc6\xb9\xf0\x8c"
                                  "\xa7\x4a\xda\xf8\xce\xb7\xce\xfb\xe0");
  EXPECT_EQ(kKnownEncoded, EncodeToBase32(kKnownDecoded));
  EXPECT_EQ(kKnownDecoded, DecodeFromBase32(kKnownEncoded));
  EXPECT_TRUE(EncodeToBase32("").empty());
  EXPECT_TRUE(DecodeFromBase32("").empty());
  EXPECT_TRUE(DecodeFromBase32("{").empty());
  for (int i = 0; i < 1000; ++i) {
    std::string original = RandomString(100);
    std::string encoded = EncodeToBase32(original);
    EXPECT_EQ(160U, encoded.size()) << "Encoding failed.";
    std::string decoded = DecodeFromBase32(encoded);
    EXPECT_EQ(original, decoded) << "encoded -> decoded failed.";
  }
}

TEST(UtilsTest, BEH_HexSubstr) {
  EXPECT_TRUE(HexSubstr("").empty());
  EXPECT_EQ("41", HexSubstr("A"));
  EXPECT_EQ("58595a", HexSubstr("XYZ"));
  EXPECT_EQ("6162636465666768", HexSubstr("abcdefgh"));
  EXPECT_EQ("6162636..6676869", HexSubstr("abcdefghi"));
  EXPECT_EQ(16U, HexSubstr(RandomString(8 + RandomUint32() % 20)).size());
}

TEST(UtilsTest, BEH_Base32Substr) {
  EXPECT_TRUE(Base32Substr("").empty());
  EXPECT_EQ("ie", Base32Substr("A"));
  EXPECT_EQ("mbnxw", Base32Substr("XYZ"));
  EXPECT_EQ("nftgg3dfn3vys", Base32Substr("abcdefgh"));
  EXPECT_EQ("nftgg3d..ys4mkpn", Base32Substr("abcdefghijk"));
  EXPECT_EQ(16U, Base32Substr(RandomString(16 + RandomUint32() % 20)).size());
}

TEST(UtilsTest, BEH_Base64Substr) {
  EXPECT_TRUE(Base64Substr("").empty());
  EXPECT_EQ("QQ==", Base64Substr("A"));
  EXPECT_EQ("WFla", Base64Substr("XYZ"));
  EXPECT_EQ("YWJjZGVmZ2g=", Base64Substr("abcdefgh"));
  EXPECT_EQ("YWJjZGV..mtsbW5v", Base64Substr("abcdefghijklmno"));
  EXPECT_EQ(16U, Base64Substr(RandomString(32 + RandomUint32() % 20)).size());
}

TEST(UtilsTest, BEH_TimeFunctions) {
  uint64_t s, ms, ns;
  bptime::time_duration since_epoch(GetDurationSinceEpoch());
  ms = since_epoch.total_milliseconds();
  ns = since_epoch.total_nanoseconds();
  s = since_epoch.total_seconds();
  EXPECT_EQ(s, ms / 1000) << "s vs. ms failed.";
  EXPECT_EQ(s, ns / 1000000000) << "s vs. ns failed.";
  EXPECT_EQ(ms, ns / 1000000) << "ms vs. ns failed.";
}

TEST(UtilsTest, BEH_RandomNumberGen) {
  std::set<int32_t>random_ints;
  std::set<uint32_t>random_uints;
  const size_t kCount(10000);
  // look for less than 0.05% duplicates
  const size_t kMaxDuplicates(kCount / 2000);
  for (size_t i = 0; i < kCount; ++i) {
    random_ints.insert(RandomInt32());
    random_uints.insert(RandomUint32());
  }
  EXPECT_GE(kMaxDuplicates, kCount - random_ints.size());
  EXPECT_GE(kMaxDuplicates, kCount - random_uints.size());
}

TEST(UtilsTest, BEH_ReadWriteFile) {
  TestPath test_path(CreateTestPath("MaidSafe_TestUtils"));
  fs::path file_path(*test_path / "file.dat");
  std::string file_content;
  ASSERT_FALSE(fs::exists(file_path));
  EXPECT_FALSE(ReadFile(file_path, NULL));
  EXPECT_FALSE(ReadFile(file_path, &file_content));
  EXPECT_TRUE(file_content.empty());

  EXPECT_FALSE(WriteFile("", file_content));
  EXPECT_TRUE(WriteFile(file_path, file_content));
  EXPECT_TRUE(fs::exists(file_path));
  EXPECT_EQ(0, fs::file_size(file_path));
  EXPECT_FALSE(ReadFile(file_path, NULL));
  EXPECT_TRUE(ReadFile(file_path, &file_content));
  EXPECT_TRUE(file_content.empty());

  file_content = RandomString(3000 + RandomUint32() % 1000);
  EXPECT_TRUE(WriteFile(file_path, file_content));
  EXPECT_EQ(crypto::Hash<crypto::SHA512>(file_content),
            crypto::HashFile<crypto::SHA512>(file_path));

  std::string file_content_in;
  EXPECT_TRUE(ReadFile(file_path, &file_content_in));
  EXPECT_EQ(file_content, file_content_in);

  EXPECT_TRUE(WriteFile(file_path, "moo"));
  EXPECT_TRUE(ReadFile(file_path, &file_content_in));
  EXPECT_EQ("moo", file_content_in);
}

TEST(UtilsTest, BEH_Sleep) {
  bptime::ptime first_time(bptime::microsec_clock::universal_time());
  bptime::ptime second_time(bptime::microsec_clock::universal_time());
  EXPECT_LT((second_time - first_time).total_milliseconds(), 100);
  Sleep(bptime::milliseconds(100));
  bptime::ptime third_time(bptime::microsec_clock::universal_time());
  EXPECT_GE((third_time - first_time).total_milliseconds(), 100);
}

TEST(UtilsTest, BEH_GetMaidSafeVersion) {
  EXPECT_EQ("v0.00.00", GetMaidSafeVersion(0));
  EXPECT_EQ("v0.00.01", GetMaidSafeVersion(1));
  EXPECT_EQ("v0.00.10", GetMaidSafeVersion(10));
  EXPECT_EQ("v0.01.00", GetMaidSafeVersion(100));
  EXPECT_EQ("v0.01.01", GetMaidSafeVersion(101));
  EXPECT_EQ("v0.01.10", GetMaidSafeVersion(110));
  EXPECT_EQ("v0.10.00", GetMaidSafeVersion(1000));
  EXPECT_EQ("v0.10.01", GetMaidSafeVersion(1001));
  EXPECT_EQ("v0.10.10", GetMaidSafeVersion(1010));
  EXPECT_EQ("v1.00.00", GetMaidSafeVersion(10000));
  EXPECT_EQ("v1.00.01", GetMaidSafeVersion(10001));
  EXPECT_EQ("v1.00.10", GetMaidSafeVersion(10010));
  EXPECT_EQ("v1.01.00", GetMaidSafeVersion(10100));
  EXPECT_EQ("v1.01.01", GetMaidSafeVersion(10101));
  EXPECT_EQ("v1.01.10", GetMaidSafeVersion(10110));
  EXPECT_EQ("v1.10.00", GetMaidSafeVersion(11000));
  EXPECT_EQ("v1.10.01", GetMaidSafeVersion(11001));
  EXPECT_EQ("v1.10.10", GetMaidSafeVersion(11010));
  EXPECT_EQ("v10.00.00", GetMaidSafeVersion(100000));
  EXPECT_EQ("v10.00.01", GetMaidSafeVersion(100001));
  EXPECT_EQ("v10.00.10", GetMaidSafeVersion(100010));
  EXPECT_EQ("v10.01.00", GetMaidSafeVersion(100100));
  EXPECT_EQ("v10.01.01", GetMaidSafeVersion(100101));
  EXPECT_EQ("v10.01.10", GetMaidSafeVersion(100110));
  EXPECT_EQ("v10.10.00", GetMaidSafeVersion(101000));
  EXPECT_EQ("v10.10.01", GetMaidSafeVersion(101001));
  EXPECT_EQ("v10.10.10", GetMaidSafeVersion(101010));
  std::string major_version, minor_version, patch_version;
  EXPECT_EQ("v1.01.01", GetMaidSafeVersion(10101, &major_version,
                                           &minor_version, &patch_version));
  EXPECT_EQ("1", major_version);
  EXPECT_EQ("01", minor_version);
  EXPECT_EQ("01", patch_version);
  EXPECT_EQ("v12.34.56", GetMaidSafeVersion(123456, &major_version,
                                            &minor_version, &patch_version));
  EXPECT_EQ("12", major_version);
  EXPECT_EQ("34", minor_version);
  EXPECT_EQ("56", patch_version);
}

TEST(UtilsTest, BEH_GetHomeDir) {
  EXPECT_FALSE(GetHomeDir().empty());
  DLOG(INFO) << " I think your home directory is "
             << GetHomeDir().string();
}

TEST(UtilsTest, BEH_GetUserAppDir) {
  EXPECT_FALSE(GetUserAppDir().empty());
  DLOG(INFO) << " I think your user app directory is "
             << GetUserAppDir().string();
}

TEST(UtilsTest, BEH_GetSystemAppDir) {
  EXPECT_FALSE(GetSystemAppDir().empty());
  DLOG(INFO) << " I think your System app directory is "
             << GetSystemAppDir().string();
}

TEST(UtilsTest, BEH_AppDir) {
  EXPECT_NE(GetSystemAppDir(), GetUserAppDir());
  EXPECT_NE(GetSystemAppDir(), GetHomeDir());
  EXPECT_NE(GetUserAppDir(), GetHomeDir());
  std::string home(GetHomeDir().string());
  std::string system(GetSystemAppDir().string());
  std::string user_app(GetUserAppDir().string());
  EXPECT_TRUE(user_app.find(home) != std::string::npos);
  EXPECT_TRUE(system.find(home) == std::string::npos);
}


namespace {

void CleanupTest(fs::path *&test_path) {
  if (!test_path->empty()) {
    boost::system::error_code error_code;
    if (fs::remove_all(*test_path, error_code) == 0)
      LOG(WARNING) << "Test directory " << *test_path << " already deleted.";
    if (error_code)
      LOG(WARNING) << "Failed to clean up test directory " << *test_path
                   << "  (error message: " << error_code.message() << ")";
  }
  delete test_path;
  test_path = NULL;
}

}  // unnamed namespace

TEST(UtilsTest, BEH_CreateTestPath) {
  fs::path test_path;
  boost::system::error_code error_code;
  {
    TestPath test_path_ptr(CreateTestPath());
    test_path = *test_path_ptr;
    EXPECT_FALSE(test_path.empty());
    EXPECT_TRUE(fs::exists(test_path, error_code));
    EXPECT_EQ(0, error_code.value()) << error_code.message();
  }
  EXPECT_FALSE(fs::exists(test_path, error_code));
  EXPECT_EQ(boost::system::errc::no_such_file_or_directory, error_code.value())
        << error_code.message();
  {
    TestPath test_path_ptr(CreateTestPath("Another"));
    test_path = *test_path_ptr;
    EXPECT_FALSE(test_path.empty());
    EXPECT_TRUE(fs::exists(test_path, error_code));
    EXPECT_EQ(0, error_code.value()) << error_code.message();
  }
  EXPECT_FALSE(fs::exists(test_path, error_code));
  EXPECT_EQ(boost::system::errc::no_such_file_or_directory, error_code.value())
        << error_code.message();
  // Ensure we're able to cope with error cases
  fs::path *empty_path(new fs::path);
  CleanupTest(empty_path);
  EXPECT_TRUE(NULL == empty_path);
  fs::path *non_existent(new fs::path(std::string(100, 'a')));
  CleanupTest(non_existent);
  EXPECT_TRUE(NULL == non_existent);
}

}  // namespace test

}  // namespace maidsafe
