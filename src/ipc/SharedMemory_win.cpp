// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202211

/**
 * @file
 *
 *
 * @brief shared memory of windows
 *
 * 参看：https://learn.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory
 *
 */

#ifdef _WIN32

#include "ipc/SharedMemory.h"

#include <windows.h>

#include "utils/error_code.h"
#include "utils/macro.h"

namespace ipc {

SharedMemory::~SharedMemory() {
  // std::error_code ec;
  // close(ec);
}

SharedMemory SharedMemory::open(const char *key, std::error_code &ec) {
  SharedMemory result;

  std::string key_g = std::string("Global\\") + key;
  HANDLE map_file = OpenFileMapping(FILE_MAP_ALL_ACCESS, // read/write access
                                    false, // do not inherit the name
                                    key_g.c_str());
  if (map_file == nullptr) {
    ec = getErrorCode();
    return result;
  }
  result.shmid_ = map_file;
  result.attach(ec);

  return result;
}

SharedMemory SharedMemory::create(const char *key, size_t mem_size,
                                  std::error_code &ec) {

  std::string key_g = std::string("Global\\") + key;
  HANDLE map_file = CreateFileMapping(
      INVALID_HANDLE_VALUE,         // use paging file
      NULL,                         // default security
      PAGE_READWRITE,               // read/write access
      0,                            // maximum object size (high-order DWORD)
      static_cast<DWORD>(mem_size), // maximum object size (low-order DWORD)
      key_g.c_str());               // name of mapping object

  SharedMemory result;
  std::error_code er = getErrorCode();
  if (map_file == nullptr || er.value() == ERROR_ALREADY_EXISTS) {
    ec = er;
    return result;
  }
  result.shmid_ = map_file;
  result.size_ = mem_size;

  result.attach(ec);

  return result;
}

void SharedMemory::deatch(std::error_code &ec) {
  if (memory_ == nullptr) {
    return;
  }
  if (UnmapViewOfFile(memory_) == 0) {
    ec = getErrorCode();
    return;
  }

  memory_ = nullptr;
  size_ = 0;
}

void SharedMemory::attach(std::error_code &ec) {
  if ((memory_ = MapViewOfFile(shmid_,              // handle to map object
                               FILE_MAP_ALL_ACCESS, // read/write permission
                               0, 0, size_)) == nullptr) {
    ec = getErrorCode();
    memory_ = nullptr;
    size_ = 0;
    return;
  }
  if (size_ == 0) {
    MEMORY_BASIC_INFORMATION mbi = {0};
    if (VirtualQueryEx(GetCurrentProcess(), shmid_, &mbi, sizeof(mbi)) == 0) {
      ec = getErrorCode();
      return;
    }
    size_ = mbi.RegionSize;
  }
}

void SharedMemory::close(std::error_code &ec) {
  deatch(ec);
  if (shmid_ == nullptr) {
    return;
  }
  if (CloseHandle(shmid_) == 0) {
    ec = getErrorCode();
    return;
  }
  shmid_ = nullptr;
}

void SharedMemory::remove(std::error_code &ec) { CHECK_EC(ec, ); }

} // namespace ipc

#endif // _WIN32
