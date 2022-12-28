// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202210

#ifndef __linux__

#include "ipc/MsgQueue.h"

namespace ipc {

MsgQueue MsgQueue::open(const std::string &key, std::error_code &ec) {
  return MsgQueue();
}

MsgQueue MsgQueue::create(const std::string &key, std::error_code &ec) {
  return MsgQueue();
}

void MsgQueue::send(const char *data, size_t size, std::error_code &ec) {}

bool MsgQueue::sendTimeout(const char *data, size_t size, size_t timeout_ms,
                           std::error_code &ec) {

  return !ec;
}

size_t MsgQueue::recv(char *data, size_t data_size, std::error_code &ec) {
  return 0;
}

size_t MsgQueue::recvTimeout(char *data, size_t data_size, size_t timeout_ms,
                             std::error_code &ec) {
  return 0;
}

void MsgQueue::close(std::error_code &ec) {}

void MsgQueue::remove(std::error_code &ec) {}

} // namespace ipc

#endif // __linux__
