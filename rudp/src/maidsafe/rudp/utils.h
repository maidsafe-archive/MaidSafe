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

#ifndef MAIDSAFE_RUDP_UTILS_H_
#define MAIDSAFE_RUDP_UTILS_H_

#include <cstdint>
#include <string>
#include <vector>

#include "boost/asio/ip/address.hpp"
#include "boost/asio/ip/udp.hpp"

#include "maidsafe/rudp/common.h"


namespace maidsafe {

namespace rudp {

// Tries to connect a socket to one of peer_endpoints.  If peer_endpoints is
// empty, will then try to connect to one of the hard-coded default endpoints,
// unless use_defaults is false.  The first successful connection attempt will
// cause the function to return true and provide this node's local IP address.
// If use_defaults is false, only the first peer_endpoint will be attempted.
bool TryConnectTo(std::vector<boost::asio::ip::udp::endpoint> peer_endpoints,
                  bool use_defaults = true,
                  boost::asio::ip::address *local_ip = nullptr);

// Returns true if 1024 < port < 49151 and the address is correctly specified.
bool IsValid(const Endpoint &endpoint);

// // Convert an IP in ASCII format to IPv4 or IPv6 bytes
// std::string IpAsciiToBytes(const std::string &decimal_ip);
//
// // Convert an IPv4 or IPv6 in bytes format to ASCII format
// std::string IpBytesToAscii(const std::string &bytes_ip);
//
// // Convert an internet network address into dotted string format.
// void IpNetToAscii(uint32_t address, char *ip_buffer);
//
// // Convert a dotted string format internet address into Ipv4 format.
// uint32_t IpAsciiToNet(const char *buffer);
//
// // Returns a random port number in the valid range (1024 < port < 49151).
// uint16_t GetRandomPort();

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_UTILS_H_
