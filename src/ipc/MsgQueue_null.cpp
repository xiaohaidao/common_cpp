// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202210

#ifndef __linux__

#include "ipc/MsgQueue.h"

namespace ipc {

msg_queue msg_queue::open(const std::string & /*key*/,
                          std::error_code & /*ec*/) {
  return msg_queue();
}

msg_queue msg_queue::create(const std::string & /*key*/,
                            std::error_code & /*ec*/) {
  return msg_queue();
}

void msg_queue::send(const char *data, size_t size, std::error_code &ec) {}

bool msg_queue::send_timeout(const char * /*data*/, size_t /*size*/,
                             size_t /*timeout_ms*/, std::error_code &ec) {

  return !ec;
}

size_t msg_queue::recv(char * /*data*/, size_t /*data_size*/,
                       std::error_code & /*ec*/) {
  return 0;
}

size_t msg_queue::recv_timeout(char * /*data*/, size_t /*data_size*/,
                               size_t /*timeout_ms*/,
                               std::error_code & /*ec*/) {
  return 0;
}

void msg_queue::close(std::error_code &ec) {}

void msg_queue::remove(std::error_code &ec) {}

} // namespace ipc

#endif // __linux__
