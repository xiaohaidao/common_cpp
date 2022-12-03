// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202211

#ifdef _WIN32

#include "reactor/Select.h"

#include <limits>

#include "utils/error_code.h"

Select::Select() {
  FD_ZERO(&read_);
  FD_ZERO(&write_);
  FD_ZERO(&except_);
  FD_ZERO(&readability_);
  FD_ZERO(&writability_);
  FD_ZERO(&check_except_);
}

void Select::post_read(socket_type s, std::function<void(Select &)> f) {
  FD_SET(s, &read_);
  map_read_[s] = f;
}

void Select::post_write(socket_type s, std::function<void(Select &)> f) {
  FD_SET(s, &write_);
  map_write_[s] = f;
}

void Select::post_except(socket_type s, std::function<void(Select &)> f) {
  FD_SET(s, &write_);
  map_except_[s] = f;
}

void Select::depost(socket_type s) {
  FD_CLR(s, &read_);
  map_read_.erase(s);
  FD_CLR(s, &write_);
  map_write_.erase(s);
  FD_CLR(s, &except_);
  map_except_.erase(s);
}

void Select::run_one(std::error_code &ec) {
  run_one_timeout((std::numeric_limits<size_t>::max)(), ec);
}

void Select::run_one_timeout(size_t timeout_ms, std::error_code &ec) {
  if (!wait_timeout(timeout_ms, ec)) {
    return;
  }
  for (auto iter = map_read_.begin(); iter != map_read_.end(); ++iter) {
    if (readability(iter->first)) {
      iter->second(*this);
    }
  }

  for (auto iter = map_write_.begin(); iter != map_write_.end(); ++iter) {
    if (writability(iter->first)) {
      iter->second(*this);
    }
  }

  for (auto iter = map_except_.begin(); iter != map_except_.end(); ++iter) {
    if (check_except(iter->first)) {
      iter->second(*this);
    }
  }
}

void Select::wait(std::error_code &ec) {
  wait_timeout((std::numeric_limits<size_t>::max)(), ec);
}

bool Select::wait_timeout(size_t timeout_ms, std::error_code &ec) {
  struct timeval time = {};
  time.tv_usec = timeout_ms % 1000 * 1000;
  time.tv_sec = timeout_ms / 1000;
  readability_ = read_;
  writability_ = write_;
  check_except_ = except_;
  int re = ::select(0, &readability_, &writability_, &check_except_,
                    timeout_ms == (std::numeric_limits<size_t>::max)() ? nullptr
                                                                       : &time);
  if (re == SOCKET_ERROR) {
    ec = getNetErrorCode();
  }
  if (re <= 0) {
    // FD_ZERO(&readability_);
    // FD_ZERO(&writability_);
    // FD_ZERO(&check_except_);
    // for (auto iter = map_read_.begin(); iter != map_read_.end(); ++iter) {
    //   if (readability(iter->first)) {
    //     printf("readability %d\n", iter->first);
    //   }
    // }

    // for (auto iter = map_write_.begin(); iter != map_write_.end(); ++iter) {
    //   if (writability(iter->first)) {
    //     printf("writability %d\n", iter->first);
    //   }
    // }

    // for (auto iter = map_except_.begin(); iter != map_except_.end(); ++iter)
    // {
    //   if (check_except(iter->first)) {
    //     printf("check_except %d\n", iter->first);
    //   }
    // }
  }
  return re > 0;
}

bool Select::readability(socket_type s) { return FD_ISSET(s, &readability_); }

bool Select::writability(socket_type s) { return FD_ISSET(s, &writability_); }

bool Select::check_except(socket_type s) { return FD_ISSET(s, &check_except_); }

#endif // _WIN32
