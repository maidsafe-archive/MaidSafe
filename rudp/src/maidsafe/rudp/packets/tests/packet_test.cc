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

#include "maidsafe/common/test.h"

#include "maidsafe/rudp/log.h"
#include "maidsafe/rudp/packets/packet.h"
#include "maidsafe/rudp/packets/data_packet.h"
#include "maidsafe/rudp/packets/control_packet.h"
#include "maidsafe/rudp/packets/ack_packet.h"
#include "maidsafe/rudp/packets/handshake_packet.h"
#include "maidsafe/rudp/packets/keepalive_packet.h"
#include "maidsafe/rudp/packets/shutdown_packet.h"
#include "maidsafe/rudp/packets/ack_of_ack_packet.h"
#include "maidsafe/rudp/packets/negative_ack_packet.h"
#include "maidsafe/rudp/parameters.h"

namespace maidsafe {

namespace rudp {

namespace detail {

namespace test {

TEST(PacketTest, FUNC_DecodeDestinationSocketId) {
  {
    // Try to decode with an invalid buffer
    uint32_t id;
    char d[15];
    EXPECT_FALSE(Packet::DecodeDestinationSocketId(&id,
                                                   boost::asio::buffer(d)));
  }
  {
    // Decode with a valid buffer
    char d[16];
    d[12] = 0x44;
    d[13] = 0x22;
    d[14] = 0x11;
    d[15] = 0x00;
    uint32_t id;
    EXPECT_TRUE(Packet::DecodeDestinationSocketId(&id,
                                                  boost::asio::buffer(d)));
    EXPECT_EQ(0x44221100, id);
  }
}

class DataPacketTest : public testing::Test {
 public:
  DataPacketTest() : data_packet_() {}

  void RestoreDefault() {
    data_packet_.SetFirstPacketInMessage(false);
    data_packet_.SetLastPacketInMessage(false);
    data_packet_.SetInOrder(false);
    data_packet_.SetPacketSequenceNumber(0);
    data_packet_.SetMessageNumber(0);
    data_packet_.SetTimeStamp(0);
    data_packet_.SetDestinationSocketId(0);
    data_packet_.SetData("");
  }

  void TestEncodeDecode() {
    std::string data;
    for (uint32_t i = 0; i < Parameters::max_size; ++i)
      data += "a";
    uint32_t packet_sequence_number = 0x7fffffff;
    uint32_t message_number = 0x1fffffff;
    uint32_t time_stamp = 0xffffffff;
    uint32_t destination_socket_id = 0xffffffff;

    data_packet_.SetData(data);
    data_packet_.SetPacketSequenceNumber(packet_sequence_number);
    data_packet_.SetMessageNumber(message_number);
    data_packet_.SetTimeStamp(time_stamp);
    data_packet_.SetDestinationSocketId(destination_socket_id);

    char char_array[Parameters::kUDPPayload];
    boost::asio::mutable_buffer dbuffer(boost::asio::buffer(&char_array[0],
        DataPacket::kHeaderSize + Parameters::max_size));
    EXPECT_EQ(DataPacket::kHeaderSize + data.size(),
              data_packet_.Encode(dbuffer));
    RestoreDefault();
    EXPECT_TRUE(data_packet_.Decode(dbuffer));

    std::string full_data = data_packet_.Data();
    std::string trimmed_data;
    trimmed_data.assign(full_data, 0, data.size());
    EXPECT_EQ(data, trimmed_data);
    EXPECT_EQ(packet_sequence_number, data_packet_.PacketSequenceNumber());
    EXPECT_EQ(message_number, data_packet_.MessageNumber());
    EXPECT_EQ(time_stamp, data_packet_.TimeStamp());
    EXPECT_EQ(destination_socket_id, data_packet_.DestinationSocketId());
  }

