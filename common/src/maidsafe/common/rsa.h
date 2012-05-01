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

#ifndef MAIDSAFE_COMMON_RSA_H_
#define MAIDSAFE_COMMON_RSA_H_

#include <string>

#ifdef __MSVC__
#  pragma warning(push, 1)
#  pragma warning(disable: 4702)
#endif
#include "cryptopp/channels.h"
#include "cryptopp/ida.h"
#include "cryptopp/rsa.h"
#ifdef __MSVC__
#  pragma warning(pop)
#  pragma warning(disable: 4505)
#endif

#include "boost/function.hpp"
#include "boost/serialization/string.hpp"

#include "maidsafe/common/version.h"
#if MAIDSAFE_COMMON_VERSION != 1200
#  error This API is not compatible with the installed library.\
    Please update the MaidSafe-Common library.
#endif

namespace maidsafe {

namespace rsa {

typedef CryptoPP::RSA::PrivateKey PrivateKey;
typedef CryptoPP::RSA::PublicKey PublicKey;
typedef std::string ValidationToken, Identity, PlainText, Signature, CipherText;
typedef std::function<void(PublicKey,
                           ValidationToken)> GetPublicKeyAndValidationCallback;
typedef std::function<void(Identity,
                           GetPublicKeyAndValidationCallback)>
    GetPublicKeyAndValidationFunctor;
typedef std::function<bool(Identity,                          // NOLINT (Fraser)
                           PublicKey,
                           ValidationToken)> ValidatePublicKeyFunctor;
typedef std::function<bool(PlainText, Signature, PublicKey)> ValidateFunctor;  // NOLINT (Fraser)

struct Keys {
 public:
  enum { kKeySize = 2048 };
  Keys() : identity(), private_key(), public_key(), validation_token() {}
  Identity identity;
  PrivateKey private_key;
  PublicKey public_key;
  ValidationToken validation_token;  // certificate, additional signature etc.
};

int GenerateKeyPair(Keys *keypair);

int Encrypt(const PlainText &plain_text,
            const PublicKey &public_key,
            CipherText *result);

int Decrypt(const CipherText &data,
            const PrivateKey &private_key,
            PlainText *result);

int Sign(const PlainText &data,
         const PrivateKey &private_key,
         Signature *signature);

int CheckSignature(const PlainText &data,
                   const Signature &signature,
                   const PublicKey &public_key);

void EncodePrivateKey(const PrivateKey &key, std::string *private_key);

void EncodePublicKey(const PublicKey &key, std::string *public_key);

void DecodePrivateKey(const std::string &private_key, PrivateKey *key);

void DecodePublicKey(const std::string &public_key, PublicKey *key);

// check decoded keys were the same as encoded and pub key not replaced
bool CheckRoundtrip(const PublicKey &public_key,
                    const PrivateKey &private_key);

bool ValidateKey(const PrivateKey &private_key, unsigned int level = 2U);

bool ValidateKey(const PublicKey &public_key, unsigned int level = 2U);

bool Validate(const PlainText &plain_text,
              const Signature &signature,
              const PublicKey &public_key);

bool MatchingPublicKeys(const PublicKey &public_key1,
                        const PublicKey &public_key2);

bool MatchingPrivateKeys(const PrivateKey &private_key1,
                         const PrivateKey &private_key2);

}  // namespace rsa

namespace asymm = rsa;

}  // namespace maidsafe


namespace boost {

namespace serialization {

#ifdef __MSVC__
#  pragma warning(disable: 4127)
#endif
template <typename Archive>
void serialize(Archive &archive,                              // NOLINT (Fraser)
               maidsafe::rsa::PrivateKey &private_key,
               const unsigned int& /*version*/) {
  std::string encoded_private_key;
  if (Archive::is_saving::value)
    maidsafe::rsa::EncodePrivateKey(private_key, &encoded_private_key);
  archive & encoded_private_key;
  if (Archive::is_loading::value)
    maidsafe::rsa::DecodePrivateKey(encoded_private_key, &private_key);
#ifdef __MSVC__
#  pragma warning(default: 4127)
#endif
}

#ifdef __MSVC__
#  pragma warning(disable: 4127)
#endif
template <typename Archive>
void serialize(Archive &archive,                              // NOLINT (Fraser)
               maidsafe::rsa::PublicKey &public_key,
               const unsigned int& /*version*/) {
  std::string encoded_public_key;
  if (Archive::is_saving::value)
    maidsafe::rsa::EncodePublicKey(public_key, &encoded_public_key);
  archive & encoded_public_key;
  if (Archive::is_loading::value)
    maidsafe::rsa::DecodePublicKey(encoded_public_key, &public_key);
#ifdef __MSVC__
#  pragma warning(default: 4127)
#endif
}

}  // namespace serialization

}  // namespace boost


#endif  // MAIDSAFE_COMMON_RSA_H_
