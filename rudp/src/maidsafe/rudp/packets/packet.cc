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

#include "maidsafe/rudp/packets/packet.h"

namespace asio = boost::asio;

namespace maidsafe {

namespace rudp {

namespace detail {

Packet::~Packet() {
}

bool Packet::DecodeDestinationSocketId(uint32_t *id,
                                       const asio::const_buffer &data) {
  // Refuse to decode anything that's too short.
  if (asio::buffer_size(data) < 16)
    return false;

  DecodeUint32(id, asio::buffer_cast<const unsigned char*>(data) + 12);
  return true;
}

void Packet::DecodeUint32(uint32_t *n, const unsigned char *p) {
  *n = p[0];
  *n = ((*n << 8) | p[1]);
  *n = ((*n << 8) | p[2]);
  *n = ((*n << 8) | p[3]);
}

void Packet::EncodeUint32(uint32_t n, unsigned char *p) {
  p[0] = ((n >> 24) & 0xff);
  p[1] = ((n >> 16) & 0xff);
  p[2] = ((n >> 8) & 0xff);
  p[3] = (n & 0xff);
}

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe
