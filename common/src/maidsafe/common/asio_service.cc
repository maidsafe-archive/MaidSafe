/* Copyright (c) 2012 maidsafe.net limited
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "maidsafe/common/asio_service.h"
#include "maidsafe/common/log.h"


namespace maidsafe {

AsioService::AsioService() : service_(), work_(), thread_group_() {}

AsioService::~AsioService() {
  Stop();
}

void AsioService::Start(const uint32_t &thread_count) {
  if (work_) {
    DLOG(ERROR) << "AsioService is already running with "
                << thread_group_->size() << " threads.";
    return;
  }
  service_.reset();
  work_.reset(new boost::asio::io_service::work(service_));
  thread_group_.reset(new boost::thread_group);
  for (uint32_t i = 0; i != thread_count; ++i) {
    thread_group_->create_thread([&]() {
        for (;;) {
          try {
            service_.run();
            if (!work_)
              break;
          }
          catch(const boost::system::system_error &e) {
            DLOG(ERROR) << e.what();
          }
        }
    });
  }
}

void AsioService::Stop() {
  work_.reset();
  service_.stop();
  if (thread_group_) {
    thread_group_->interrupt_all();
    thread_group_->join_all();
  }
}

boost::asio::io_service& AsioService::service() {
  return service_;
}

}  // namespace maidsafe
