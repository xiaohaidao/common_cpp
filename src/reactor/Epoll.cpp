
#ifdef __linux__

#include "reactor/Epoll.h"

#include <sys/epoll.h>

#include "utils/error_code.h"

Epoll::Epoll() : fd_(-1) {}

Epoll::Epoll(std::error_code &ec) : fd_(epoll_create1(EPOLL_CLOEXEC)) {
  if (fd_ == -1) {
    ec = getErrorCode();
  }
}

void Epoll::post(int fd, ReactorOp *op, int or_op_enum, std::error_code &ec) {
  struct epoll_event event = {};
  event.events = or_op_enum;
  event.data.ptr = op;
  if (epoll_ctl(fd_, EPOLL_CTL_ADD, fd, &event)) {
    std::error_code re_ec = getErrorCode();
    if (re_ec.value() != EEXIST) {
      ec = re_ec;
      return;
    }
    if (epoll_ctl(fd_, EPOLL_CTL_MOD, fd, &event)) {
      ec = getErrorCode();
    }
  }
}

void Epoll::post_read(int fd, ReactorOp *op, std::error_code &ec) {
  post(fd, op, READ_OP_ENUM, ec);
}

void Epoll::post_write(int fd, ReactorOp *op, std::error_code &ec) {
  post(fd, op, WRITE_OP_ENUM, ec);
}

void Epoll::depost(int fd, std::error_code &ec) {
  struct epoll_event event = {};
  if (epoll_ctl(fd_, EPOLL_CTL_DEL, fd, &event)) {
    ec = getErrorCode();
  }
}

size_t Epoll::call(QueueOp &queue) {
  size_t n = 0;
  while (call_one(queue))
    ++n;
  return n;
}

size_t Epoll::call_one(QueueOp &queue) {
  if (ReactorOp *op = (ReactorOp *)queue.begin()) {
    op->complete(this, std::error_code(), 0);
    queue.pop();
    return 1;
  }
  return 0;
}

size_t Epoll::run_once(QueueOp &queue, std::error_code &ec) {
  return run_once_timeout(queue, -1, ec);
}

size_t Epoll::run_once_timeout(QueueOp &queue, int timeout_ms,
                               std::error_code &ec) {

  struct epoll_event events[128];
  int number = epoll_wait(fd_, events, sizeof(events), timeout_ms);
  if (number < 0) {
    ec = getErrorCode();
    return 0;
  }
  for (size_t i = 0; i < number; ++i) {
    ReactorOp *ptr = (ReactorOp *)events[i].data.ptr;
    ptr->set_event_data(events[i].events);
    queue.push(ptr);
  }
  return number;
}

#endif // __linux__
