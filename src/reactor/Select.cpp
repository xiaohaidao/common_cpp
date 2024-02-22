// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202211

#include "reactor/Select.h"

#include <limits>

#include "utils/error_code.h"

#define POST_SOCKET(s, op_size, ops, set, op, ec)                              \
  do {                                                                         \
    if ((op_size) >= (ops).size()) {                                           \
      (ec) = {EINVAL, std::system_category()};                                 \
      return;                                                                  \
    }                                                                          \
    size_t index = (ops).size();                                               \
    for (size_t i = 0; i < (op_size); ++i) {                                   \
      if ((ops)[i].first == (s)) {                                             \
        index = i;                                                             \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (index == (ops).size()) {                                               \
      index = op_size;                                                         \
      ++(op_size);                                                             \
    }                                                                          \
    FD_SET(s, &(set));                                                         \
    (ops)[index] = {s, op};                                                    \
  } while (false)

#define REMOVE_SOCKET(s, op_size, ops, set)                                    \
  do {                                                                         \
    for (size_t i = 0; i < (op_size); ++i) {                                   \
      if ((ops)[i].first == (s)) {                                             \
        std::swap((ops)[i], (ops)[(op_size)-1]);                               \
        --(op_size);                                                           \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    FD_CLR(s, &(set));                                                         \
  } while (false)

#define FIND_SOCKET(queue, op_size, ops, set, re_size)                         \
  do {                                                                         \
    for (size_t i = 0; i < (op_size); ++i) {                                   \
      if (FD_ISSET((ops)[i].first, &(set))) {                                  \
        ReactorOp *ptr = (ops)[i].second;                                      \
        if (!(queue).exist(ptr)) {                                             \
          (queue).push(ptr);                                                   \
          ++(re_size);                                                         \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while (false)

Select::Select()
    : map_read_op_size_(0), map_write_op_size_(0), map_except_op_size_(0) {
  FD_ZERO(&read_);
  FD_ZERO(&write_);
  FD_ZERO(&except_);
}

Select::Select(std::error_code & /*ec*/)
    : map_read_op_size_(0), map_write_op_size_(0), map_except_op_size_(0) {
  FD_ZERO(&read_);
  FD_ZERO(&write_);
  FD_ZERO(&except_);
}

void Select::post_read(socket_type s, ReactorOp *op, std::error_code &ec) {
  POST_SOCKET(s, map_read_op_size_, map_read_op_, read_, op, ec);
  fd_ = (std::max)(fd_, s + 1);
}

void Select::post_write(socket_type s, ReactorOp *op, std::error_code &ec) {
  POST_SOCKET(s, map_write_op_size_, map_write_op_, write_, op, ec);
  fd_ = (std::max)(fd_, s + 1);
}

void Select::post_except(socket_type s, ReactorOp *op, std::error_code &ec) {
  POST_SOCKET(s, map_except_op_size_, map_except_op_, except_, op, ec);
  fd_ = (std::max)(fd_, s + 1);
}

void Select::cancel(socket_type s, std::error_code & /*ec*/) {
  REMOVE_SOCKET(s, map_read_op_size_, map_read_op_, read_);
  REMOVE_SOCKET(s, map_write_op_size_, map_write_op_, write_);
  REMOVE_SOCKET(s, map_except_op_size_, map_except_op_, except_);
}

size_t Select::call(QueueOp &queue) {
  size_t n = 0;
  while (call_one(queue))
    ++n;
  return n;
}

size_t Select::call_one(QueueOp &queue) {
  if (ReactorOp *op = (ReactorOp *)queue.begin()) {
    op->complete(this, std::error_code(), 0);
    queue.pop();
    return 1;
  }
  return 0;
}

size_t Select::run_once(QueueOp &queue, std::error_code &ec) {
  return run_once_timeout(queue, (std::numeric_limits<size_t>::max)(), ec);
}

size_t Select::run_once_timeout(QueueOp &queue, size_t timeout_ms,
                                std::error_code &ec) {
  struct timeval time = {};
  time.tv_usec = timeout_ms % 1000u * 1000u;
  time.tv_sec = static_cast<long>(timeout_ms / 1000u);
  fd_type readability = read_;
  fd_type writability = write_;
  fd_type check_except = except_;
  int const re = ::select(
      static_cast<int>(fd_), &readability, &writability, &check_except,
      timeout_ms == (std::numeric_limits<size_t>::max)() ? nullptr : &time);
  if (re < 0) {
    ec = get_net_error_code();
    return 0;
  }
  if (re == 0) { // timeout
    return 0;
  }
  size_t re_size = 0;
  FIND_SOCKET(queue, map_read_op_size_, map_read_op_, readability, re_size);
  FIND_SOCKET(queue, map_write_op_size_, map_write_op_, writability, re_size);
  FIND_SOCKET(queue, map_except_op_size_, map_except_op_, check_except,
              re_size);
  return re_size;
}