 protected:
  DataPacket data_packet_;
};

TEST_F(DataPacketTest, FUNC_SequenceNumber) {
  EXPECT_EQ(0U, data_packet_.PacketSequenceNumber());
//   data_packet_.SetPacketSequenceNumber(0x80000000);
//   EXPECT_EQ(0U, data_packet_.PacketSequenceNumber());
  data_packet_.SetPacketSequenceNumber(0x7fffffff);
  EXPECT_EQ(0x7fffffff, data_packet_.PacketSequenceNumber());
}

TEST_F(DataPacketTest, FUNC_FirstPacketInMessage) {
  EXPECT_FALSE(data_packet_.FirstPacketInMessage());
  data_packet_.SetFirstPacketInMessage(true);
  EXPECT_TRUE(data_packet_.FirstPacketInMessage());
}

TEST_F(DataPacketTest, FUNC_LastPacketInMessage) {
  EXPECT_FALSE(data_packet_.LastPacketInMessage());
  data_packet_.SetLastPacketInMessage(true);
  EXPECT_TRUE(data_packet_.LastPacketInMessage());
}

TEST_F(DataPacketTest, FUNC_InOrder) {
  EXPECT_FALSE(data_packet_.InOrder());
  data_packet_.SetInOrder(true);
  EXPECT_TRUE(data_packet_.InOrder());
}

TEST_F(DataPacketTest, FUNC_MessageNumber) {
  EXPECT_EQ(0U, data_packet_.MessageNumber());
//   data_packet_.SetPacketMessageNumber(0x20000000);
//   EXPECT_EQ(0U, data_packet_.MessageNumber());
  data_packet_.SetMessageNumber(0x1fffffff);
  EXPECT_EQ(0x1fffffff, data_packet_.MessageNumber());
}

TEST_F(DataPacketTest, FUNC_TimeStamp) {
  EXPECT_EQ(0U, data_packet_.TimeStamp());
  data_packet_.SetTimeStamp(0xffffffff);
  EXPECT_EQ(0xffffffff, data_packet_.TimeStamp());
}

TEST_F(DataPacketTest, FUNC_DestinationSocketId) {
  EXPECT_EQ(0U, data_packet_.DestinationSocketId());
  data_packet_.SetDestinationSocketId(0xffffffff);
  EXPECT_EQ(0xffffffff, data_packet_.DestinationSocketId());
}

TEST_F(DataPacketTest, FUNC_Data) {
  EXPECT_EQ("", data_packet_.Data());
  data_packet_.SetData("Data Test");
  EXPECT_EQ("Data Test", data_packet_.Data());
}

TEST_F(DataPacketTest, FUNC_IsValid) {
  char d1[15];
  EXPECT_FALSE(data_packet_.IsValid(boost::asio::buffer(d1)));
  char d2[16];
  d2[0] = 0x00;
  EXPECT_TRUE(data_packet_.IsValid(boost::asio::buffer(d2)));
  d2[0] = static_cast<unsigned char>(0x80);
  EXPECT_FALSE(data_packet_.IsValid(boost::asio::buffer(d2)));
}

TEST_F(DataPacketTest, BEH_EncodeDecode) {
  {
    // Pass in a buffer having the length less than required
    std::string data("Encode Decode Test");
    data_packet_.SetData(data);
    char dbuffer[32];
    EXPECT_EQ(0U, data_packet_.Encode(boost::asio::buffer(dbuffer)));
  }
  RestoreDefault();
  {
    // Send a packet as the First packet in message
    data_packet_.SetFirstPacketInMessage(true);
    TestEncodeDecode();
    EXPECT_TRUE(data_packet_.FirstPacketInMessage());
    EXPECT_FALSE(data_packet_.InOrder());
    EXPECT_FALSE(data_packet_.LastPacketInMessage());
  }
  RestoreDefault();
  {
    // Send a packet as the InOrder packet in message
    data_packet_.SetInOrder(true);
    TestEncodeDecode();
    EXPECT_TRUE(data_packet_.InOrder());
    EXPECT_FALSE(data_packet_.FirstPacketInMessage());
    EXPECT_FALSE(data_packet_.LastPacketInMessage());
  }
  RestoreDefault();
  {
    // Send a packet as the Last packet in message
    data_packet_.SetLastPacketInMessage(true);
    TestEncodeDecode();
    EXPECT_TRUE(data_packet_.LastPacketInMessage());
    EXPECT_FALSE(data_packet_.FirstPacketInMessage());
    EXPECT_FALSE(data_packet_.InOrder());
  }
}

class ControlPacketTest : public testing::Test {
 public:
  ControlPacketTest() : control_packet_() {}

