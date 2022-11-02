// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202210

/**
 * @file
 *
 * @brief posix的信号量
 *
 *
 * 参看：https://man7.org/linux/man-pages/man7/sem_overview.7.html
 *
 */

#ifdef __linux__

#include "ipc/Semaphores.h"

#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>

#include "utils/macro.h"

namespace ipc {

Semaphores Semaphores::open(const std::string &key, std::error_code &ec) {
  CHECK_EC(ec, Semaphores());
  Semaphores result;
  if ((result.sem_ = sem_open(key.c_str(), O_RDWR, 0, 0)) == SEM_FAILED) {
    ec = {errno, std::system_category()};
    return result;
  }
  result.key_ = key;
  return result;
}

Semaphores Semaphores::create(const std::string &key, std::error_code &ec) {
  CHECK_EC(ec, Semaphores());
  Semaphores result;
  //  delete O_EXCL will create force
  if ((result.sem_ = sem_open(key.c_str(), O_RDWR | O_CREAT | O_EXCL,
                              DEFFILEMODE, 0)) == SEM_FAILED) {
    ec = {errno, std::system_category()};
    return result;
  }
  result.key_ = key;
  return result;
}

void Semaphores::wait(std::error_code &ec) {
  CHECK_EC(ec, );
  if (::sem_wait((sem_t *)sem_) == -1) {
    ec = {errno, std::system_category()};
  }
}

bool Semaphores::tryWait(std::error_code &ec) {
  CHECK_EC(ec, false);
  if (::sem_trywait((sem_t *)sem_) == -1) {
    int e = errno;
    if (e != EAGAIN) {
      ec = {e, std::system_category()};
    }
    return false;
  }
  return true;
}

bool Semaphores::tryWaitFor(size_t timeout_ms, std::error_code &ec) {
  CHECK_EC(ec, false);
  struct timespec timeout {};
  if (clock_gettime(CLOCK_REALTIME, &timeout) == -1) {
    ec = {errno, std::system_category()};
    return false;
  }
  timeout.tv_sec += timeout_ms / 1000;
  timeout.tv_nsec += timeout_ms % 1000 * 1000000;
  timeout.tv_sec += timeout.tv_nsec / 1000000000;
  timeout.tv_nsec = timeout.tv_nsec % 1000000000;

  if (::sem_timedwait((sem_t *)sem_, &timeout) == -1) {
    int e = errno;
    if (e != ETIMEDOUT) {
      ec = {e, std::system_category()};
    }
    return false;
  }
  return true;
}

void Semaphores::notifyOne(std::error_code &ec) {
  CHECK_EC(ec, );
  if (::sem_post((sem_t *)sem_) == -1) {
    ec = {errno, std::system_category()};
    return;
  }
}

void Semaphores::close(std::error_code &ec) {
  CHECK_EC(ec, );

  if (::sem_close((sem_t *)sem_) == -1) {
    ec = {errno, std::system_category()};
    return;
  }
  sem_ = nullptr;
}

void Semaphores::remove(std::error_code &ec) {
  CHECK_EC(ec, );
  close(ec);
  CHECK_EC(ec, );

  if (sem_unlink(key_.c_str()) == -1) {
    ec = {errno, std::system_category()};
    return;
  }
}

} // namespace ipc

#endif // __linux__
