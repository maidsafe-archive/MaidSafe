/*******************************************************************************
 *  Copyright 2012 MaidSafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of MaidSafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of MaidSafe.net. *
 ******************************************************************************/

#ifndef MAIDSAFE_RUDP_RETURN_CODES_H_
#define MAIDSAFE_RUDP_RETURN_CODES_H_

#include "maidsafe/rudp/version.h"

#if MAIDSAFE_RUDP_VERSION != 100
#  error This API is not compatible with the installed library.\
    Please update the maidsafe_rudp library.
#endif


namespace maidsafe {

namespace rudp {

enum ReturnCode {
  kSuccess = 0,
  kError = -350001,
  kRemoteUnreachable = -350002,
  kNoConnection = -350003,
  kNoNetwork = -350004,
  kInvalidIP = -350005,
  kInvalidPort = -350006,
  kInvalidData = -350007,
  kNoSocket = -350008,
  kInvalidAddress = -350009,
  kNoRendezvous = -350010,
  kBehindFirewall = -350011,
  kBindError = -350012,
  kConnectError = -350013,
  kAlreadyStarted = -350014,
  kListenError = -350015,
  kCloseSocketError = -350016,
  kSendFailure = -350017,
  kSendTimeout = -350018,
  kSendStalled = -350019,
  kSendParseFailure = -350020,
  kSendSizeFailure = -350021,
  kReceiveFailure = -350022,
  kReceiveTimeout = -350023,
  kReceiveStalled = -350024,
  kReceiveParseFailure = -350025,
  kReceiveSizeFailure = -350026,
  kAddManagedEndpointError = -350027,
  kAddManagedEndpointTimedOut = -350028,
  kManagedEndpointLost = -350029,
  kSetOptionFailure = -350030,
  kMessageSizeTooLarge = -350031,
  kWrongIpVersion = -350032,
  kPendingResult = -350033,

  // Managed Connections
  kNoneAvailable = -350100,
  kFull = -350101,
  kNullParameter = -350102,
  kInvalidTransport = -350103,
  kInvalidConnection = -350104,
  kConnectionAlreadyExists = -350105,

  // Upper limit of values for this enum.
  kReturnCodeLimit = -359999
};

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_RETURN_CODES_H_