 protected:
  void TestAdditionalInfo() {
    EXPECT_EQ(0U, control_packet_.AdditionalInfo());
    control_packet_.SetAdditionalInfo(0xffffffff);
    EXPECT_EQ(0xffffffff, control_packet_.AdditionalInfo());
  }

  void SetType(uint16_t n) {
    control_packet_.SetType(n);
  }

  bool IsValidBase(const boost::asio::const_buffer &buffer,
                   uint16_t expected_packet_type) {
    return control_packet_.IsValidBase(buffer, expected_packet_type);
  }

  void TestEncodeDecode() {
    {
      // Pass in a buffer having the length less than required
      char d[15];
      EXPECT_EQ(0U, control_packet_.EncodeBase(boost::asio::buffer(d)));
    }
    {
      control_packet_.SetType(0x7fff);
      control_packet_.SetAdditionalInfo(0xffffffff);
      control_packet_.SetTimeStamp(0xffffffff);
      control_packet_.SetDestinationSocketId(0xffffffff);

      char char_array[ControlPacket::kHeaderSize];
      boost::asio::mutable_buffer dbuffer(boost::asio::buffer(char_array));
      EXPECT_EQ(ControlPacket::kHeaderSize,
                control_packet_.EncodeBase(dbuffer));

      control_packet_.SetType(0);
      control_packet_.SetAdditionalInfo(0);
      control_packet_.SetTimeStamp(0);
      control_packet_.SetDestinationSocketId(0);
      EXPECT_TRUE(control_packet_.DecodeBase(dbuffer, 0x7fff));

      EXPECT_EQ(0x7fff, control_packet_.Type());
      EXPECT_EQ(0xffffffff, control_packet_.AdditionalInfo());
      EXPECT_EQ(0xffffffff, control_packet_.TimeStamp());
      EXPECT_EQ(0xffffffff, control_packet_.DestinationSocketId());
    }
  }

  ControlPacket control_packet_;
};

TEST_F(ControlPacketTest, FUNC_Type) {
  EXPECT_EQ(0U, control_packet_.Type());
//   control_packet_.SetType(0x8000);
//   EXPECT_EQ(0U, control_packet_.Type());
  SetType(0x7fff);
  EXPECT_EQ(0x7fff, control_packet_.Type());
}

TEST_F(ControlPacketTest, FUNC_AdditionalInfo) {
  TestAdditionalInfo();
}

TEST_F(ControlPacketTest, FUNC_TimeStamp) {
  EXPECT_EQ(0U, control_packet_.TimeStamp());
  control_packet_.SetTimeStamp(0xffffffff);
  EXPECT_EQ(0xffffffff, control_packet_.TimeStamp());
}

TEST_F(ControlPacketTest, FUNC_DestinationSocketId) {
  EXPECT_EQ(0U, control_packet_.DestinationSocketId());
  control_packet_.SetDestinationSocketId(0xffffffff);
  EXPECT_EQ(0xffffffff, control_packet_.DestinationSocketId());
}

TEST_F(ControlPacketTest, FUNC_IsValidBase) {
  {
    // Buffer length too short
    char d[15];
    EXPECT_FALSE(IsValidBase(boost::asio::buffer(d), 0x7444));
  }
  char d[16];
  {
    // Packet type is not a control_packet
    d[0] = 0x00;
    EXPECT_FALSE(IsValidBase(boost::asio::buffer(d), 0x7444));
  }
  {
    // Input control packet is not in an expected packet type
    d[0] = static_cast<unsigned char>(0x80);
    EXPECT_FALSE(IsValidBase(boost::asio::buffer(d), 0x7444));
  }
  {
    // Everything is fine
    d[0] = static_cast<unsigned char>(0xf4);
    d[1] = 0x44;
    EXPECT_TRUE(IsValidBase(boost::asio::buffer(d), 0x7444));
  }
}

TEST_F(ControlPacketTest, BEH_EncodeDecode) {
  TestEncodeDecode();
}

class AckPacketTest : public testing::Test {
 public:
  AckPacketTest() : ack_packet_() {}

