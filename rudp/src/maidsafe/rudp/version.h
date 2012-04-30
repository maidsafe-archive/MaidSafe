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

#ifndef MAIDSAFE_RUDP_VERSION_H_
#define MAIDSAFE_RUDP_VERSION_H_

#define MAIDSAFE_RUDP_VERSION 100

#if defined CMAKE_MAIDSAFE_RUDP_VERSION &&\
            MAIDSAFE_RUDP_VERSION != CMAKE_MAIDSAFE_RUDP_VERSION
#  error The project version has changed.  Re-run CMake.
#endif

#include "maidsafe/common/version.h"

#define THIS_NEEDS_MAIDSAFE_COMMON_VERSION 1200
#if MAIDSAFE_COMMON_VERSION < THIS_NEEDS_MAIDSAFE_COMMON_VERSION
#  error This API is not compatible with the installed library.\
    Please update the maidsafe-common library.
#elif MAIDSAFE_COMMON_VERSION > THIS_NEEDS_MAIDSAFE_COMMON_VERSION
#  error This API uses a newer version of the maidsafe-common library.\
    Please update this project. (MAIDSAFE_COMMON_VERSION)
#endif

#endif  // MAIDSAFE_RUDP_VERSION_H_
