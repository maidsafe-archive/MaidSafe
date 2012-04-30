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
// Original author: Christopher M. Kohlhoff (chris at kohlhoff dot com)

#ifndef MAIDSAFE_RUDP_PACKETS_PACKET_H_
#define MAIDSAFE_RUDP_PACKETS_PACKET_H_

#include <cstdint>

#include "boost/asio/buffer.hpp"

namespace maidsafe {

namespace rudp {

namespace detail {

class Packet {
 public:
  // Get the destination socket id from an encoded packet.
  static bool DecodeDestinationSocketId(uint32_t *id,
                                        const boost::asio::const_buffer &data);

 protected:
  // Prevent deletion through this type.
  virtual ~Packet();

  // Helper functions for encoding and decoding integers.
  static void DecodeUint32(uint32_t *n, const unsigned char *p);
  static void EncodeUint32(uint32_t n, unsigned char *p);
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_PACKETS_PACKET_H_
