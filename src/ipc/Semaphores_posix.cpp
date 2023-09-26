// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202210

/**
 * @file
 *
 * @brief semaphores of posix
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

#include "utils/error_code.h"
#include "utils/macro.h"

namespace ipc {

Semaphores::Semaphores() : sem_(nullptr) {}

Semaphores::~Semaphores() {
  std::error_code ec;
  close(ec);
}

Semaphores Semaphores::open(const char *key, std::error_code &ec) {
  Semaphores result;
  if ((result.sem_ = sem_open(key, O_RDWR, 0, 0)) == SEM_FAILED) {
    ec = getErrorCode();
    return result;
  }
  result.key_ = key;
  return result;
}

Semaphores Semaphores::create(const char *key, std::error_code &ec,
                              unsigned int number) {
  Semaphores result;
  //  delete O_EXCL will create force
  if ((result.sem_ = sem_open(key, O_RDWR | O_CREAT | O_EXCL, DEFFILEMODE,
                              number)) == SEM_FAILED) {
    ec = getErrorCode();
    return result;
  }
  result.key_ = key;
  return result;
}

void Semaphores::wait(std::error_code &ec) {
  if (::sem_wait((sem_t *)sem_) == -1) {
    ec = getErrorCode();
  }
}

bool Semaphores::tryWait(std::error_code &ec) {
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
  struct timespec timeout {};
  if (clock_gettime(CLOCK_REALTIME, &timeout) == -1) {
    ec = getErrorCode();
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
  if (::sem_post((sem_t *)sem_) == -1) {
    ec = getErrorCode();
    return;
  }
}

void Semaphores::close(std::error_code &ec) {
  if (::sem_close((sem_t *)sem_) == -1) {
    ec = getErrorCode();
    return;
  }
  sem_ = nullptr;
}

void Semaphores::remove(std::error_code &ec) {
  close(ec);

  if (sem_unlink(key_.c_str()) == -1) {
    ec = getErrorCode();
    return;
  }
}

} // namespace ipc

#endif // __linux__
