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

#include "maidsafe/rudp/core/session.h"

#include <cassert>

#include "maidsafe/rudp/packets/data_packet.h"
#include "maidsafe/rudp/core/peer.h"
#include "maidsafe/rudp/core/sliding_window.h"
#include "maidsafe/rudp/core/tick_timer.h"
#include "maidsafe/rudp/log.h"
#include "maidsafe/rudp/utils.h"

namespace bptime = boost::posix_time;


namespace maidsafe {

namespace rudp {

namespace detail {

Session::Session(Peer &peer,                                  // NOLINT (Fraser)
                 TickTimer &tick_timer,
                 boost::asio::ip::udp::endpoint &this_external_endpoint)
    : peer_(peer),
      tick_timer_(tick_timer),
      this_external_endpoint_(this_external_endpoint),
      id_(0),
      sending_sequence_number_(0),
      receiving_sequence_number_(0),
      peer_connection_type_(0),
      state_(kClosed) {}

void Session::Open(uint32_t id, uint32_t sequence_number) {
  assert(id != 0);
  id_ = id;
  sending_sequence_number_ = sequence_number;
  state_ = kProbing;
  SendConnectionRequest();
}

bool Session::IsOpen() const {
  return state_ != kClosed;
}

bool Session::IsConnected() const {
  return state_ == kConnected;
}

uint32_t Session::Id() const {
  return id_;
}

uint32_t Session::ReceivingSequenceNumber() const {
  return receiving_sequence_number_;
}

uint32_t Session::PeerConnectionType() const {
  return peer_connection_type_;
}

void Session::Close() {
  state_ = kClosed;
}

void Session::HandleHandshake(const HandshakePacket &packet) {
  if (peer_.Id() == 0) {
    peer_.SetId(packet.SocketId());
  }

  // TODO(Fraser#5#): 2012-04-04 - Check if we need to uncomment the lines below
  if (state_ == kProbing) {
//    if (packet.ConnectionType() == 1 && packet.SynCookie() == 0)
    state_ = kHandshaking;
    SendCookie();
  } else if (state_ == kHandshaking) {
//    if (packet.SynCookie() == 1) {
    state_ = kConnected;
    peer_connection_type_ = packet.ConnectionType();
    receiving_sequence_number_ = packet.InitialPacketSequenceNumber();
    SendConnectionAccepted();
    if (IsValid(packet.Endpoint())) {
      if (!this_external_endpoint_.address().is_unspecified() &&
          this_external_endpoint_ != packet.Endpoint()) {
        DLOG(ERROR) << "External endpoint currently " << this_external_endpoint_
                    << " - about to be set to " << packet.Endpoint();
      }
      this_external_endpoint_ = packet.Endpoint();
    } else {
      DLOG(ERROR) << "Invalid external endpoint in handshake: "
                  << packet.Endpoint();
    }
//    }
  }
}

void Session::HandleTick() {
  if (state_ == kProbing) {
    SendConnectionRequest();
  } else if (state_ == kHandshaking) {
    SendCookie();
  }
}

void Session::SendConnectionRequest() {
  HandshakePacket packet;
  packet.SetRudpVersion(4);
  packet.SetSocketType(HandshakePacket::kStreamSocketType);
  packet.SetSocketId(id_);
  packet.SetEndpoint(peer_.Endpoint());
  packet.SetDestinationSocketId(0);
  packet.SetConnectionType(1);

  int result(peer_.Send(packet));
  if (result != kSuccess)
    DLOG(ERROR) << "Failed to send handshake to " << peer_.Endpoint();

  // Schedule another connection request.
  tick_timer_.TickAfter(bptime::milliseconds(250));
}

void Session::SendCookie() {
  HandshakePacket packet;
  packet.SetEndpoint(peer_.Endpoint());
  packet.SetDestinationSocketId(peer_.Id());
  packet.SetRudpVersion(4);
  packet.SetSocketType(HandshakePacket::kStreamSocketType);
  packet.SetInitialPacketSequenceNumber(sending_sequence_number_);
  packet.SetMaximumPacketSize(Parameters::max_size);
  packet.SetMaximumFlowWindowSize(Parameters::maximum_window_size);
  packet.SetConnectionType(Parameters::connection_type);
  packet.SetSocketId(id_);
  packet.SetSynCookie(1);  // TODO(Team) calculate cookie

  int result(peer_.Send(packet));
  if (result != kSuccess)
    DLOG(ERROR) << "Failed to send cookie to " << peer_.Endpoint();

  // Schedule another cookie send.
  tick_timer_.TickAfter(bptime::milliseconds(250));
}

void Session::SendConnectionAccepted() {
  HandshakePacket packet;
  packet.SetEndpoint(peer_.Endpoint());
  packet.SetDestinationSocketId(peer_.Id());
  packet.SetRudpVersion(4);
  packet.SetSocketType(HandshakePacket::kStreamSocketType);
  packet.SetInitialPacketSequenceNumber(sending_sequence_number_);
  packet.SetMaximumPacketSize(Parameters::max_size);
  packet.SetMaximumFlowWindowSize(Parameters::maximum_window_size);
  packet.SetConnectionType(Parameters::connection_type);
  packet.SetSocketId(id_);
  packet.SetSynCookie(1);  // TODO(Team) calculate cookie

  int result(peer_.Send(packet));
  if (result != kSuccess)
    DLOG(ERROR) << "Failed to send connection_accepted to " << peer_.Endpoint();
}

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe
