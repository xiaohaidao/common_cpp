// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202210

/**
 * @file
 *
 * @brief posix的消息队列
 *
 * 队列大小默认为10，消息大小默认最大为8192
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

#include "utils/log.h"

namespace ipc {

#define CHECK_EC(ec, re)                                                       \
  do {                                                                         \
    if (ec) {                                                                  \
      return re;                                                               \
    }                                                                          \
  } while (false)

void getSystemAttri(mqd_t mq) {
  struct mq_attr attr;
  if (mq_getattr(mq, &attr) != -1) {
    LOG_TRACE("system maxinum # of message on queue : %ld", attr.mq_maxmsg);
    LOG_TRACE("system maxinum message size : %ld", attr.mq_msgsize);
  }
}

std::string transferName(const std::string &name) {
  if (name.size() == 0) {
    return name;
  }
  if (name[0] != '/') {
    return "/" + name;
  }
  return name;
}

MsgQueue MsgQueue::get(const std::string &key, std::error_code &ec) {
  CHECK_EC(ec, MsgQueue());
  MsgQueue result;
  if ((result.msgid_ = mq_open(transferName(key).c_str(), O_RDWR)) == -1) {
    ec = {errno, std::system_category()};
    return result;
  }
  // getSystemAttri(result.msgid_);
  result.key_ = transferName(key);
  return result;
}

MsgQueue MsgQueue::create(const std::string &key, std::error_code &ec) {
  CHECK_EC(ec, MsgQueue());
  MsgQueue result;
  //  delete O_EXCL will create force
  if ((result.msgid_ =
           mq_open(transferName(key).c_str(), O_RDWR | O_CREAT | O_EXCL,
                   DEFFILEMODE, nullptr)) == -1) {
    ec = {errno, std::system_category()};
    return result;
  }
  // getSystemAttri(result.msgid_);
  result.key_ = transferName(key);
  return result;
}

void MsgQueue::send(const char *data, size_t size, std::error_code &ec) {
  CHECK_EC(ec, );
  constexpr int priority = 0; // the priority in [0-31], highest priority first
  if (mq_send(msgid_, data, size, priority) == -1) {
    ec = {errno, std::system_category()};
    return;
  }
}

void MsgQueue::sendTimeout(const char *data, size_t size, size_t timeout_ms,
                           std::error_code &ec) {

  CHECK_EC(ec, );

  struct timespec timeout {};
  timeout.tv_sec = timeout_ms / 1000;
  timeout.tv_nsec = timeout_ms % 1000 * 1000000;

  constexpr int priority = 0; // the priority in [0-31], highest priority first
  if (mq_timedsend(msgid_, data, size, priority, &timeout) == -1) {
    ec = {errno, std::system_category()};
    return;
  }
}

size_t MsgQueue::recv(char *data, size_t data_size, std::error_code &ec) {
  CHECK_EC(ec, 0);
  size_t size;
  if ((size = mq_receive(msgid_, data, data_size, nullptr)) == -1) {
    ec = {errno, std::system_category()};
    return 0;
  }
  return size;
}

size_t MsgQueue::recvTimeout(char *data, size_t data_size, size_t timeout_ms,
                             std::error_code &ec) {
  CHECK_EC(ec, 0);

  struct timespec timeout {};
  timeout.tv_sec = timeout_ms / 1000;
  timeout.tv_nsec = timeout_ms % 1000 * 1000000;

  size_t size;
  if ((size = mq_timedreceive(msgid_, data, data_size, nullptr, &timeout)) ==
      -1) {
    ec = {errno, std::system_category()};
    return 0;
  }
  return size;
}

void MsgQueue::close(std::error_code &ec) {
  CHECK_EC(ec, );

  if (msgid_ <= 0) {
    return;
  }
  if (mq_close(msgid_) == -1) {
    ec = {errno, std::system_category()};
    return;
  }
  msgid_ = 0;
}

void MsgQueue::remove(std::error_code &ec) {
  CHECK_EC(ec, );
  close(ec);
  CHECK_EC(ec, );

  if (mq_unlink(key_.c_str()) == -1) {
    ec = {errno, std::system_category()};
    return;
  }
}

} // namespace ipc

#endif // __linux__
