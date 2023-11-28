// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202210

/**
 * @file
 *
 * @brief posix message queue
 *
 * the queue size defaule is 10，the message size default is the most 8192
 * constexpr size_t msg_size = 8192;
 * constexpr size_t msg_queue_size = 10;
 *
 * 参看：https://man7.org/linux/man-pages/man7/mq_overview.7.html
 *
 */

#ifdef __linux__

#include "ipc/MsgQueue.h"

#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils/error_code.h"
#include "utils/macro.h"

namespace ipc {

std::string transferName(const std::string &name) {
  if (name.size() > 0 && name[0] != '/') {
    return "/" + name;
  }
  return name;
}

MsgQueue MsgQueue::open(const std::string &key, std::error_code &ec) {
  MsgQueue result;
  if ((result.msgid_ = mq_open(transferName(key).c_str(), O_RDWR)) == -1) {
    ec = get_error_code();
    return result;
  }
  result.key_ = transferName(key);
  return result;
}

MsgQueue MsgQueue::create(const std::string &key, std::error_code &ec) {
  MsgQueue result;
  //  delete O_EXCL will create force
  if ((result.msgid_ =
           mq_open(transferName(key).c_str(), O_RDWR | O_CREAT | O_EXCL,
                   DEFFILEMODE, nullptr)) == -1) {
    ec = get_error_code();
    return result;
  }
  result.key_ = transferName(key);
  return result;
}

void MsgQueue::send(const char *data, size_t size, std::error_code &ec) {
  constexpr int priority = 0; // the priority in [0-31], highest priority first
  if (mq_send(msgid_, data, size, priority) == -1) {
    ec = get_error_code();
    return;
  }
}

bool MsgQueue::sendTimeout(const char *data, size_t size, size_t timeout_ms,
                           std::error_code &ec) {

  struct timespec timeout {};
  if (clock_gettime(CLOCK_REALTIME, &timeout) == -1) {
    ec = get_error_code();
    return false;
  }
  timeout.tv_sec += timeout_ms / 1000;
  timeout.tv_nsec += timeout_ms % 1000 * 1000000;
  timeout.tv_sec += timeout.tv_nsec / 1000000000;
  timeout.tv_nsec = timeout.tv_nsec % 1000000000;

  constexpr int priority = 0; // the priority in [0-31], highest priority first
  if (mq_timedsend(msgid_, data, size, priority, &timeout) == -1) {
    int e_code = errno;
    if (e_code != ETIMEDOUT) {
      ec = {e_code, std::system_category()};
    }
    return false;
  }
  return true;
}

size_t MsgQueue::recv(char *data, size_t data_size, std::error_code &ec) {
  size_t size;
  if ((size = mq_receive(msgid_, data, data_size, nullptr)) == -1) {
    ec = get_error_code();
    return 0;
  }
  return size;
}

size_t MsgQueue::recvTimeout(char *data, size_t data_size, size_t timeout_ms,
                             std::error_code &ec) {

  struct timespec timeout {};
  if (clock_gettime(CLOCK_REALTIME, &timeout) == -1) {
    ec = get_error_code();
    return 0;
  }
  timeout.tv_sec += timeout_ms / 1000;
  timeout.tv_nsec += timeout_ms % 1000 * 1000000;
  timeout.tv_sec += timeout.tv_nsec / 1000000000;
  timeout.tv_nsec = timeout.tv_nsec % 1000000000;

  size_t size = 0;
  if ((size = mq_timedreceive(msgid_, data, data_size, nullptr, &timeout)) ==
      -1) {
    int e_code = errno;
    if (e_code != ETIMEDOUT) {
      ec = {e_code, std::system_category()};
    }
    return 0;
  }
  return size;
}

void MsgQueue::close(std::error_code &ec) {
  if (msgid_ <= 0) {
    return;
  }
  if (mq_close(msgid_) == -1) {
    ec = get_error_code();
    return;
  }
  msgid_ = 0;
}

void MsgQueue::remove(std::error_code &ec) {
  close(ec);

  if (mq_unlink(key_.c_str()) == -1) {
    ec = get_error_code();
    return;
  }
}

} // namespace ipc

#endif // __linux__
