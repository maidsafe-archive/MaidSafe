/*******************************************************************************
 *  Copyright 2012 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ******************************************************************************/


#ifndef MAIDSAFE_ROUTING_TIMER_H_
#define MAIDSAFE_ROUTING_TIMER_H_

#include "boost/asio.hpp"
#include "maidsafe/common/asio_service.h"
#include "maidsafe/common/utils.h"
#include "maidsafe/routing/routing_pb.h"

namespace maidsafe {

namespace routing {

namespace asio = boost::asio;
typedef std::function<void(int, std::string)> TaskResponseFunctor;
typedef uint32_t TaskId;
class Timer {
 public:
  explicit Timer(AsioService &io_service);
  typedef std::shared_ptr<asio::deadline_timer> TimerPointer;
  TaskId AddTask(uint32_t timeout, const TaskResponseFunctor &);
  void KillTask(uint32_t task_id);  // removes from queue immediately no run
  void ExecuteTaskNow(protobuf::Message &message);  //executes and removes task
 private:
  Timer &operator=(const Timer&);
  Timer(const Timer&);
  Timer(const Timer&&);
  AsioService &io_service_;
  TaskId task_id_;
  std::map<uint32_t, std::pair<TimerPointer, TaskResponseFunctor> > queue_;
};

}  // namespace routing

}  // namespace maidsafe


#endif  // MAIDSAFE_ROUTING_TIMER_H_
