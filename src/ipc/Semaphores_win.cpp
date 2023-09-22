// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202210

/**
 * @file
 *
 * @brief semaphores of windows
 *
 *
 * 参看：https://learn.microsoft.com/en-us/windows/win32/sync/semaphore-objects
 *
 */

#ifdef _WIN32

#include "ipc/Semaphores.h"

#include <windows.h>

#include "utils/error_code.h"
#include "utils/macro.h"

namespace ipc {

Semaphores Semaphores::open(const std::string &key, std::error_code &ec) {
  Semaphores result;
  std::string key_g = std::string("Global\\") + key;
  HANDLE han = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, key_g.c_str());
  if (han == nullptr) {
    ec = getErrorCode();
    return result;
  }
  result.sem_ = han;
  return result;
}

Semaphores Semaphores::create(const std::string &key, std::error_code &ec) {
  Semaphores result;
  std::string key_g = std::string("Global\\") + key;
  HANDLE han = CreateSemaphore(nullptr, 1, 256, key_g.c_str());
  std::error_code er_code = getErrorCode();
  if (han == nullptr || er_code.value() == ERROR_ALREADY_EXISTS) {
    ec = er_code;
    return result;
  }
  result.sem_ = han;
  return result;
}

void Semaphores::wait(std::error_code &ec) {
  if (WaitForSingleObject(sem_, INFINITE) == WAIT_FAILED) {
    ec = getErrorCode();
  }
}

bool Semaphores::tryWait(std::error_code &ec) { return tryWaitFor(0, ec); }

bool Semaphores::tryWaitFor(size_t timeout_ms, std::error_code &ec) {
  DWORD re = ::WaitForSingleObject(sem_, static_cast<DWORD>(timeout_ms));
  if (re == WAIT_FAILED) {
    ec = getErrorCode();
    return false;
  }
  if (re == WAIT_TIMEOUT) {
    return false;
  }
  return true;
}

void Semaphores::notifyOne(std::error_code &ec) {
  if (ReleaseSemaphore(sem_, 1, nullptr) == 0) {
    ec = getErrorCode();
    return;
  }
}

void Semaphores::close(std::error_code &ec) {
  if (sem_ == nullptr) {
    return;
  }

  if (CloseHandle(sem_) == 0) {
    ec = getErrorCode();
    return;
  }
  sem_ = nullptr;
}

void Semaphores::remove(std::error_code &ec) { close(ec); }

} // namespace ipc

#endif // _WIN32
