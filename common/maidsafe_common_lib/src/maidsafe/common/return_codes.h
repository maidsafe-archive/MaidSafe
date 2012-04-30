/*******************************************************************************
 *  Copyright 2011 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 *******************************************************************************
 */

#ifndef MAIDSAFE_COMMON_RETURN_CODES_H_
#define MAIDSAFE_COMMON_RETURN_CODES_H_

#include "maidsafe/common/version.h"

#if MAIDSAFE_COMMON_VERSION != 1200
# error This API is not compatible with the installed library.\
Please update the library.
#endif

namespace maidsafe {

enum CommonReturnCode {
  kSuccess = 0,
  kGeneralError = -100001,
  kNullParameter = -100002,
  kFailedSymmDecrypt = -100003,

  // RSA error codes
  kKeyGenerationError = -100100,
  kDataEmpty = -100101,
  kInvalidPublicKey = -100102,
  kInvalidPrivateKey = -100103,
  kRSAEncryptError = -100104,
  kRSADecryptError = -100105,
  kRSASigningError = -100106,
  kRSAInvalidSignature = -100107,
  kRSASignatureEmpty = -100108,
  kRSASerialisationError = -100109,
  kRSAParseError = -100110
};

}  // namespace maidsafe

#endif  // MAIDSAFE_COMMON_RETURN_CODES_H_