 protected:
  void RestoreDefault() {
    ack_packet_.SetAckSequenceNumber(0);
    ack_packet_.SetPacketSequenceNumber(0);
    ack_packet_.SetRoundTripTime(0);
    ack_packet_.SetRoundTripTimeVariance(0);
    ack_packet_.SetAvailableBufferSize(0);
    ack_packet_.SetPacketsReceivingRate(0);
    ack_packet_.SetEstimatedLinkCapacity(0);
  }

  void TestEncodeDecode() {
    ack_packet_.SetAckSequenceNumber(0xffffffff);
    ack_packet_.SetPacketSequenceNumber(0xffffffff);

    char char_array_optional[AckPacket::kOptionalPacketSize];
    char char_array[AckPacket::kPacketSize];
    boost::asio::mutable_buffer dbuffer;
    if (ack_packet_.HasOptionalFields()) {
      dbuffer = boost::asio::buffer(char_array_optional);
      EXPECT_EQ(AckPacket::kOptionalPacketSize,
                ack_packet_.Encode(dbuffer));
    } else {
      dbuffer = boost::asio::buffer(char_array);
      EXPECT_EQ(AckPacket::kPacketSize, ack_packet_.Encode(dbuffer));
    }
    RestoreDefault();
    EXPECT_TRUE(ack_packet_.Decode(dbuffer));

    EXPECT_EQ(0xffffffff, ack_packet_.AckSequenceNumber());
    EXPECT_EQ(0xffffffff, ack_packet_.PacketSequenceNumber());
  }

  AckPacket ack_packet_;
};

TEST_F(AckPacketTest, FUNC_IsValid) {
  {
    // Buffer length wrong
    char d[ControlPacket::kHeaderSize + 10];
    EXPECT_FALSE(ack_packet_.IsValid(boost::asio::buffer(d)));
  }
  char d[ControlPacket::kHeaderSize + 4];
  {
    // Packet type wrong
    d[0] = static_cast<unsigned char>(0x80);
    EXPECT_FALSE(ack_packet_.IsValid(boost::asio::buffer(d)));
  }
  {
    // Everything is fine
    d[0] = static_cast<unsigned char>(0x80);
    d[1] = AckPacket::kPacketType;
    EXPECT_TRUE(ack_packet_.IsValid(boost::asio::buffer(d)));
  }
}

TEST_F(AckPacketTest, BEH_EncodeDecode) {
  {
    // Pass in a buffer having the length less than required
    char dbuffer[AckPacket::kPacketSize - 1];
    EXPECT_EQ(0U, ack_packet_.Encode(boost::asio::buffer(dbuffer)));
  }
  {
    // TODO(Team) There will be an error if passed in buffer has a size less
    //            than kOptionalPacketSize, but the has_optional_fields_ has
    //            been set
  }
  RestoreDefault();
  {
    // Send an ack_packet without optional fields
    ack_packet_.SetHasOptionalFields(false);
    TestEncodeDecode();
    EXPECT_EQ(0U, ack_packet_.RoundTripTime());
    EXPECT_EQ(0U, ack_packet_.RoundTripTimeVariance());
    EXPECT_EQ(0U, ack_packet_.AvailableBufferSize());
    EXPECT_EQ(0U, ack_packet_.PacketsReceivingRate());
    EXPECT_EQ(0U, ack_packet_.EstimatedLinkCapacity());
  }
  RestoreDefault();
  {
    // Send an ack_packet without optional fields
    ack_packet_.SetHasOptionalFields(true);
    ack_packet_.SetRoundTripTime(0x11111111);
    ack_packet_.SetRoundTripTimeVariance(0x22222222);
    ack_packet_.SetAvailableBufferSize(0x44444444);
    ack_packet_.SetPacketsReceivingRate(0x88888888);
    ack_packet_.SetEstimatedLinkCapacity(0xffffffff);
    TestEncodeDecode();
    EXPECT_EQ(0x11111111, ack_packet_.RoundTripTime());
    EXPECT_EQ(0x22222222, ack_packet_.RoundTripTimeVariance());
    EXPECT_EQ(0x44444444, ack_packet_.AvailableBufferSize());
    EXPECT_EQ(0x88888888, ack_packet_.PacketsReceivingRate());
    EXPECT_EQ(0xffffffff, ack_packet_.EstimatedLinkCapacity());
  }
}

class HandshakePacketTest : public testing::Test {
 public:
  HandshakePacketTest() : handshake_packet_() {}

