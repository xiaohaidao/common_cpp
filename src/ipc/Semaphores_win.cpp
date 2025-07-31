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

semaphores::semaphores() : sem_(nullptr) {}

semaphores::~semaphores() {}

semaphores semaphores::open(const char *key, std::error_code &ec) {
  semaphores result;
  std::string const key_g = std::string("Global\\") + key;
  HANDLE han = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, key_g.c_str());
  if (han == nullptr) {
    ec = get_error_code();
    return result;
  }
  result.sem_ = han;
  return result;
}

semaphores semaphores::create(const char *key, std::error_code &ec,
                              unsigned int number) {
  semaphores result;
  std::string const key_g = std::string("Global\\") + key;
  HANDLE han = CreateSemaphore(nullptr, (long)number, 256, key_g.c_str());
  std::error_code const er_code = get_error_code();
  if (han == nullptr || er_code.value() == ERROR_ALREADY_EXISTS) {
    ec = er_code;
    return result;
  }
  result.sem_ = han;
  return result;
}

void semaphores::wait(std::error_code &ec) {
  if (WaitForSingleObject(sem_, INFINITE) == WAIT_FAILED) {
    ec = get_error_code();
  }
}

bool semaphores::try_wait(std::error_code &ec) { return try_wait_for(0, ec); }

bool semaphores::try_wait_for(size_t timeout_ms, std::error_code &ec) {
  DWORD const re = ::WaitForSingleObject(sem_, static_cast<DWORD>(timeout_ms));
  if (re == WAIT_FAILED) {
    ec = get_error_code();
    return false;
  }
  if (re == WAIT_TIMEOUT) {
    return false;
  }
  return true;
}

void semaphores::notify_one(std::error_code &ec) {
  if (ReleaseSemaphore(sem_, 1, nullptr) == 0) {
    ec = get_error_code();
    return;
  }
}

void semaphores::close(std::error_code &ec) {
  if (sem_ == nullptr) {
    return;
  }

  if (CloseHandle(sem_) == 0) {
    ec = get_error_code();
    return;
  }
  sem_ = nullptr;
}

void semaphores::remove(std::error_code &ec) { close(ec); }

} // namespace ipc

#endif // _WIN32
