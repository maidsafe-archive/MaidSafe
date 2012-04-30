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

#include "boost/lexical_cast.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"

#include "maidsafe/common/test.h"
#include "maidsafe/common/crypto.h"
#include "maidsafe/common/utils.h"
#include "maidsafe/rudp/core/message_handler.h"
#include "maidsafe/rudp/transport_pb.h"

namespace maidsafe {

namespace rudp {

namespace test {

// class TransportMessageHandlerTest : public testing::Test {
// public:
//  TransportMessageHandlerTest() : private_key_(),
//                                  msg_hndlr_(),
//                                  asym_null_private_key_(),
//                                  msg_hndlr_no_securifier_(
//                                      asym_null_private_key_),
//                                  invoked_slots_(),
//                                  slots_mutex_(),
//                                  error_count_(0) {}
//  static void SetUpTestCase() {
//    asymm::GenerateKeyPair(&crypto_key_pair_);
//  }
//
//  virtual void SetUp() {
//    private_key_.reset(new asymm::PrivateKey(crypto_key_pair_.private_key));
//    msg_hndlr_.reset(new MessageHandler(private_key_));
//  }
//  virtual void TearDown() {}
//
//  template<class T>
//  std::string EncryptMessage(T request, MessageType request_type) {
//    protobuf::WrapperMessage message;
//    message.set_msg_type(request_type);
//    message.set_payload(request.SerializeAsString());
//    std::string result(1, kNone);
//    result += message.SerializeAsString();
//    return result;
//  }
//
//  void ManagedEndpointSlot(const protobuf::ManagedEndpointMessage&,
//                           protobuf::ManagedEndpointMessage*,
//                           transport::Timeout*) {
//    boost::mutex::scoped_lock lock(slots_mutex_);
//    auto it = invoked_slots_->find(kManagedEndpointMessage);
//    if (it != invoked_slots_->end())
//      ++((*it).second);
//  }
//  void NatDetectionReqSlot(const protobuf::NatDetectionRequest&,
//                           protobuf::NatDetectionResponse* response,
//                           transport::Timeout*) {
//    boost::mutex::scoped_lock lock(slots_mutex_);
//    response->set_nat_type(0);
//    auto it = invoked_slots_->find(kNatDetectionRequest);
//    if (it != invoked_slots_->end())
//      ++((*it).second);
//  }
//  void NatDetectionRspSlot(const protobuf::NatDetectionResponse&) {
//    boost::mutex::scoped_lock lock(slots_mutex_);
//    auto it = invoked_slots_->find(kNatDetectionResponse);
//    if (it != invoked_slots_->end())
//      ++((*it).second);
//  }
//  void ProxyConnectReqSlot(const protobuf::ProxyConnectRequest&,
//                           protobuf::ProxyConnectResponse* response,
//                           transport::Timeout*) {
//    boost::mutex::scoped_lock lock(slots_mutex_);
//    response->set_result(true);
//    auto it = invoked_slots_->find(kProxyConnectRequest);
//    if (it != invoked_slots_->end())
//      ++((*it).second);
//  }
//  void ProxyConnectRspSlot(const protobuf::ProxyConnectResponse&) {
//    boost::mutex::scoped_lock lock(slots_mutex_);
//    auto it = invoked_slots_->find(kProxyConnectResponse);
//    if (it != invoked_slots_->end())
//      ++((*it).second);
//  }
//  void ForwardRendezvousReqSlot(const protobuf::ForwardRendezvousRequest&,
//                                protobuf::ForwardRendezvousResponse* response,
//                                transport::Timeout*) {
//    boost::mutex::scoped_lock lock(slots_mutex_);
//    protobuf::Endpoint *rv_endpoint =
//        response->mutable_receiver_rendezvous_endpoint();
//    rv_endpoint->set_ip("127.0.0.1");
//    rv_endpoint->set_port(9999);
//    auto it = invoked_slots_->find(kForwardRendezvousRequest);
//    if (it != invoked_slots_->end())
//      ++((*it).second);
//  }
//  void ForwardRendezvousRspSlot(const protobuf::ForwardRendezvousResponse&) {
//    boost::mutex::scoped_lock lock(slots_mutex_);
//    auto it = invoked_slots_->find(kForwardRendezvousResponse);
//    if (it != invoked_slots_->end())
//      ++((*it).second);
//  }
//  void RendezvousReqSlot(const protobuf::RendezvousRequest&) {
//    boost::mutex::scoped_lock lock(slots_mutex_);
//    auto it = invoked_slots_->find(kRendezvousRequest);
//    if (it != invoked_slots_->end())
//      ++((*it).second);
//  }
//  void RendezvousAckSlot(const protobuf::RendezvousAcknowledgement&) {
//    boost::mutex::scoped_lock lock(slots_mutex_);
//    auto it = invoked_slots_->find(kRendezvousAcknowledgement);
//    if (it != invoked_slots_->end())
//      ++((*it).second);
//  }
//  void ErrorSlot(const ReturnCode &tc) { error_count_ += tc; }
//
//  void ConnectToHandlerSignals() {
//    msg_hndlr_->on_managed_endpoint_message()->connect(boost::bind(
//        &TransportMessageHandlerTest::ManagedEndpointSlot, this, _1, _2, _3));
//    msg_hndlr_->on_nat_detection_request()->connect(boost::bind(
//        &TransportMessageHandlerTest::NatDetectionReqSlot, this, _1, _2, _3));
//    msg_hndlr_->on_nat_detection_response()->connect(boost::bind(
//        &TransportMessageHandlerTest::NatDetectionRspSlot, this, _1));
//    msg_hndlr_->on_proxy_connect_request()->connect(boost::bind(
//        &TransportMessageHandlerTest::ProxyConnectReqSlot, this, _1, _2, _3));
//    msg_hndlr_->on_proxy_connect_response()->connect(boost::bind(
//        &TransportMessageHandlerTest::ProxyConnectRspSlot, this, _1));
//    msg_hndlr_->on_forward_rendezvous_request()->connect(boost::bind(
//        &TransportMessageHandlerTest::ForwardRendezvousReqSlot,
//        this, _1, _2, _3));
//    msg_hndlr_->on_forward_rendezvous_response()->connect(boost::bind(
//        &TransportMessageHandlerTest::ForwardRendezvousRspSlot, this, _1));
//    msg_hndlr_->on_rendezvous_request()->connect(boost::bind(
//        &TransportMessageHandlerTest::RendezvousReqSlot, this, _1));
//    msg_hndlr_->on_rendezvous_acknowledgement()->connect(boost::bind(
//        &TransportMessageHandlerTest::RendezvousAckSlot, this, _1));
//    msg_hndlr_->on_error()->connect(boost::bind(
//        &TransportMessageHandlerTest::ErrorSlot, this, _1));
//  }
//  void InitialiseMap() {
//    invoked_slots_.reset(new std::map<MessageType, uint16_t>);
//    for (int n = kManagedEndpointMessage; n != kRendezvousAcknowledgement;
//        ++n)
//      invoked_slots_->insert(std::pair<MessageType, uint16_t>(
//                                       MessageType(n), 0));
//  }
//  std::vector<std::string> CreateMessages() {
//    protobuf::ManagedEndpointMessage me_msg;
//    protobuf::NatDetectionRequest nd_req;
//    protobuf::ProxyConnectRequest pc_req;
//    protobuf::ForwardRendezvousRequest fr_req;
//    protobuf::RendezvousRequest r_req;
//    protobuf::NatDetectionResponse nd_res;
//    protobuf::ProxyConnectResponse pc_res;
//    protobuf::ForwardRendezvousResponse fr_res;
//    protobuf::RendezvousAcknowledgement ra_msg;
//
//    protobuf::Endpoint ep;
//    ep.set_ip(std::string("192.168.1.1"));
//    ep.set_port(12345);
//    me_msg.mutable_endpoint()->CopyFrom(ep);
//    pc_req.mutable_endpoint()->CopyFrom(ep);
//    fr_req.mutable_receiver_endpoint()->CopyFrom(ep);
//    r_req.mutable_originator_endpoint()->CopyFrom(ep);
//    nd_res.mutable_endpoint()->CopyFrom(ep);
//    fr_res.mutable_receiver_rendezvous_endpoint()->CopyFrom(ep);
//    ra_msg.mutable_originator_endpoint()->CopyFrom(ep);
//
//    nd_req.set_local_port(12021);
//    nd_res.set_nat_type(0);
//    pc_req.set_rendezvous_connect(true);
//    pc_res.set_result(true);
//
//
//    EXPECT_TRUE(me_msg.IsInitialized());
//    EXPECT_TRUE(nd_req.IsInitialized());
//    EXPECT_TRUE(pc_req.IsInitialized());
//    EXPECT_TRUE(fr_req.IsInitialized());
//    EXPECT_TRUE(r_req.IsInitialized());
//    EXPECT_TRUE(nd_res.IsInitialized());
//    EXPECT_TRUE(pc_res.IsInitialized());
//    EXPECT_TRUE(fr_res.IsInitialized());
//    EXPECT_TRUE(ra_msg.IsInitialized());
//
//    std::vector<std::string> messages;
//    transport::protobuf::WrapperMessage wrap;
//    wrap.set_msg_type(kManagedEndpointMessage);
//    wrap.set_payload(me_msg.SerializeAsString());
//    messages.push_back(std::string(1, kNone) + wrap.SerializeAsString());
//
//    return messages;
//  }
//  void ExecuteThread(std::vector<std::string> messages_copy, int rounds) {
//    Info info;
//    std::string response;
//    Timeout timeout;
//
//    uint32_t random_sleep((RandomUint32() % 100) + 100);
//    for (int a = 0; a < rounds; ++a) {
//      Sleep(boost::posix_time::milliseconds(random_sleep));
//      for (size_t n = 0; n < messages_copy.size(); ++n)
//        msg_hndlr_->OnMessageReceived(messages_copy[n], info, &response,
//                                      &timeout);
//    }
//  }
//
//  std::shared_ptr<std::map<MessageType, uint16_t>> invoked_slots() {
//    return invoked_slots_;
//  }
//  int error_count() { return error_count_; }
//
// protected:
//  static asymm::Keys crypto_key_pair_;
//  std::shared_ptr<asymm::PrivateKey> private_key_;
//  std::shared_ptr<MessageHandler> msg_hndlr_;
//  std::shared_ptr<asymm::PrivateKey> asym_null_private_key_;
//  MessageHandler msg_hndlr_no_securifier_;
//  std::shared_ptr<std::map<MessageType, uint16_t>> invoked_slots_;
//  boost::mutex slots_mutex_;
//  int error_count_;
// };

// asymm::Keys TransportMessageHandlerTest::crypto_key_pair_;
//
// TEST_F(TransportMessageHandlerTest, BEH_OnError) {
//  ConnectToHandlerSignals();
//
//  int errors(0);
//  for (int tc = transport::kError;
//       tc != transport::kMessageSizeTooLarge; --tc) {
//    errors += tc;
//    msg_hndlr_->OnError(transport::ReturnCode(tc), Endpoint());
//  }
//
//  ASSERT_EQ(errors, error_count());
// }
//
// TEST_F(TransportMessageHandlerTest, BEH_OnMessageNullSecurifier) {
//  ConnectToHandlerSignals();
//  InitialiseMap();
//  std::vector<std::string> messages(CreateMessages());
//
//  Info info;
//  std::string response;
//  Timeout timeout;
//  for (size_t n = 0; n < messages.size(); ++n)
//    msg_hndlr_no_securifier_.OnMessageReceived(
//        std::string(1, kasymmmetricEncrypt) + messages[n],
//        info, &response, &timeout);
//  std::shared_ptr<std::map<MessageType,
//                  uint16_t>> slots = invoked_slots();
//  for (auto it = slots->begin(); it != slots->end(); ++it)
//    ASSERT_EQ(uint16_t(0), (*it).second);
//
//  slots->clear();
//  InitialiseMap();
//  for (size_t n = 0; n < messages.size(); ++n)
//    msg_hndlr_->OnMessageReceived(
//        std::string(1, kasymmmetricEncrypt) + messages[n],
//        info, &response, &timeout);
//  for (auto it = slots->begin(); it != slots->end(); ++it)
//    ASSERT_EQ(uint16_t(0), (*it).second);
//
//  slots->clear();
//  InitialiseMap();
//  for (size_t n = 0; n < messages.size(); ++n)
//    msg_hndlr_->OnMessageReceived("", info, &response, &timeout);
//  for (auto it = slots->begin(); it != slots->end(); ++it)
//    ASSERT_EQ(uint16_t(0), (*it).second);
// }
//
// TEST_F(TransportMessageHandlerTest, BEH_WrapMessageManagedEndpointMessage) {  // NOLINT
//  protobuf::ManagedEndpointMessage managed_endpoint_message;
//  ASSERT_TRUE(managed_endpoint_message.IsInitialized());
//
//  std::string function(msg_hndlr_->WrapMessage(managed_endpoint_message));
//  std::string manual(EncryptMessage<protobuf::ManagedEndpointMessage>(
//                         managed_endpoint_message, kManagedEndpointMessage));
//  EXPECT_EQ(manual, function);
// }
//
// TEST_F(TransportMessageHandlerTest, BEH_OnMessageReceived) {
//  ConnectToHandlerSignals();
//  InitialiseMap();
//  std::vector<std::string> messages(CreateMessages());
//
//  Info info;
//  std::string response;
//  Timeout timeout;
//  for (size_t n = 0; n < messages.size(); ++n)
//    msg_hndlr_->OnMessageReceived(messages[n], info, &response, &timeout);
//
//  std::shared_ptr<std::map<MessageType, uint16_t>> slots = invoked_slots();
//  for (auto it = slots->begin(); it != slots->end(); ++it)
//    ASSERT_EQ(uint16_t(1), (*it).second);
// }
//
// TEST_F(TransportMessageHandlerTest, BEH_ThreadedMessageHandling) {
//  ConnectToHandlerSignals();
//  InitialiseMap();
//  std::vector<std::string> messages(CreateMessages());
//
//  uint8_t thread_count((RandomUint32() % 5) + 4);
//  uint16_t total_messages(0);
//  boost::thread_group thg;
//  for (uint8_t n = 0; n < thread_count; ++n) {
//    uint16_t rounds((RandomUint32() % 5) + 4);
//    thg.create_thread(std::bind(&TransportMessageHandlerTest::ExecuteThread,
//                                this, messages, rounds));
//    total_messages += rounds;
//  }
//
//  thg.join_all();
//  std::shared_ptr<std::map<MessageType,
//                  uint16_t>> slots = invoked_slots();
//  for (auto it = slots->begin(); it != slots->end(); ++it)
//    ASSERT_EQ(uint16_t(total_messages), (*it).second);
// }
//
// TEST_F(TransportMessageHandlerTest, BEH_MakeSerialisedWrapperMessage) {
//  std::string payload(RandomString(5 * 1024));
//  ASSERT_TRUE(msg_hndlr_no_securifier_.MakeSerialisedWrapperMessage(0,
//      payload, kasymmmetricEncrypt, crypto_key_pair_.public_key).empty());
//  ASSERT_TRUE(msg_hndlr_no_securifier_.MakeSerialisedWrapperMessage(0,
//      payload, kSignAndAsymEncrypt, crypto_key_pair_.public_key).empty());
//
//  ASSERT_EQ("", msg_hndlr_->MakeSerialisedWrapperMessage(0,
//                                                         payload,
//                                                         kasymmmetricEncrypt,
//                                                         asymm::PublicKey()));
//  ASSERT_EQ("", msg_hndlr_->MakeSerialisedWrapperMessage(0,
//                                                         payload,
//                                                         kSignAndAsymEncrypt,
//                                                         asymm::PublicKey()));
//
//  ASSERT_FALSE(msg_hndlr_->MakeSerialisedWrapperMessage(0, payload,
//               kasymmmetricEncrypt, crypto_key_pair_.public_key).empty());
//  ASSERT_FALSE(msg_hndlr_->MakeSerialisedWrapperMessage(0, payload,
//               kSignAndAsymEncrypt, crypto_key_pair_.public_key).empty());
// }

}  // namespace test

}  // namespace rudp

}  // namespace maidsafe