 protected:
  HandshakePacket handshake_packet_;
};

TEST_F(HandshakePacketTest, FUNC_IsValid) {
  {
    // Buffer length wrong
    char d[HandshakePacket::kPacketSize + 10];
    EXPECT_FALSE(handshake_packet_.IsValid(boost::asio::buffer(d)));
  }
  char d[HandshakePacket::kPacketSize];
  {
    // Packet type wrong
    d[0] = static_cast<unsigned char>(0x80);
    EXPECT_FALSE(handshake_packet_.IsValid(boost::asio::buffer(d)));
  }
  {
    // Everything is fine
    d[0] = static_cast<unsigned char>(0x80);
    d[1] = HandshakePacket::kPacketType;
    EXPECT_TRUE(handshake_packet_.IsValid(boost::asio::buffer(d)));
  }
}

TEST_F(HandshakePacketTest, BEH_EncodeDecode) {
  {
    // Pass in a buffer having the length less than required
    char dbuffer[HandshakePacket::kPacketSize - 1];
    EXPECT_EQ(0U, handshake_packet_.Encode(boost::asio::buffer(dbuffer)));
  }
  {
    // Encode and Decode a Handshake Packet
    handshake_packet_.SetRudpVersion(0x11111111);
    handshake_packet_.SetSocketType(0x22222222);
    handshake_packet_.SetInitialPacketSequenceNumber(0x44444444);
    handshake_packet_.SetMaximumPacketSize(0x88888888);
    handshake_packet_.SetMaximumFlowWindowSize(0xffffffff);
    handshake_packet_.SetConnectionType(0xdddddddd);
    handshake_packet_.SetSocketId(0xbbbbbbbb);
    handshake_packet_.SetSynCookie(0xaaaaaaaa);
    boost::asio::ip::udp::endpoint endpoint(
        boost::asio::ip::address::from_string(
                           "2001:db8:85a3:8d3:1319:8a2e:370:7348"),
        12345);
    handshake_packet_.SetEndpoint(endpoint);

    char char_array[HandshakePacket::kPacketSize];
    boost::asio::mutable_buffer dbuffer(boost::asio::buffer(char_array));
    handshake_packet_.Encode(boost::asio::buffer(dbuffer));

    handshake_packet_.SetRudpVersion(0);
    handshake_packet_.SetSocketType(0);
    handshake_packet_.SetInitialPacketSequenceNumber(0);
    handshake_packet_.SetMaximumPacketSize(0);
    handshake_packet_.SetMaximumFlowWindowSize(0);
    handshake_packet_.SetConnectionType(0);
    handshake_packet_.SetSocketId(0);
    handshake_packet_.SetSynCookie(0);
    handshake_packet_.SetEndpoint(boost::asio::ip::udp::endpoint());

    handshake_packet_.Decode(dbuffer);

    EXPECT_EQ(0x11111111, handshake_packet_.RudpVersion());
    EXPECT_EQ(0x22222222, handshake_packet_.SocketType());
    EXPECT_EQ(0x44444444, handshake_packet_.InitialPacketSequenceNumber());
    EXPECT_EQ(0x88888888, handshake_packet_.MaximumPacketSize());
    EXPECT_EQ(0xffffffff, handshake_packet_.MaximumFlowWindowSize());
    EXPECT_EQ(0xdddddddd, handshake_packet_.ConnectionType());
    EXPECT_EQ(0xbbbbbbbb, handshake_packet_.SocketId());
    EXPECT_EQ(0xaaaaaaaa, handshake_packet_.SynCookie());
    EXPECT_EQ(endpoint, handshake_packet_.Endpoint());
  }
}

TEST(KeepalivePacketTest, FUNC_ALL) {
  // Generally, KeepalivePacket use Base(ControlPacket)'s IsValid and
  // Encode/Decode directly. So here we only test those error condition branch
  KeepalivePacket keepalive_packet;
  {
    // Decode with a wrong length Buffer
    char d[KeepalivePacket::kPacketSize + 10];
    EXPECT_FALSE(keepalive_packet.Decode(boost::asio::buffer(d)));
  }
  {
    // Decode with a type wrong Packet
    char d[KeepalivePacket::kPacketSize];
    d[0] = static_cast<unsigned char>(0x80);
    EXPECT_FALSE(keepalive_packet.Decode(boost::asio::buffer(d)));
  }
  {
    // Encode then Decode
    char char_array[KeepalivePacket::kPacketSize];
    boost::asio::mutable_buffer dbuffer(boost::asio::buffer(char_array));
    EXPECT_EQ(KeepalivePacket::kPacketSize,
              keepalive_packet.Encode(dbuffer));
    EXPECT_TRUE(keepalive_packet.Decode(dbuffer));
  }
}

TEST(ShutdownPacketTest, FUNC_ALL) {
  // Generally, ShutdownPacket use Base(ControlPacket)'s IsValid and
  // Encode/Decode directly. So here we only test those error condition branch
  ShutdownPacket shutdown_packet;
  {
    // Decode with a wrong length Buffer
    char d[ShutdownPacket::kPacketSize + 10];
    EXPECT_FALSE(shutdown_packet.Decode(boost::asio::buffer(d)));
  }
  {
    // Decode with a type wrong Packet
    char d[ShutdownPacket::kPacketSize];
    d[0] = static_cast<unsigned char>(0x80);
    EXPECT_FALSE(shutdown_packet.Decode(boost::asio::buffer(d)));
  }
  {
    // Encode then Decode
    char char_array[ShutdownPacket::kPacketSize];
    boost::asio::mutable_buffer dbuffer(boost::asio::buffer(char_array));
    EXPECT_EQ(ShutdownPacket::kPacketSize,
              shutdown_packet.Encode(dbuffer));
    EXPECT_TRUE(shutdown_packet.Decode(dbuffer));
  }
}

TEST(AckOfAckPacketTest, FUNC_ALL) {
  // Generally, AckOfAckPacket use Base(ControlPacket)'s IsValid and
  // Encode/Decode directly. So here we only test those error condition branch
  AckOfAckPacket ackofack_packet;
  {
    // Decode with a wrong length Buffer
    char d[AckOfAckPacket::kPacketSize + 10];
    EXPECT_FALSE(ackofack_packet.Decode(boost::asio::buffer(d)));
  }
  {
    // Decode with a type wrong Packet
    char d[AckOfAckPacket::kPacketSize];
    d[0] = static_cast<unsigned char>(0x80);
    EXPECT_FALSE(ackofack_packet.Decode(boost::asio::buffer(d)));
  }
  {
    // Encode then Decode
    ackofack_packet.SetAckSequenceNumber(0xffffffff);
    char char_array[AckOfAckPacket::kPacketSize];
    boost::asio::mutable_buffer dbuffer(boost::asio::buffer(char_array));
    EXPECT_EQ(AckOfAckPacket::kPacketSize,
              ackofack_packet.Encode(dbuffer));
    ackofack_packet.SetAckSequenceNumber(0);
    EXPECT_TRUE(ackofack_packet.Decode(dbuffer));
    EXPECT_EQ(0xffffffff, ackofack_packet.AckSequenceNumber());
  }
}

class NegativeAckPacketTest : public testing::Test {
 public:
  NegativeAckPacketTest() : negative_ack_packet_() {}

