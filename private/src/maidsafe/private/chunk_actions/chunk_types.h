/*
* ============================================================================
*
* Copyright [2011] maidsafe.net limited
*
* The following source code is property of maidsafe.net limited and is not
* meant for external use.  The use of this code is governed by the license
* file licence.txt found in the root of this directory and also on
* www.maidsafe.net.
*
* You are not free to copy, amend or otherwise use this source code without
* the explicit written permission of the board of directors of maidsafe.net.
*
* ============================================================================
*/

#ifndef MAIDSAFE_PRIVATE_CHUNK_ACTIONS_CHUNK_TYPES_H_
#define MAIDSAFE_PRIVATE_CHUNK_ACTIONS_CHUNK_TYPES_H_

#include "maidsafe/private/version.h"

#if MAIDSAFE_PRIVATE_VERSION != 300
#  error This API is not compatible with the installed library.\
    Please update the library.
#endif

namespace maidsafe {

namespace priv {

namespace chunk_actions {

const unsigned char kDefaultType(0);
const unsigned char kAppendableByAll(1);
const unsigned char kModifiableByOwner(2);
const unsigned char kSignaturePacket(3);
const unsigned char kUnknownType(255);

}  // namespace chunk_actions

}  // namespace priv

}  // namespace maidsafe

#endif  // MAIDSAFE_PRIVATE_CHUNK_ACTIONS_CHUNK_TYPES_H_
