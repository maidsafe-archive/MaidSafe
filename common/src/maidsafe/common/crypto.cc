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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" ANDp
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

#include "maidsafe/common/crypto.h"
#include <memory>
#include <algorithm>
#include <vector>

#ifdef __MSVC__
#  pragma warning(push, 1)
#  pragma warning(disable: 4702)
#endif

#include "boost/scoped_array.hpp"
#include "cryptopp/gzip.h"
#include "cryptopp/hex.h"
#include "cryptopp/aes.h"
#include "cryptopp/modes.h"
#include "cryptopp/osrng.h"
#include "cryptopp/pssr.h"
#include  "cryptopp/ida.h"
#include "cryptopp/pwdbased.h"
#include "cryptopp/cryptlib.h"

#ifdef __MSVC__
#  pragma warning(pop)
#endif
#include "boost/thread/mutex.hpp"

#include "maidsafe/common/log.h"
#include "maidsafe/common/platform_config.h"
#include "maidsafe/common/return_codes.h"
#include "maidsafe/common/utils.h"

namespace maidsafe {

namespace crypto {

namespace {

CryptoPP::RandomNumberGenerator &rng() {
  static CryptoPP::AutoSeededRandomPool random_number_generator;
  return random_number_generator;
}

}  // unamed namespace

std::string XOR(const std::string &first, const std::string &second) {
  size_t common_size(first.size());
  if ((common_size != second.size()) || (common_size == 0)) {
    DLOG(WARNING) << "Size mismatch or zero.";
    return "";
  }

  boost::scoped_array<char> first_char(new char[common_size]);
  std::copy(first.begin(), first.end(), first_char.get());
  boost::scoped_array<char> second_char(new char[common_size]);
  std::copy(second.begin(), second.end(), second_char.get());

  boost::scoped_array<char> buffer(new char[common_size]);
  for (size_t i = 0; i < common_size; ++i)
    buffer[i] = first_char[i] ^ second_char[i];

  std::string result(buffer.get(), common_size);
  return result;
}

int SecurePassword(const std::string &password,
                           const std::string &salt,
                           const uint32_t &pin,
                           std::string *derived_password,
                           const std::string &label) {
  if (password.empty() || salt.empty() || pin == 0 || label.empty()) {
    DLOG(WARNING) << "Invalid parameter.";
    return kGeneralError;
  }
  uint16_t iter = (pin % 10000) + 10000;
  CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA512> pbkdf;
  CryptoPP::SecByteBlock derived(AES256_KeySize + AES256_IVSize);
  byte purpose = 0;  // unused in this pbkdf implementation
  CryptoPP::SecByteBlock context(salt.size() + label.size());
  std::copy_n(salt.data(), salt.size(), &context[0]);
  std::copy_n(label.data(),  label.size(), &context[salt.size()]);
  pbkdf.DeriveKey(derived, derived.size(), purpose,
                  reinterpret_cast<const byte*>(password.data()),
                  password.size(), context.data(), context.size(), iter);
  CryptoPP::StringSink string_sink(*derived_password);
  string_sink.Put(derived, derived.size());
  return kSuccess;
}

std::string SymmEncrypt(const std::string &input,
                        const std::string &key,
                        const std::string &initialisation_vector) {
  if (key.size() < AES256_KeySize ||
      initialisation_vector.size() < AES256_IVSize) {
    DLOG(WARNING) << "Undersized key or IV.";
    return "";
  }

  try {
    byte byte_key[AES256_KeySize], byte_iv[AES256_IVSize];

    CryptoPP::StringSource(key.substr(0, AES256_KeySize), true,
        new CryptoPP::ArraySink(byte_key, sizeof(byte_key)));

    CryptoPP::StringSource(initialisation_vector.substr(0, AES256_IVSize), true,
        new CryptoPP::ArraySink(byte_iv, sizeof(byte_iv)));

    std::string result;
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryptor(byte_key,
        sizeof(byte_key), byte_iv);
    CryptoPP::StringSource(input, true,
        new CryptoPP::StreamTransformationFilter(encryptor,
            new CryptoPP::StringSink(result)));
    return result;
  }
  catch(const CryptoPP::Exception &e) {
    DLOG(ERROR) << "Failed symmetric encryption: " << e.what();
    return "";
  }
}

std::string SymmDecrypt(const std::string &input,
                        const std::string &key,
                        const std::string &initialisation_vector) {
  if (key.size() < AES256_KeySize ||
      initialisation_vector.size() < AES256_IVSize ||
      input.empty()) {
    DLOG(WARNING) << "Undersized key or IV or input.";
    return "";
  }

  try {
    byte byte_key[AES256_KeySize], byte_iv[AES256_IVSize];

    CryptoPP::StringSource(key.substr(0, AES256_KeySize), true,
        new CryptoPP::ArraySink(byte_key, sizeof(byte_key)));

    CryptoPP::StringSource(initialisation_vector.substr(0, AES256_IVSize), true,
        new CryptoPP::ArraySink(byte_iv, sizeof(byte_iv)));

    std::string result;
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryptor(byte_key,
        sizeof(byte_key), byte_iv);
    CryptoPP::StringSource(input, true,
        new CryptoPP::StreamTransformationFilter(decryptor,
            new CryptoPP::StringSink(result)));
    return result;
  }
  catch(const CryptoPP::Exception &e) {
    DLOG(ERROR) << "Failed symmetric decryption: " << e.what();
    return "";
  }
}

std::string Compress(const std::string &input,
                     const uint16_t &compression_level) {
  if (compression_level > kMaxCompressionLevel)
    return "";
  try {
    std::string result;
    CryptoPP::StringSource(input, true, new CryptoPP::Gzip(
        new CryptoPP::StringSink(result), compression_level));
    return result;
  }
  catch(const CryptoPP::Exception &e) {
    DLOG(ERROR) << "Failed compressing: " << e.what();
    return "";
  }
}

std::string Uncompress(const std::string &input) {
  try {
    std::string result;
    CryptoPP::StringSource(input, true, new CryptoPP::Gunzip(
        new CryptoPP::StringSink(result)));
    return result;
  }
  catch(const CryptoPP::Exception &e) {
    DLOG(ERROR) << "Failed uncompressing: " << e.what();
    return "";
  }
}

void SecretShareData(const uint8_t &threshold,
                     const uint8_t &nShares,
                     const std::string &data,
                     std::vector<std::string> *out_strings) {
CryptoPP::ChannelSwitch * channelswitch = new CryptoPP::ChannelSwitch;
CryptoPP::StringSource source(
    data,
    false,
    new CryptoPP::SecretSharing(rng(), threshold, nShares, channelswitch));
  out_strings->resize(nShares);
  CryptoPP::vector_member_ptrs<CryptoPP::StringSink> string_sink(nShares);
  std::string channel;

  for (int i = 0; i < nShares; ++i) {
    string_sink[i].reset(new CryptoPP::StringSink(out_strings->at(i)));
     channel = CryptoPP::WordToString<CryptoPP::word32>(i);
     string_sink[i]->Put(const_cast<byte *>(reinterpret_cast<const byte *>
                                                     (channel.data())), 4);
    // see http://www.cryptopp.com/wiki/ChannelSwitch
    channelswitch->AddRoute(channel,
                            *string_sink[i],
                            CryptoPP::DEFAULT_CHANNEL);
  }
  source.PumpAll();
}

void SecretRecoverData(const uint8_t &threshold,
                       const std::vector<std::string> &in_strings,
                       std::string *data) {
  uint8_t size(static_cast<uint8_t>(in_strings.size()));
  uint8_t num_to_check = std::min(size, threshold);

  CryptoPP::SecretRecovery recovery(num_to_check,
                                    new CryptoPP::StringSink(*data));

  CryptoPP::vector_member_ptrs<CryptoPP::StringSource>
                              string_sources(num_to_check);
  CryptoPP::SecByteBlock channel(4);

  for (auto i = 0 ; i < num_to_check ; ++i) {
    string_sources[i].reset(new CryptoPP::StringSource(in_strings.at(i),
                                                       false));
    string_sources[i]->Pump(4);
    string_sources[i]->Get(channel, 4);
    string_sources[i]->Attach(new CryptoPP::ChannelSwitch(recovery,
                    std::string(reinterpret_cast<char *>(channel.begin()), 4)));
  }
    while (string_sources[0]->Pump(256))
      for (auto i = 1; i < num_to_check; i++)
        string_sources[i]->Pump(256);

  for (auto i = 0; i < num_to_check; i++)
    string_sources[i]->PumpAll();
}

}  // namespace crypto

}  // namespace maidsafe