 protected:
  NegativeAckPacket negative_ack_packet_;
};

TEST_F(NegativeAckPacketTest, FUNC_IsValid) {
  {
    // Buffer length less
    char d[ControlPacket::kHeaderSize];
    EXPECT_FALSE(negative_ack_packet_.IsValid(boost::asio::buffer(d)));
  }
  {
    // Buffer length wrong
    char d[ControlPacket::kHeaderSize + 13];
    EXPECT_FALSE(negative_ack_packet_.IsValid(boost::asio::buffer(d)));
  }
  char d[ControlPacket::kHeaderSize + 12];
  {
    // Packet type wrong
    d[0] = static_cast<unsigned char>(0x80);
    EXPECT_FALSE(negative_ack_packet_.IsValid(boost::asio::buffer(d)));
  }
  {
    // Everything is fine
    d[0] = static_cast<unsigned char>(0x80);
    d[1] = NegativeAckPacket::kPacketType;
    EXPECT_TRUE(negative_ack_packet_.IsValid(boost::asio::buffer(d)));
  }
}

TEST_F(NegativeAckPacketTest, FUNC_ContainsSequenceNumber) {
  EXPECT_FALSE(negative_ack_packet_.HasSequenceNumbers());
  {
    // Search in Empty
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0));
  }
  {
    // Search a single
    negative_ack_packet_.AddSequenceNumber(0x8);
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0));
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0x7fffffff));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x8));
  }
  {
    // Search in an one-value-range
    negative_ack_packet_.AddSequenceNumbers(0x9, 0x9);
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0));
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0x7fffffff));
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0x7));
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0x10));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x9));
  }
  {
    // Search in a range
    negative_ack_packet_.AddSequenceNumbers(0x11, 0x17);
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0));
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0x7fffffff));
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0x7));
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0x10));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x11));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x17));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x16));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x12));
  }
  {
    // Search in a wrapped around range
    negative_ack_packet_.AddSequenceNumbers(0x7fffffff, 0x0);
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x7fffffff));
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0x7));
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0x10));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x11));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x17));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x16));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x12));
  }
  {
    // Search in an overlapped range
    negative_ack_packet_.AddSequenceNumbers(0x7ffffff0, 0xf);
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x7fffffff));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x7));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x8));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x9));
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0x10));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x11));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x17));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x16));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x12));
    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0x7fffffef));
  }
  EXPECT_TRUE(negative_ack_packet_.HasSequenceNumbers());
}

TEST_F(NegativeAckPacketTest, BEH_EncodeDecode) {
  negative_ack_packet_.AddSequenceNumber(0x8);
  {
    // Pass in a buffer having less space to encode
    char d[ControlPacket::kHeaderSize + 1 * 4 - 1];
    EXPECT_FALSE(negative_ack_packet_.IsValid(boost::asio::buffer(d)));
  }
  {
    // Encode and Decode a NegativeAck Packet
    negative_ack_packet_.AddSequenceNumbers(0x7fffffff, 0x5);

    char char_array[ControlPacket::kHeaderSize + 3 * 4];
    boost::asio::mutable_buffer dbuffer(boost::asio::buffer(char_array));
    negative_ack_packet_.Encode(boost::asio::buffer(dbuffer));

    negative_ack_packet_.AddSequenceNumber(0x7);

    negative_ack_packet_.Decode(dbuffer);

    EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0x7));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x8));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x7fffffff));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x0));
    EXPECT_TRUE(negative_ack_packet_.ContainsSequenceNumber(0x5));
//     EXPECT_FALSE(negative_ack_packet_.ContainsSequenceNumber(0x80000000));
  }
}

}  // namespace test

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe
