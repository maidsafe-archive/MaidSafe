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

#include "maidsafe/rudp/core/socket.h"

#include <algorithm>
#include <utility>
#include <limits>

#include "boost/assert.hpp"

#include "maidsafe/common/utils.h"

#include "maidsafe/rudp/log.h"
#include "maidsafe/rudp/core/multiplexer.h"

namespace asio = boost::asio;
namespace ip = asio::ip;
namespace bs = boost::system;
namespace bptime = boost::posix_time;
namespace args = std::placeholders;

namespace maidsafe {

namespace rudp {

namespace detail {

Socket::Socket(Multiplexer &multiplexer)  // NOLINT (Fraser)
  : dispatcher_(multiplexer.dispatcher_),
    peer_(multiplexer),
    tick_timer_(multiplexer.socket_.get_io_service()),
    session_(peer_, tick_timer_, multiplexer.external_endpoint_),
    congestion_control_(),
    sender_(peer_, tick_timer_, congestion_control_),
    receiver_(peer_, tick_timer_, congestion_control_),
    waiting_connect_(multiplexer.socket_.get_io_service()),
    waiting_connect_ec_(),
    waiting_write_(multiplexer.socket_.get_io_service()),
    waiting_write_buffer_(),
    waiting_write_ec_(),
    waiting_write_bytes_transferred_(0),
    waiting_read_(multiplexer.socket_.get_io_service()),
    waiting_read_buffer_(),
    waiting_read_transfer_at_least_(0),
    waiting_read_ec_(),
    waiting_read_bytes_transferred_(0),
    waiting_keepalive_sequence_number_(0),
    waiting_probe_(multiplexer.socket_.get_io_service()),
    waiting_probe_ec_(),
    waiting_flush_(multiplexer.socket_.get_io_service()),
    waiting_flush_ec_(),
    sent_length_(0) {
  waiting_connect_.expires_at(bptime::pos_infin);
  waiting_write_.expires_at(bptime::pos_infin);
  waiting_read_.expires_at(bptime::pos_infin);
  waiting_flush_.expires_at(bptime::pos_infin);
}

Socket::~Socket() {
  if (IsOpen())
    dispatcher_.RemoveSocket(session_.Id());
}

uint32_t Socket::Id() const {
  return session_.Id();
}

uint32_t Socket::SentLength() {
  BOOST_ASSERT(waiting_write_bytes_transferred_ - sent_length_ <=
               std::numeric_limits<uint32_t>::max());
  uint32_t sent_length = static_cast<uint32_t>(
      waiting_write_bytes_transferred_ - sent_length_);
  sent_length_ = waiting_write_bytes_transferred_;
  return sent_length;
}

uint32_t Socket::BestReadBufferSize() {
  return congestion_control_.BestReadBufferSize();
}

boost::asio::ip::udp::endpoint Socket::RemoteEndpoint() const {
  return peer_.Endpoint();
}

uint32_t Socket::RemoteId() const {
  return peer_.Id();
}

bool Socket::IsOpen() const {
  return session_.IsOpen();
}

void Socket::Close() {
  if (session_.IsOpen()) {
    sender_.NotifyClose();
    congestion_control_.OnClose();
    dispatcher_.RemoveSocket(session_.Id());
  }
  session_.Close();
//  peer_.SetEndpoint(ip::udp::endpoint());
  peer_.SetId(0);
  tick_timer_.Cancel();
  waiting_connect_ec_ = asio::error::operation_aborted;
  waiting_connect_.cancel();
  waiting_write_ec_ = asio::error::operation_aborted;
  waiting_write_bytes_transferred_ = 0;
  waiting_write_.cancel();
  waiting_read_ec_ = asio::error::operation_aborted;
  waiting_read_bytes_transferred_ = 0;
  waiting_read_.cancel();
  waiting_flush_ec_ = asio::error::operation_aborted;
  waiting_flush_.cancel();
  waiting_probe_ec_ = asio::error::shut_down;
  waiting_probe_.cancel();
}

void Socket::StartConnect(const ip::udp::endpoint &remote) {
  peer_.SetEndpoint(remote);
  peer_.SetId(0);  // Assigned when handshake response is received.
  session_.Open(dispatcher_.AddSocket(this),
                sender_.GetNextPacketSequenceNumber());
}

void Socket::StartProbe() {
  waiting_probe_ec_ = asio::error::operation_aborted;
  if (!session_.IsConnected()) {
    waiting_probe_ec_ = boost::asio::error::not_connected;
    waiting_probe_.cancel();
    waiting_keepalive_sequence_number_ = 0;
    return;
  }
  // Request packet sequence numbers must be odd
  waiting_keepalive_sequence_number_ = (RandomUint32() | 0x00000001);
  KeepalivePacket keepalive_packet;
  keepalive_packet.SetDestinationSocketId(peer_.Id());
  keepalive_packet.SetSequenceNumber(waiting_keepalive_sequence_number_);
  if (kSuccess != sender_.SendKeepalive(keepalive_packet)) {
    waiting_probe_ec_ = boost::asio::error::try_again;
    waiting_probe_.cancel();
    waiting_keepalive_sequence_number_ = 0;
  }
}

void Socket::StartWrite(const asio::const_buffer &data) {
  // Check for a no-op write.
  if (asio::buffer_size(data) == 0) {
    waiting_write_ec_.clear();
    waiting_write_.cancel();
    return;
  }

  // Try processing the write immediately. If there's space in the write buffer
  // then the operation will complete immediately. Otherwise, it will wait until
  // some other event frees up space in the buffer.
  waiting_write_buffer_ = data;
  waiting_write_bytes_transferred_ = 0;
  ProcessWrite();
}

void Socket::ProcessWrite() {
  // There's only a waiting write if the write buffer is non-empty.
  if (asio::buffer_size(waiting_write_buffer_) == 0)
    return;

  // Copy whatever data we can into the write buffer.
  size_t length = sender_.AddData(waiting_write_buffer_);
  waiting_write_buffer_ = waiting_write_buffer_ + length;
  waiting_write_bytes_transferred_ += length;
  // If we have finished writing all of the data then it's time to trigger the
  // write's completion handler.
  if (asio::buffer_size(waiting_write_buffer_) == 0) {
    sent_length_ = 0;
    // The write is done. Trigger the write's completion handler.
    waiting_write_ec_.clear();
    waiting_write_.cancel();
  }
}

void Socket::StartRead(const asio::mutable_buffer &data,
                       size_t transfer_at_least) {
  // Check for a no-read write.
  if (asio::buffer_size(data) == 0) {
    waiting_read_ec_.clear();
    waiting_read_.cancel();
    return;
  }

  // Try processing the read immediately. If there's available data then the
  // operation will complete immediately. Otherwise it will wait until the next
  // data packet arrives.
  waiting_read_buffer_ = data;
  waiting_read_transfer_at_least_ = transfer_at_least;
  waiting_read_bytes_transferred_ = 0;
  ProcessRead();
}

void Socket::ProcessRead() {
  // There's only a waiting read if the read buffer is non-empty.
  if (asio::buffer_size(waiting_read_buffer_) == 0)
    return;

  // Copy whatever data we can into the read buffer.
  size_t length = receiver_.ReadData(waiting_read_buffer_);
  waiting_read_buffer_ = waiting_read_buffer_ + length;
  waiting_read_bytes_transferred_ += length;

  // If we have filled the buffer, or read more than the minimum number of
  // bytes required, then it's time to trigger the read's completion handler.
  if (asio::buffer_size(waiting_read_buffer_) == 0 ||
      waiting_read_bytes_transferred_ >= waiting_read_transfer_at_least_) {
    // the read is done. Trigger the read's completion handler.
    waiting_read_ec_.clear();
    waiting_read_.cancel();
  }
}

void Socket::StartFlush() {
  ProcessFlush();
}

void Socket::ProcessFlush() {
  if (sender_.Flushed() && receiver_.Flushed()) {
    waiting_flush_ec_.clear();
    waiting_flush_.cancel();
  }
}

void Socket::HandleReceiveFrom(const asio::const_buffer &data,
                               const ip::udp::endpoint &endpoint) {
  if (endpoint == peer_.Endpoint()) {
    DataPacket data_packet;
    AckPacket ack_packet;
    AckOfAckPacket ack_of_ack_packet;
    NegativeAckPacket negative_ack_packet;
    HandshakePacket handshake_packet;
    ShutdownPacket shutdown_packet;
    KeepalivePacket keepalive_packet;
    if (keepalive_packet.Decode(data)) {
      HandleKeepalive(keepalive_packet);
    } else if (data_packet.Decode(data)) {
      HandleData(data_packet);
    } else if (ack_packet.Decode(data)) {
      HandleAck(ack_packet);
    } else if (ack_of_ack_packet.Decode(data)) {
      HandleAckOfAck(ack_of_ack_packet);
    } else if (negative_ack_packet.Decode(data)) {
      HandleNegativeAck(negative_ack_packet);
    } else if (handshake_packet.Decode(data)) {
      HandleHandshake(handshake_packet);
    } else if (shutdown_packet.Decode(data)) {
      Close();
    } else {
      DLOG(ERROR) << "Socket " << session_.Id()
                  << " ignoring invalid packet from " << endpoint;
    }
  } else {
    DLOG(ERROR) << "Socket " << session_.Id()
                << " ignoring spurious packet from " << endpoint;
  }
}

void Socket::HandleHandshake(const HandshakePacket &packet) {
  bool was_connected = session_.IsConnected();
  session_.HandleHandshake(packet);
  if (!was_connected && session_.IsConnected()) {
    congestion_control_.OnOpen(sender_.GetNextPacketSequenceNumber(),
                               session_.ReceivingSequenceNumber());
    congestion_control_.SetPeerConnectionType(session_.PeerConnectionType());
    receiver_.Reset(session_.ReceivingSequenceNumber());
    waiting_connect_ec_.clear();
    waiting_connect_.cancel();
  }
}

void Socket::HandleKeepalive(const KeepalivePacket &packet) {
  if (session_.IsConnected()) {
    if (packet.IsResponse()) {
      if (waiting_keepalive_sequence_number_ &&
            packet.IsResponseOf(waiting_keepalive_sequence_number_)) {
        waiting_keepalive_sequence_number_ = 0;
        waiting_probe_ec_.clear();
        waiting_probe_.cancel();
        return;
      } else {
        DLOG(INFO) << "Socket " << session_.Id()
                   << " ignoring unexpected keepalive response packet from "
                   << peer_.Endpoint();
      }
    } else {
      sender_.HandleKeepalive(packet);
    }
  }
}

void Socket::HandleData(const DataPacket &packet) {
  if (session_.IsConnected()) {
    receiver_.HandleData(packet);
    ProcessRead();
    ProcessWrite();
  }
}

void Socket::HandleAck(const AckPacket &packet) {
  if (session_.IsConnected()) {
    sender_.HandleAck(packet);
    ProcessRead();
    ProcessWrite();
    ProcessFlush();
  }
}

void Socket::HandleAckOfAck(const AckOfAckPacket &packet) {
  if (session_.IsConnected()) {
    receiver_.HandleAckOfAck(packet);
    ProcessRead();
    ProcessWrite();
    ProcessFlush();
  }
}

void Socket::HandleNegativeAck(const NegativeAckPacket &packet) {
  if (session_.IsConnected()) {
    sender_.HandleNegativeAck(packet);
  }
}

void Socket::HandleTick() {
  if (session_.IsConnected()) {
    sender_.HandleTick();
    receiver_.HandleTick();
    ProcessRead();
    ProcessWrite();
    ProcessFlush();
  } else {
    session_.HandleTick();
  }
}

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe
